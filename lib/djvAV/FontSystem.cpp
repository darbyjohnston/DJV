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
#include <djvCore/ResourceSystem.h>
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
            Path fontPath;
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

            DJV_PRIVATE_PTR();
            p.fontPath = context->getSystemT<ResourceSystem>()->getPath(ResourcePath::FontsDirectory);
            p.measureCache.setMax(measureCacheMax);
            p.glyphCache.setMax(glyphCacheMax);
            p.dpi = dpi;

            p.statsTimer = Timer::create(context);
            p.statsTimer->setRepeating(true);
            p.statsTimer->start(
                Core::Timer::getMilliseconds(Core::Timer::Value::VerySlow),
                [this](float)
            {
                DJV_PRIVATE_PTR();
                std::lock_guard<std::mutex>(p.cacheMutex);
                std::stringstream s;
                s << "Measure cache: " << p.measureCache.getPercentageUsed() << "%\n";
                s << "Glyph cache: " << p.glyphCache.getPercentageUsed() << "%";
                _log(s.str());
            });
            
            p.running = true;
            p.thread = std::thread(
                [this]
            {
                DJV_PRIVATE_PTR();
                _initFreeType();
                const auto timeout = Core::Timer::getValue(Core::Timer::Value::Medium);
                while (p.running)
                {
                    {
                        std::unique_lock<std::mutex> lock(p.requestMutex);
                        p.requestCV.wait_for(
                            lock,
                            std::chrono::milliseconds(timeout),
                            [this]
                        {
                            DJV_PRIVATE_PTR();
                            return
                                p.metricsQueue.size() ||
                                p.measureQueue.size() ||
                                p.breakLinesQueue.size() ||
                                p.glyphsQueue.size();
                        });
                        p.metricsRequests = std::move(p.metricsQueue);
                        p.measureRequests = std::move(p.measureQueue);
                        p.breakLinesRequests = std::move(p.breakLinesQueue);
                        p.glyphsRequests = std::move(p.glyphsQueue);
                    }
                    if (p.metricsRequests.size())
                    {
                        _handleMetricsRequests();
                    }
                    if (p.measureRequests.size())
                    {
                        _handleMeasureRequests();
                    }
                    if (p.breakLinesRequests.size())
                    {
                        _handleBreakLinesRequests();
                    }
                    if (p.glyphsRequests.size())
                    {
                        _handleGlyphsRequests();
                    }
                }
                _delFreeType();
            });
        }

        FontSystem::FontSystem() :
            _p(new Private)
        {}

        FontSystem::~FontSystem()
        {
            DJV_PRIVATE_PTR();
            p.running = false;
            if (_p->thread.joinable())
            {
                p.thread.join();
            }
        }

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
            DJV_PRIVATE_PTR();
            MetricsRequest request;
            request.font = font;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(p.requestMutex);
            p.metricsQueue.push_back(std::move(request));
            p.requestCV.notify_one();
            return future;
        }

        std::future<glm::vec2> FontSystem::measure(const std::string& text, const Font& font)
        {
            DJV_PRIVATE_PTR();
            MeasureRequest request;
            request.text = text;
            request.font = font;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(p.requestMutex);
            p.measureQueue.push_back(std::move(request));
            p.requestCV.notify_one();
            return future;
        }

        std::future<glm::vec2> FontSystem::measure(const std::string& text, float maxLineWidth, const Font& font)
        {
            DJV_PRIVATE_PTR();
            MeasureRequest request;
            request.text = text;
            request.font = font;
            request.maxLineWidth = maxLineWidth;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(p.requestMutex);
            p.measureQueue.push_back(std::move(request));
            p.requestCV.notify_one();
            return future;
        }

        std::future<std::vector<FontLine> > FontSystem::breakLines(const std::string& text, float maxLineWidth, const Font& font)
        {
            DJV_PRIVATE_PTR();
            BreakLinesRequest request;
            request.text = text;
            request.font = font;
            request.maxLineWidth = maxLineWidth;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(p.requestMutex);
            p.breakLinesQueue.push_back(std::move(request));
            p.requestCV.notify_one();
            return future;
        }

        std::future<std::vector<std::shared_ptr<FontGlyph> > > FontSystem::getGlyphs(const std::string& text, const Font& font)
        {
            DJV_PRIVATE_PTR();
            GlyphsRequest request;
            request.text = text;
            request.font = font;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(p.requestMutex);
            p.glyphsQueue.push_back(std::move(request));
            p.requestCV.notify_one();
            return future;
        }

        void FontSystem::_initFreeType()
        {
            DJV_PRIVATE_PTR();
            try
            {
                FT_Error ftError = FT_Init_FreeType(&p.ftLibrary);
                if (ftError)
                {
                    throw std::runtime_error("Cannot initialize FreeType");
                }
                for (const auto& i : FileInfo::dirList(p.fontPath))
                {
                    const std::string& fileName = i.getFileName();

                    std::stringstream s;
                    s << "Loading font: " << fileName;
                    _log(s.str());

                    FT_Face ftFace;
                    ftError = FT_New_Face(p.ftLibrary, fileName.c_str(), 0, &ftFace);
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
                        p.fontFileNames[name] = fileName;
                        p.fontFaces[name] = ftFace;
                    }
                }
                if (!p.fontFaces.size())
                {
                    throw std::runtime_error("Cannot find any fonts");
                }
                p.fontFileNamesPromise.set_value(p.fontFileNames);
            }
            catch (const std::exception& e)
            {
                _log(e.what());
            }
        }

        void FontSystem::_delFreeType()
        {
            DJV_PRIVATE_PTR();
            if (p.ftLibrary)
            {
                for (const auto& i : p.fontFaces)
                {
                    FT_Done_Face(i.second);
                }
                FT_Done_FreeType(p.ftLibrary);
            }
        }

        void FontSystem::_handleMetricsRequests()
        {
            DJV_PRIVATE_PTR();
            for (auto& request : p.metricsRequests)
            {
                const auto font = p.fontFaces.find(request.font.name);
                if (font != p.fontFaces.end())
                {
                    FontMetrics metrics;
                    FT_Error ftError = FT_Set_Char_Size(
                        font->second,
                        0,
                        static_cast<int>(request.font.size * 64.f),
                        static_cast<int>(p.dpi.x),
                        static_cast<int>(p.dpi.y));
                    if (!ftError)
                    {
                        metrics.ascender = font->second->size->metrics.ascender / 64.f;
                        metrics.descender = font->second->size->metrics.descender / 64.f;
                        metrics.lineHeight = font->second->size->metrics.height / 64.f;
                    }
                    request.promise.set_value(std::move(metrics));
                }
            }
            p.metricsRequests.clear();
        }

        void FontSystem::_handleMeasureRequests()
        {
            DJV_PRIVATE_PTR();
            for (auto& request : p.measureRequests)
            {
                const auto font = p.fontFaces.find(request.font.name);
                if (font != p.fontFaces.end())
                {
                    FT_Error ftError = FT_Set_Char_Size(
                        font->second,
                        0,
                        static_cast<int>(request.font.size * 64.f),
                        static_cast<int>(p.dpi.x),
                        static_cast<int>(p.dpi.y));
                    if (!ftError)
                    {
                        const std::basic_string<djv_char_t> utf32 = p.utf32.from_bytes(request.text);
                        glm::vec2 size = glm::vec2(0.f, 0.f);
                        glm::vec2 pos = glm::vec2(0.f, font->second->size->metrics.height / 64.f);
                        for (const auto& c : utf32)
                        {
                            glm::vec2 glyphSize(0.f, 0.f);
                            {
                                const auto hash = getFontGlyphHash(c, request.font);
                                std::lock_guard<std::mutex>(p.cacheMutex);
                                if (p.measureCache.contains(hash))
                                {
                                    glyphSize = p.measureCache.get(hash);
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
                                    p.measureCache.add(hash, glyphSize);
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
            p.measureRequests.clear();
        }

        void FontSystem::_handleBreakLinesRequests()
        {
            DJV_PRIVATE_PTR();
            for (auto& request : p.breakLinesRequests)
            {
                const auto font = p.fontFaces.find(request.font.name);
                if (font != p.fontFaces.end())
                {
                    FT_Error ftError = FT_Set_Char_Size(
                        font->second,
                        0,
                        static_cast<int>(request.font.size * 64.f),
                        static_cast<int>(p.dpi.x),
                        static_cast<int>(p.dpi.y));
                    if (!ftError)
                    {
                        const std::basic_string<djv_char_t> utf32 = p.utf32.from_bytes(request.text);
                        std::basic_string<djv_char_t> line;
                        std::vector<FontLine> lines;
                        glm::vec2 pos = glm::vec2(0.f, font->second->size->metrics.height / 64.f);
                        for (const auto& c : utf32)
                        {
                            glm::vec2 glyphSize(0.f, 0.f);
                            {
                                const auto hash = getFontGlyphHash(c, request.font);
                                std::lock_guard<std::mutex>(p.cacheMutex);
                                if (p.measureCache.contains(hash))
                                {
                                    glyphSize = p.measureCache.get(hash);
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
                                    p.measureCache.add(hash, glyphSize);
                                    FT_Done_Glyph(ftGlyph);
                                }
                            }
                            if ((pos.x > 0 && pos.x + glyphSize.x > request.maxLineWidth) || '\n' == c)
                            {
                                lines.push_back(FontLine(
                                    p.utf32.to_bytes(line),
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
                                p.utf32.to_bytes(line),
                                glm::vec2(pos.x, font->second->size->metrics.height / 64.f)));
                        }
                        request.promise.set_value(lines);
                    }
                }
            }
            p.breakLinesRequests.clear();
        }

        void FontSystem::_handleGlyphsRequests()
        {
            DJV_PRIVATE_PTR();
            for (auto& request : p.glyphsRequests)
            {
                const auto font = p.fontFaces.find(request.font.name);
                if (font != p.fontFaces.end())
                {
                    FT_Error ftError = FT_Set_Char_Size(
                        font->second,
                        0,
                        static_cast<int>(request.font.size * 64.f),
                        static_cast<int>(p.dpi.x),
                        static_cast<int>(p.dpi.y));
                    if (!ftError)
                    {
                        const std::basic_string<djv_char_t> utf32 = p.utf32.from_bytes(request.text);
                        std::vector<std::shared_ptr<FontGlyph> > glyphs;
                        glyphs.reserve(utf32.size());
                        for (const auto& c : utf32)
                        {
                            std::shared_ptr<FontGlyph> glyph;
                            {
                                const auto hash = getFontGlyphHash(c, request.font);
                                std::lock_guard<std::mutex>(p.cacheMutex);
                                if (p.glyphCache.contains(hash))
                                {
                                    glyph = p.glyphCache.get(hash);
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
                                    for (int y = 0; y < info.size.y; ++y)
                                    {
                                        memcpy(data->getData(info.size.y - 1 - y), bitmap->bitmap.buffer + y * info.size.x, info.size.x);
                                    }
                                    glyph->pixelData = data;
                                    glyph->offset = glm::vec2(font->second->glyph->bitmap_left, font->second->glyph->bitmap_top);
                                    glyph->advance = font->second->glyph->advance.x / 64.f;
                                    p.glyphCache.add(hash, glyph);
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
            p.glyphsRequests.clear();
        }

    } // namespace AV
} // namespace djv
