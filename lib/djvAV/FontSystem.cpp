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
#include <djvCore/Vector.h>

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
        namespace Font
        {
            namespace
            {
                //! \todo [1.0 S] Should this be configurable?
                //const size_t measureCacheMax = 10000;
                //const size_t glyphCacheMax = 10000;

                struct MetricsRequest
                {
                    MetricsRequest() {}
                    MetricsRequest(MetricsRequest&& other) = default;
                    MetricsRequest& operator = (MetricsRequest&&) = default;

                    Info info;
                    std::promise<Metrics> promise;
                };

                struct MeasureRequest
                {
                    MeasureRequest() {}
                    MeasureRequest(MeasureRequest&&) = default;
                    MeasureRequest& operator = (MeasureRequest&& other) = default;

                    std::string text;
                    Info info;
                    float maxLineWidth = std::numeric_limits<float>::max();
                    std::promise<glm::vec2> promise;
                };

                struct BreakLinesRequest
                {
                    BreakLinesRequest() {}
                    BreakLinesRequest(BreakLinesRequest&&) = default;
                    BreakLinesRequest& operator = (BreakLinesRequest&&) = default;

                    std::string text;
                    Info info;
                    float maxLineWidth = std::numeric_limits<float>::max();
                    std::promise<std::vector<TextLine> > promise;
                };

                struct GlyphsRequest
                {
                    GlyphsRequest() {}
                    GlyphsRequest(GlyphsRequest&&) = default;
                    GlyphsRequest& operator = (GlyphsRequest&&) = default;

                    std::string text;
                    Info info;
                    std::promise<std::vector<std::shared_ptr<Glyph> > > promise;
                };

                inline bool isSpace(djv_char_t c)
                {
                    return ' ' == c || '\t' == c;
                }

                inline bool isNewline(djv_char_t c)
                {
                    return '\n' == c || '\r' == c;
                }

                /*#undef FTERRORS_H_
                #define FT_ERRORDEF( e, v, s )  { e, s },
                #define FT_ERROR_START_LIST     {
                #define FT_ERROR_END_LIST       { 0, NULL } };
                            const struct
                            {
                                int          code;
                                const char * message;
                            } ftErrors[] =
                #include FT_ERRORS_H
                            std::string getFTError(FT_Error code)
                            {
                                for (auto i : ftErrors)
                                {
                                    if (code == i.code)
                                    {
                                        return i.message;
                                    }
                                }
                                return std::string();
                            }*/

            } // namespace

            const std::string Info::familyDefault = "Noto Sans";
            const std::string Info::faceDefault = "Regular";

            Info::Info()
            {}

            Info::Info(const std::string& family, const std::string& face, float size, int dpi) :
                family(family),
                face(face),
                size(size),
                dpi(dpi)
            {}

            TextLine::TextLine()
            {}

            TextLine::TextLine(const std::string& text, const glm::vec2& size) :
                text(text),
                size(size)
            {}

            GlyphInfo::GlyphInfo()
            {}

            GlyphInfo::GlyphInfo(uint32_t code, const Info & info) :
                _code(code),
                _info(info)
            {
                static std::map<size_t, UID> map;
                size_t hash = 0;
                Memory::hashCombine(hash, info.dpi);
                Memory::hashCombine(hash, info.size);
                Memory::hashCombine(hash, info.face);
                Memory::hashCombine(hash, info.family);
                Memory::hashCombine(hash, code);
                const auto i = map.find(hash);
                if (i != map.end())
                {
                    _uid = i->second;
                }
                else
                {
                    _uid = createUID();
                    map[hash] = _uid;
                }
            }

            void Glyph::_init(
                const GlyphInfo & info,
                const std::shared_ptr<Image::Data> & imageData,
                const glm::vec2 & offset,
                float advance)
            {
                _info = info;
                _imageData = imageData;
                _offset = offset;
                _advance = advance;
            }

            Glyph::Glyph()
            {}

            std::shared_ptr<Glyph> Glyph::create(
                const GlyphInfo & info,
                const std::shared_ptr<Image::Data> & imageData,
                const glm::vec2 & offset,
                float advance)
            {
                auto out = std::shared_ptr<Glyph>(new Glyph);
                out->_init(info, imageData, offset, advance);
                return out;
            }

            struct System::Private
            {
                FT_Library ftLibrary = nullptr;
                FileSystem::Path fontPath;
                std::map<std::string, std::string> fontNames;
                std::promise<std::map<std::string, std::string> > fontNamesPromise;
                std::map<std::string, std::map<std::string, FT_Face> > fontFaces;

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
                //! \todo [1.0 S] The Cache class provides an upper limit but using a map is much faster.
                //Cache<UID, glm::vec2> measureCache;
                //Cache<UID, std::shared_ptr<Glyph> > glyphCache;
                std::map<UID, glm::vec2> measureCache;
                std::map<UID, std::shared_ptr<Glyph> > glyphCache;
                std::mutex cacheMutex;

                std::shared_ptr<Time::Timer> statsTimer;
                std::thread thread;
                std::atomic<bool> running;

                std::vector<glm::vec2> getGlyphSizes(const std::basic_string<djv_char_t> &, const Info &, const FT_Face &);
            };

            void System::_init(Context * context)
            {
                ISystem::_init("djv::AV::Font::System", context);

                DJV_PRIVATE_PTR();
                p.fontPath = context->getPath(FileSystem::ResourcePath::FontsDirectory);
                //p.measureCache.setMax(measureCacheMax);
                //p.glyphCache.setMax(glyphCacheMax);

                p.statsTimer = Time::Timer::create(context);
                p.statsTimer->setRepeating(true);
                p.statsTimer->start(
                    Time::Timer::getMilliseconds(Time::Timer::Value::VerySlow),
                    [this](float)
                {
                    DJV_PRIVATE_PTR();
                    std::lock_guard<std::mutex> lock(p.cacheMutex);
                    std::stringstream s;
                    //s << "Measure cache: " << p.measureCache.getPercentageUsed() << "%\n";
                    //s << "Glyph cache: " << p.glyphCache.getPercentageUsed() << "%";
                    s << "Measure cache: " << p.measureCache.size() << "\n";
                    s << "Glyph cache: " << p.glyphCache.size();
                    _log(s.str());
                });

                p.running = true;
                p.thread = std::thread(
                    [this]
                {
                    DJV_PRIVATE_PTR();
                    _initFreeType();
                    const auto timeout = Time::Timer::getValue(Time::Timer::Value::Medium);
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

            System::System() :
                _p(new Private)
            {}

            System::~System()
            {
                DJV_PRIVATE_PTR();
                p.running = false;
                if (_p->thread.joinable())
                {
                    p.thread.join();
                }
            }

            std::shared_ptr<System> System::create(Context * context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(context);
                return out;
            }
            
            std::future<std::map<std::string, std::string> > System::getFontNames()
            {
                auto future = _p->fontNamesPromise.get_future();
                return future;
            }

            std::future<Metrics> System::getMetrics(const Info& info)
            {
                DJV_PRIVATE_PTR();
                MetricsRequest request;
                request.info = info;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.metricsQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            std::future<glm::vec2> System::measure(const std::string& text, const Info& info)
            {
                DJV_PRIVATE_PTR();
                MeasureRequest request;
                request.text = text;
                request.info = info;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.measureQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            std::future<glm::vec2> System::measure(const std::string& text, float maxLineWidth, const Info& info)
            {
                DJV_PRIVATE_PTR();
                MeasureRequest request;
                request.text = text;
                request.info = info;
                request.maxLineWidth = maxLineWidth;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.measureQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            std::future<std::vector<TextLine> > System::breakLines(const std::string& text, float maxLineWidth, const Info& info)
            {
                DJV_PRIVATE_PTR();
                BreakLinesRequest request;
                request.text         = text;
                request.info         = info;
                request.maxLineWidth = maxLineWidth;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.breakLinesQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            std::future<std::vector<std::shared_ptr<Glyph> > > System::getGlyphs(const std::string& text, const Info& info)
            {
                DJV_PRIVATE_PTR();
                GlyphsRequest request;
                request.text = text;
                request.info = info;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.glyphsQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            void System::_initFreeType()
            {
                DJV_PRIVATE_PTR();
                try
                {
                    FT_Error ftError = FT_Init_FreeType(&p.ftLibrary);
                    if (ftError)
                    {
                        throw std::runtime_error(_getText(DJV_TEXT("djv::AV::Font", "Cannot initialize FreeType.")));
                    }
                    for (const auto& i : FileSystem::FileInfo::directoryList(p.fontPath))
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
                            p.fontNames[ftFace->family_name] = fileName;
                            p.fontFaces[ftFace->family_name][ftFace->style_name] = ftFace;
                        }
                    }
                    if (!p.fontFaces.size())
                    {
                        throw std::runtime_error(_getText(DJV_TEXT("djv::AV::Font", "Cannot find any fonts.")));
                    }
                    p.fontNamesPromise.set_value(p.fontNames);
                }
                catch (const std::exception& e)
                {
                    _log(e.what());
                }
            }

            void System::_delFreeType()
            {
                DJV_PRIVATE_PTR();
                if (p.ftLibrary)
                {
                    for (const auto& i : p.fontFaces)
                    {
                        for (const auto & j : i.second)
                        {
                            FT_Done_Face(j.second);
                        }
                    }
                    FT_Done_FreeType(p.ftLibrary);
                }
            }

            void System::_handleMetricsRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto& request : p.metricsRequests)
                {
                    const auto family = p.fontFaces.find(request.info.family);
                    if (family != p.fontFaces.end())
                    {
                        const auto font = family->second.find(request.info.face);
                        if (font != family->second.end())
                        {
                            Metrics metrics;
                            FT_Error ftError = FT_Set_Char_Size(
                                font->second,
                                0,
                                static_cast<int>(request.info.size * 64.f),
                                request.info.dpi,
                                request.info.dpi);
                            if (!ftError)
                            {
                                metrics.ascender = font->second->size->metrics.ascender / 64.f;
                                metrics.descender = font->second->size->metrics.descender / 64.f;
                                metrics.lineHeight = font->second->size->metrics.height / 64.f;
                            }
                            request.promise.set_value(std::move(metrics));
                        }
                    }
                }
                p.metricsRequests.clear();
            }

            void System::_handleMeasureRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto& request : p.measureRequests)
                {
                    const auto family = p.fontFaces.find(request.info.family);
                    if (family != p.fontFaces.end())
                    {
                        const auto font = family->second.find(request.info.face);
                        if (font != family->second.end())
                        {
                            FT_Error ftError = FT_Set_Char_Size(
                                font->second,
                                0,
                                static_cast<int>(request.info.size * 64.f),
                                request.info.dpi,
                                request.info.dpi);
                            if (ftError)
                            {
                                continue;
                            }
                            const std::basic_string<djv_char_t> utf32 = p.utf32.from_bytes(request.text);
                            const auto utf32Begin = utf32.begin();
                            glm::vec2 size = glm::vec2(0.f, 0.f);
                            glm::vec2 pos = glm::vec2(0.f, font->second->size->metrics.height / 64.f);
                            auto breakLine = utf32.end();
                            float breakLineX = 0.f;
                            const auto glyphSizes = p.getGlyphSizes(utf32, request.info, font->second);
                            for (auto i = utf32.begin(); i != utf32.end(); ++i)
                            {
                                const auto & glyphSize = glyphSizes[i - utf32Begin];
                                if (isNewline(*i))
                                {
                                    size.x = std::max(size.x, pos.x);
                                    pos.x = 0.f;
                                    pos.y += font->second->size->metrics.height / 64.f;
                                }
                                else if (pos.x > 0.f && pos.x + (!isSpace(*i) ? glyphSize.x : 0) >= request.maxLineWidth)
                                {
                                    if (breakLine != utf32.end())
                                    {
                                        i = breakLine;
                                        breakLine = utf32.end();
                                        size.x = std::max(size.x, breakLineX);
                                        pos.x = 0.f;
                                        pos.y += font->second->size->metrics.height / 64.f;
                                    }
                                    else
                                    {
                                        size.x = std::max(size.x, pos.x);
                                        pos.x = glyphSize.x;
                                        pos.y += font->second->size->metrics.height / 64.f;
                                    }
                                }
                                else
                                {
                                    if (isSpace(*i) && i != utf32.begin())
                                    {
                                        breakLine = i;
                                        breakLineX = pos.x;
                                    }
                                    pos.x += glyphSize.x;
                                }
                            }
                            size.x = std::max(size.x, pos.x);
                            size.y = pos.y;
                            request.promise.set_value(size);
                        }
                    }
                }
                p.measureRequests.clear();
            }

            void System::_handleBreakLinesRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto& request : p.breakLinesRequests)
                {
                    const auto family = p.fontFaces.find(request.info.family);
                    if (family != p.fontFaces.end())
                    {
                        const auto font = family->second.find(request.info.face);
                        if (font != family->second.end())
                        {
                            FT_Error ftError = FT_Set_Char_Size(
                                font->second,
                                0,
                                static_cast<int>(request.info.size * 64.f),
                                request.info.dpi,
                                request.info.dpi);
                            if (ftError)
                            {
                                continue;
                            }
                            const std::basic_string<djv_char_t> utf32 = p.utf32.from_bytes(request.text);
                            const auto utf32Begin = utf32.begin();
                            std::vector<TextLine> lines;
                            glm::vec2 pos = glm::vec2(0.f, font->second->size->metrics.height / 64.f);
                            auto lineBegin = utf32Begin;
                            auto breakLine = utf32.end();
                            float breakLineX = 0.f;
                            const auto glyphSizes = p.getGlyphSizes(utf32, request.info, font->second);
                            auto i = utf32.begin();
                            for (; i != utf32.end(); ++i)
                            {
                                const auto & glyphSize = glyphSizes[i - utf32Begin];
                                if (isNewline(*i))
                                {
                                    lines.push_back(TextLine(
                                        p.utf32.to_bytes(utf32.substr(lineBegin - utf32.begin(), i - lineBegin)),
                                        glm::vec2(pos.x, font->second->size->metrics.height / 64.f)));
                                    pos.x = 0.f;
                                    pos.y += font->second->size->metrics.height / 64.f;
                                    lineBegin = i;
                                }
                                else if (pos.x > 0.f && pos.x + (!isSpace(*i) ? glyphSize.x : 0) >= request.maxLineWidth)
                                {
                                    if (breakLine != utf32.end())
                                    {
                                        i = breakLine;
                                        breakLine = utf32.end();
                                        lines.push_back(TextLine(
                                            p.utf32.to_bytes(utf32.substr(lineBegin - utf32.begin(), i - lineBegin)),
                                            glm::vec2(breakLineX, font->second->size->metrics.height / 64.f)));
                                        pos.x = 0.f;
                                        pos.y += font->second->size->metrics.height / 64.f;
                                        lineBegin = i + 1;
                                    }
                                    else
                                    {
                                        lines.push_back(TextLine(
                                            p.utf32.to_bytes(utf32.substr(lineBegin - utf32.begin(), i - lineBegin)),
                                            glm::vec2(pos.x, font->second->size->metrics.height / 64.f)));
                                        pos.x = glyphSize.x;
                                        pos.y += font->second->size->metrics.height / 64.f;
                                        lineBegin = i;
                                    }
                                }
                                else
                                {
                                    if (isSpace(*i) && i != utf32.begin())
                                    {
                                        breakLine = i;
                                        breakLineX = pos.x;
                                    }
                                    pos.x += glyphSize.x;
                                }
                            }
                            if (i != lineBegin)
                            {
                                lines.push_back(TextLine(
                                    p.utf32.to_bytes(utf32.substr(lineBegin - utf32.begin(), i - lineBegin)),
                                    glm::vec2(pos.x, font->second->size->metrics.height / 64.f)));
                            }
                            request.promise.set_value(lines);
                        }
                    }
                }
                p.breakLinesRequests.clear();
            }

            void System::_handleGlyphsRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto& request : p.glyphsRequests)
                {
                    const auto family = p.fontFaces.find(request.info.family);
                    if (family != p.fontFaces.end())
                    {
                        const auto font = family->second.find(request.info.face);
                        if (font != family->second.end())
                        {
                            FT_Error ftError = FT_Set_Char_Size(
                                font->second,
                                0,
                                static_cast<int>(request.info.size * 64.f),
                                request.info.dpi,
                                request.info.dpi);
                            if (ftError)
                            {
                                //std::cout << "FT_Set_Char_Size error: " << getFTError(ftError) << std::endl;
                                continue;
                            }
                            const std::basic_string<djv_char_t> utf32 = p.utf32.from_bytes(request.text);
                            std::vector<std::shared_ptr<Glyph> > glyphs;
                            glyphs.reserve(utf32.size());
                            for (const auto& c : utf32)
                            {
                                std::shared_ptr<Glyph> glyph;
                                const auto uid = GlyphInfo(c, request.info).getUID();
                                bool inCache = false;
                                {
                                    std::lock_guard<std::mutex> lock(p.cacheMutex);
                                    /*if (p.glyphCache.contains(uid))
                                    {
                                        inCache = true;
                                        glyph = p.glyphCache.get(uid);
                                    }*/
                                    const auto i = p.glyphCache.find(uid);
                                    if (i != p.glyphCache.end())
                                    {
                                        inCache = true;
                                        glyph = i->second;
                                    }
                                }
                                if (!inCache)
                                {
                                    if (auto ftGlyphIndex = FT_Get_Char_Index(font->second, c))
                                    {
                                        ftError = FT_Load_Glyph(font->second, ftGlyphIndex, FT_LOAD_DEFAULT);
                                        if (ftError)
                                        {
                                            //std::cout << "FT_Load_Glyph error: " << getFTError(ftError) << std::endl;
                                            continue;
                                        }
                                        ftError = FT_Render_Glyph(font->second->glyph, FT_RENDER_MODE_NORMAL);
                                        if (ftError)
                                        {
                                            //std::cout << "FT_Render_Glyph error: " << getFTError(ftError) << std::endl;
                                            continue;
                                        }
                                        FT_Glyph ftGlyph;
                                        ftError = FT_Get_Glyph(font->second->glyph, &ftGlyph);
                                        if (ftError)
                                        {
                                            //std::cout << "FT_Get_Glyph error: " << getFTError(ftError) << std::endl;
                                            continue;
                                        }
                                        FT_Vector v;
                                        v.x = 0;
                                        v.y = 0;
                                        ftError = FT_Glyph_To_Bitmap(&ftGlyph, FT_RENDER_MODE_NORMAL, &v, 0);
                                        if (ftError)
                                        {
                                            //std::cout << "FT_Glyph_To_Bitmap error: " << getFTError(ftError) << std::endl;
                                            FT_Done_Glyph(ftGlyph);
                                            continue;
                                        }
                                        FT_BitmapGlyph bitmap = (FT_BitmapGlyph)ftGlyph;
                                        Image::Info info = Image::Info(bitmap->bitmap.width, bitmap->bitmap.rows, Image::Type::L_U8);
                                        auto imageData = Image::Data::create(Image::Info(bitmap->bitmap.width, bitmap->bitmap.rows, Image::Type::L_U8));
                                        for (int y = 0; y < info.size.y; ++y)
                                        {
                                            memcpy(imageData->getData(info.size.y - 1 - y), bitmap->bitmap.buffer + y * bitmap->bitmap.pitch, info.size.x);
                                            
                                            //auto p = imageData->getData(info.size.y - 1 - y);
                                            //for (int x = 0; x < info.size.x; ++x, ++p)
                                            //{
                                            //    *p = std::min(*p + 255, 255);
                                            //}
                                        }
                                        glyph = Glyph::create(
                                            GlyphInfo(c, request.info),
                                            imageData,
                                            glm::vec2(font->second->glyph->bitmap_left, font->second->glyph->bitmap_top),
                                            font->second->glyph->advance.x / 64.f);
                                        {
                                            std::lock_guard<std::mutex> lock(p.cacheMutex);
                                            //p.glyphCache.add(uid, glyph);
                                            p.glyphCache[uid] = glyph;
                                        }
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

            std::vector<glm::vec2> System::Private::getGlyphSizes(const std::basic_string<djv_char_t> & s, const Info & info, const FT_Face & face)
            {
                std::vector<glm::vec2> out;
                out.reserve(s.size());
                std::lock_guard<std::mutex> lock(cacheMutex);
                for (const auto & c : s)
                {
                    const auto uid = GlyphInfo(c, info).getUID();
                    glm::vec2 size;
                    bool inCache = false;
                    /*if (measureCache.contains(uid))
                    {
                        inCache = true;
                        size = measureCache.get(uid);
                    }*/
                    const auto i = measureCache.find(uid);
                    if (i != measureCache.end())
                    {
                        inCache = true;
                        size = i->second;
                    }
                    if (!inCache)
                    {
                        if (auto ftGlyphIndex = FT_Get_Char_Index(face, c))
                        {
                            FT_Error ftError = FT_Load_Glyph(face, ftGlyphIndex, FT_LOAD_DEFAULT);
                            if (!ftError)
                            {
                                FT_Glyph ftGlyph;
                                ftError = FT_Get_Glyph(face->glyph, &ftGlyph);
                                if (!ftError)
                                {
                                    size.x = face->glyph->advance.x / 64.f;
                                    size.y = face->glyph->metrics.height / 64.f;
                                    FT_Done_Glyph(ftGlyph);
                                    //measureCache.add(uid, size);
                                    measureCache[uid] = size;
                                }
                            }
                        }
                    }
                    out.push_back(size);
                }
                return out;
            }

        } // namespace Font
    } // namespace AV
} // namespace djv
