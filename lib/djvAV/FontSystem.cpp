//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAV/FontSystem.h>

#include <djvCore/Cache.h>
#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/Timer.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <atomic>
#include <codecvt>
#include <condition_variable>
#include <cwctype>
#include <locale>
#include <mutex>
#include <thread>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace
        {
            //! \todo [1.0 S] Should this be configurable?
            const size_t measureCacheMax = 100000;
            const size_t glyphCacheMax = 10000;
            const size_t timeout = 100;
            const size_t statsTimeout = 10000;

            struct MetricsRequest
            {
                MetricsRequest() {}
                MetricsRequest(MetricsRequest&& other) = default;
                MetricsRequest& operator = (MetricsRequest&&) = default;

                Font font;
                std::promise<FontMetrics> promise;
            };

            struct MeasureRequest
            {
                MeasureRequest() {}
                MeasureRequest(MeasureRequest&&) = default;
                MeasureRequest& operator = (MeasureRequest&& other) = default;

                std::string text;
                Font font;
                float maxLineWidth = std::numeric_limits<float>::max();
                std::promise<glm::vec2> promise;
            };

            struct BreakLinesRequest
            {
                BreakLinesRequest() {}
                BreakLinesRequest(BreakLinesRequest&&) = default;
                BreakLinesRequest& operator = (BreakLinesRequest&&) = default;

                std::string text;
                Font font;
                float maxLineWidth = std::numeric_limits<float>::max();
                std::promise<std::vector<FontLine> > promise;
            };

            struct GlyphsRequest
            {
                GlyphsRequest() {}
                GlyphsRequest(GlyphsRequest&&) = default;
                GlyphsRequest& operator = (GlyphsRequest&&) = default;

                std::string text;
                Font font;
                std::promise<std::vector<std::shared_ptr<FontGlyph> > > promise;
            };

        } // namespace

        Font::Font()
        {}

        Font::Font(const std::string& name, float size) :
            name(name),
            size(size)
        {}

        FontLine::FontLine()
        {}

        FontLine::FontLine(const std::string& text, const glm::vec2& size) :
            text(text),
            size(size)
        {}

        FontGlyphHash getFontGlyphHash(uint32_t code, const Font& font)
        {
            FontGlyphHash out = 0;
            Memory::hashCombine(out, code);
            Memory::hashCombine(out, font.name);
            Memory::hashCombine(out, font.size);
            return out;
        }

        struct FontSystem::Private
        {
            glm::vec2 dpi = glm::vec2(0.f, 0.f);
            FT_Library ftLibrary = nullptr;
            std::map<std::string, std::string> fontFileNames;
            std::promise<std::map<std::string, std::string> > fontFileNamesPromise;
            std::map<std::string, FT_Face> fontFaces;

            std::vector<MetricsRequest> metricsQueue;
            std::vector<MeasureRequest> measureQueue;
            std::vector<BreakLinesRequest> breakLinesQueue;
            std::vector<GlyphsRequest> glyphsQueue;
            std::condition_variable requestCV;
            std::mutex requestMutex;
            std::vector<MetricsRequest> metricsRequests;
            std::vector<MeasureRequest> measureRequests;
            std::vector<BreakLinesRequest> breakLinesRequests;
            std::vector<GlyphsRequest> glyphsRequests;

            std::wstring_convert<std::codecvt_utf8<djv_char_t>, djv_char_t> utf32;
            Cache<FontGlyphHash, glm::vec2> measureCache;
            Cache<FontGlyphHash, std::shared_ptr<FontGlyph> > glyphCache;
            std::mutex cacheMutex;

            std::shared_ptr<Timer> statsTimer;
            std::thread thread;
            std::atomic<bool> running;
        };

        void FontSystem::_init(const glm::vec2& dpi, const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::AV::FontSystem", context);

            _p->measureCache.setMax(measureCacheMax);
            _p->glyphCache.setMax(glyphCacheMax);
            _p->dpi = dpi;

            _p->statsTimer = Timer::create(context);
            _p->statsTimer->setRepeating(true);
            _p->statsTimer->start(
                std::chrono::milliseconds(statsTimeout),
                [this](float)
            {
                std::lock_guard<std::mutex>(_p->cacheMutex);
                std::stringstream s;
                s << "Measure cache: " << _p->measureCache.getPercentageUsed() << "%\n";
                s << "Glyph cache: " << _p->glyphCache.getPercentageUsed() << "%";
                _log(s.str());
            });
            
            _p->running = true;
            _p->thread = std::thread(
                [this]
            {
                _initFreeType();
                while (_p->running)
                {
                    {
                        std::unique_lock<std::mutex> lock(_p->requestMutex);
                        _p->requestCV.wait_for(
                            lock,
                            std::chrono::milliseconds(timeout),
                            [this]
                        {
                            return
                                _p->metricsQueue.size() ||
                                _p->measureQueue.size() ||
                                _p->breakLinesQueue.size() ||
                                _p->glyphsQueue.size();
                        });
                        _p->metricsRequests = std::move(_p->metricsQueue);
                        _p->measureRequests = std::move(_p->measureQueue);
                        _p->breakLinesRequests = std::move(_p->breakLinesQueue);
                        _p->glyphsRequests = std::move(_p->glyphsQueue);
                    }
                    _handleMetricsRequests();
                    _handleMeasureRequests();
                    _handleBreakLinesRequests();
                    _handleGlyphsRequests();
                }
                _delFreeType();
            });
        }

        FontSystem::FontSystem() :
            _p(new Private)
        {}

        FontSystem::~FontSystem()
        {}

        std::shared_ptr<FontSystem> FontSystem::create(const glm::vec2& dpi, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<FontSystem>(new FontSystem);
            out->_init(dpi, context);
            return out;
        }
        
        std::future<std::map<std::string, std::string> > FontSystem::getFileNames()
        {
            auto future = _p->fontFileNamesPromise.get_future();
            return future;
        }
        
        std::future<FontMetrics> FontSystem::getMetrics(const Font& font)
        {
            MetricsRequest request;
            request.font = font;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(_p->requestMutex);
            _p->metricsQueue.push_back(std::move(request));
            _p->requestCV.notify_one();
            return future;
        }

        std::future<glm::vec2> FontSystem::measure(const std::string& text, const Font& font)
        {
            MeasureRequest request;
            request.text = text;
            request.font = font;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(_p->requestMutex);
            _p->measureQueue.push_back(std::move(request));
            _p->requestCV.notify_one();
            return future;
        }

        std::future<glm::vec2> FontSystem::measure(const std::string& text, float maxLineWidth, const Font& font)
        {
            MeasureRequest request;
            request.text = text;
            request.font = font;
            request.maxLineWidth = maxLineWidth;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(_p->requestMutex);
            _p->measureQueue.push_back(std::move(request));
            _p->requestCV.notify_one();
            return future;
        }

        std::future<std::vector<FontLine> > FontSystem::breakLines(const std::string& text, float maxLineWidth, const Font& font)
        {
            BreakLinesRequest request;
            request.text = text;
            request.font = font;
            request.maxLineWidth = maxLineWidth;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(_p->requestMutex);
            _p->breakLinesQueue.push_back(std::move(request));
            _p->requestCV.notify_one();
            return future;
        }

        std::future<std::vector<std::shared_ptr<FontGlyph> > > FontSystem::getGlyphs(const std::string& text, const Font& font)
        {
            GlyphsRequest request;
            request.text = text;
            request.font = font;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(_p->requestMutex);
            _p->glyphsQueue.push_back(std::move(request));
            _p->requestCV.notify_one();
            return future;
        }

        void FontSystem::_exit()
        {
            ISystem::_exit();
            {
                std::unique_lock<std::mutex> lock(_p->requestMutex);
                _p->metricsQueue.clear();
                _p->measureQueue.clear();
                _p->breakLinesQueue.clear();
                _p->glyphsQueue.clear();
            }
            _p->running = false;
            _p->thread.join();
        }

        void FontSystem::_initFreeType()
        {
            try
            {
                FT_Error ftError = FT_Init_FreeType(&_p->ftLibrary);
                if (ftError)
                {
                    throw std::runtime_error("Cannot initialize FreeType");
                }
                if (auto context = getContext().lock())
                {
                    for (const auto& i : FileInfo::dirList(context->getResourcePath(ResourcePath::FontsDirectory)))
                    {
                        const std::string& fileName = i.getFileName();
                        std::stringstream s;
                        s << "Loading font: " << fileName;
                        _log(s.str());
                        FT_Face ftFace;
                        ftError = FT_New_Face(_p->ftLibrary, fileName.c_str(), 0, &ftFace);
                        if (ftError)
                        {
                            s = std::stringstream();
                            s << "Cannot load font: " << fileName;
                            _log(s.str(), LogLevel::Error);
                        }
                        else
                        {
                            s = std::stringstream();
                            s << "    Family: " << ftFace->family_name << '\n';
                            s << "    Style: " << ftFace->style_name << '\n';
                            s << "    Number of glyphs: " << static_cast<int>(ftFace->num_glyphs) << '\n';
                            s << "    Scalable: " << (FT_IS_SCALABLE(ftFace) ? "true" : "false") << '\n';
                            s << "    Kerning: " << (FT_HAS_KERNING(ftFace) ? "true" : "false");
                            _log(s.str());
                            const std::string& name = i.getPath().getBaseName();
                            _p->fontFileNames[name] = fileName;
                            _p->fontFaces[name] = ftFace;
                        }
                    }
                }
                if (!_p->fontFaces.size())
                {
                    throw std::runtime_error("Cannot find any fonts");
                }
                _p->fontFileNamesPromise.set_value(_p->fontFileNames);
            }
            catch (const std::exception& e)
            {
                _log(e.what());
            }
        }

        void FontSystem::_delFreeType()
        {
            if (_p->ftLibrary)
            {
                for (const auto& i : _p->fontFaces)
                {
                    FT_Done_Face(i.second);
                }
                FT_Done_FreeType(_p->ftLibrary);
            }
        }

        void FontSystem::_handleMetricsRequests()
        {
            for (auto& request : _p->metricsRequests)
            {
                const auto font = _p->fontFaces.find(request.font.name);
                if (font != _p->fontFaces.end())
                {
                    FontMetrics metrics;
                    FT_Error ftError = FT_Set_Char_Size(
                        font->second,
                        0,
                        static_cast<int>(request.font.size * 64.f),
                        static_cast<int>(_p->dpi.x),
                        static_cast<int>(_p->dpi.y));
                    if (!ftError)
                    {
                        metrics.ascender = font->second->size->metrics.ascender / 64.f;
                        metrics.descender = font->second->size->metrics.descender / 64.f;
                        metrics.lineHeight = font->second->size->metrics.height / 64.f;
                    }
                    request.promise.set_value(std::move(metrics));
                }
            }
            _p->metricsRequests.clear();
        }

        void FontSystem::_handleMeasureRequests()
        {
            for (auto& request : _p->measureRequests)
            {
                const auto font = _p->fontFaces.find(request.font.name);
                if (font != _p->fontFaces.end())
                {
                    FT_Error ftError = FT_Set_Char_Size(
                        font->second,
                        0,
                        static_cast<int>(request.font.size * 64.f),
                        static_cast<int>(_p->dpi.x),
                        static_cast<int>(_p->dpi.y));
                    if (!ftError)
                    {
                        const std::basic_string<djv_char_t> utf32 = _p->utf32.from_bytes(request.text);
                        glm::vec2 size = glm::vec2(0.f, 0.f);
                        glm::vec2 pos = glm::vec2(0.f, font->second->size->metrics.height / 64.f);
                        for (const auto& c : utf32)
                        {
                            glm::vec2 glyphSize(0.f, 0.f);
                            {
                                const auto hash = getFontGlyphHash(c, request.font);
                                std::lock_guard<std::mutex>(_p->cacheMutex);
                                if (_p->measureCache.contains(hash))
                                {
                                    glyphSize = _p->measureCache.get(hash);
                                }
                                else if (FT_UInt ftGlyphIndex = FT_Get_Char_Index(font->second, c))
                                {
                                    ftError = FT_Load_Glyph(font->second, ftGlyphIndex, FT_LOAD_DEFAULT);
                                    if (ftError)
                                    {
                                        continue;
                                    }
                                    FT_Glyph ftGlyph;
                                    ftError = FT_Get_Glyph(font->second->glyph, &ftGlyph);
                                    if (ftError)
                                    {
                                        continue;
                                    }
                                    glyphSize.x = font->second->glyph->advance.x / 64.f;
                                    glyphSize.y = font->second->glyph->metrics.height / 64.f;
                                    _p->measureCache.add(hash, glyphSize);
                                    FT_Done_Glyph(ftGlyph);
                                }
                            }
                            if ((pos.x > 0 && pos.x + glyphSize.x > request.maxLineWidth) || '\n' == c)
                            {
                                size.x = request.maxLineWidth;
                                pos.x = 0;
                                pos.y += font->second->size->metrics.height / 64.f;
                            }
                            pos.x += glyphSize.x;
                        }
                        size.x = std::max(size.x, pos.x);
                        size.y = pos.y;
                        request.promise.set_value(size);
                    }
                }
            }
            _p->measureRequests.clear();
        }

        void FontSystem::_handleBreakLinesRequests()
        {
            for (auto& request : _p->breakLinesRequests)
            {
                const auto font = _p->fontFaces.find(request.font.name);
                if (font != _p->fontFaces.end())
                {
                    FT_Error ftError = FT_Set_Char_Size(
                        font->second,
                        0,
                        static_cast<int>(request.font.size * 64.f),
                        static_cast<int>(_p->dpi.x),
                        static_cast<int>(_p->dpi.y));
                    if (!ftError)
                    {
                        const std::basic_string<djv_char_t> utf32 = _p->utf32.from_bytes(request.text);
                        std::basic_string<djv_char_t> line;
                        std::vector<FontLine> lines;
                        glm::vec2 pos = glm::vec2(0.f, font->second->size->metrics.height / 64.f);
                        for (const auto& c : utf32)
                        {
                            glm::vec2 glyphSize(0.f, 0.f);
                            {
                                const auto hash = getFontGlyphHash(c, request.font);
                                std::lock_guard<std::mutex>(_p->cacheMutex);
                                if (_p->measureCache.contains(hash))
                                {
                                    glyphSize = _p->measureCache.get(hash);
                                }
                                else if (FT_UInt ftGlyphIndex = FT_Get_Char_Index(font->second, c))
                                {
                                    ftError = FT_Load_Glyph(font->second, ftGlyphIndex, FT_LOAD_DEFAULT);
                                    if (ftError)
                                    {
                                        continue;
                                    }
                                    FT_Glyph ftGlyph;
                                    ftError = FT_Get_Glyph(font->second->glyph, &ftGlyph);
                                    if (ftError)
                                    {
                                        continue;
                                    }
                                    glyphSize.x = font->second->glyph->advance.x / 64.f;
                                    glyphSize.y = font->second->glyph->metrics.height / 64.f;
                                    _p->measureCache.add(hash, glyphSize);
                                    FT_Done_Glyph(ftGlyph);
                                }
                            }
                            if ((pos.x > 0 && pos.x + glyphSize.x > request.maxLineWidth) || '\n' == c)
                            {
                                lines.push_back(FontLine(
                                    _p->utf32.to_bytes(line),
                                    glm::vec2(request.maxLineWidth, font->second->size->metrics.height / 64.f)));
                                line.clear();
                                pos.x = 0;
                                pos.y += font->second->size->metrics.height / 64.f;
                            }
                            line.push_back(c);
                            pos.x += glyphSize.x;
                        }
                        if (line.size())
                        {
                            lines.push_back(FontLine(
                                _p->utf32.to_bytes(line),
                                glm::vec2(pos.x, font->second->size->metrics.height / 64.f)));
                        }
                        request.promise.set_value(lines);
                    }
                }
            }
            _p->breakLinesRequests.clear();
        }

        void FontSystem::_handleGlyphsRequests()
        {
            for (auto& request : _p->glyphsRequests)
            {
                const auto font = _p->fontFaces.find(request.font.name);
                if (font != _p->fontFaces.end())
                {
                    FT_Error ftError = FT_Set_Char_Size(
                        font->second,
                        0,
                        static_cast<int>(request.font.size * 64.f),
                        static_cast<int>(_p->dpi.x),
                        static_cast<int>(_p->dpi.y));
                    if (!ftError)
                    {
                        const std::basic_string<djv_char_t> utf32 = _p->utf32.from_bytes(request.text);
                        std::vector<std::shared_ptr<FontGlyph> > glyphs;
                        glyphs.reserve(utf32.size());
                        for (const auto& c : utf32)
                        {
                            std::shared_ptr<FontGlyph> glyph;
                            {
                                const auto hash = getFontGlyphHash(c, request.font);
                                std::lock_guard<std::mutex>(_p->cacheMutex);
                                if (_p->glyphCache.contains(hash))
                                {
                                    glyph = _p->glyphCache.get(hash);
                                }
                                else if (FT_UInt ftGlyphIndex = FT_Get_Char_Index(font->second, c))
                                {
                                    ftError = FT_Load_Glyph(font->second, ftGlyphIndex, FT_LOAD_RENDER);
                                    if (ftError)
                                    {
                                        continue;
                                    }
                                    FT_Glyph ftGlyph;
                                    ftError = FT_Get_Glyph(font->second->glyph, &ftGlyph);
                                    if (ftError)
                                    {
                                        continue;
                                    }
                                    FT_Vector v;
                                    v.x = 0;
                                    v.y = 0;
                                    ftError = FT_Glyph_To_Bitmap(&ftGlyph, FT_RENDER_MODE_NORMAL, &v, 0);
                                    if (ftError)
                                    {
                                        FT_Done_Glyph(ftGlyph);
                                        continue;
                                    }
                                    FT_BitmapGlyph bitmap = (FT_BitmapGlyph)ftGlyph;
                                    glyph = std::shared_ptr<FontGlyph>(new FontGlyph);
                                    glyph->code = c;
                                    glyph->font = request.font;
                                    Pixel::Info info = Pixel::Info(bitmap->bitmap.width, bitmap->bitmap.rows, Pixel::Type::L_U8);
                                    auto data = Pixel::Data::create(Pixel::Info(bitmap->bitmap.width, bitmap->bitmap.rows, Pixel::Type::L_U8));
                                    const auto & size = info.getSize();
                                    for (int y = 0; y < size.y; ++y)
                                    {
                                        memcpy(data->getData(size.y - 1 - y), bitmap->bitmap.buffer + y * size.x, size.x);
                                    }
                                    glyph->pixelData = data;
                                    glyph->offset = glm::vec2(font->second->glyph->bitmap_left, font->second->glyph->bitmap_top);
                                    glyph->advance = font->second->glyph->advance.x / 64.f;
                                    _p->glyphCache.add(hash, glyph);
                                    FT_Done_Glyph(ftGlyph);
                                }
                            }
                            if (glyph)
                            {
                                glyphs.push_back(glyph);
                            }
                        }
                        request.promise.set_value(std::move(glyphs));
                    }
                }
            }
            _p->glyphsRequests.clear();
        }

    } // namespace AV
} // namespace djv
