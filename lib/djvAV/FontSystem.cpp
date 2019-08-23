//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
#include <djvCore/CoreSystem.h>
#include <djvCore/FileInfo.h>
#include <djvCore/ResourceSystem.h>
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
                //! \todo Should this be configurable?
                const size_t glyphCacheMax = 10000;

                //! \todo Experimental LCD hinting.
                const bool lcdHinting = false;

                struct MetricsRequest
                {
                    MetricsRequest() {}
                    MetricsRequest(MetricsRequest && other) = default;
                    MetricsRequest & operator = (MetricsRequest &&) = default;

                    Info info;
                    std::promise<Metrics> promise;
                };

                struct MeasureRequest
                {
                    MeasureRequest() {}
                    MeasureRequest(MeasureRequest &&) = default;
                    MeasureRequest & operator = (MeasureRequest && other) = default;

                    std::string text;
                    Info info;
                    uint16_t maxLineWidth = std::numeric_limits<uint16_t>::max();
                    std::promise<glm::vec2> promise;
                };

                struct TextLinesRequest
                {
                    TextLinesRequest() {}
                    TextLinesRequest(TextLinesRequest &&) = default;
                    TextLinesRequest & operator = (TextLinesRequest &&) = default;

                    std::string text;
                    Info info;
                    uint16_t maxLineWidth = std::numeric_limits<uint16_t>::max();
                    std::promise<std::vector<TextLine> > promise;
                };

                struct GlyphsRequest
                {
                    GlyphsRequest() {}
                    GlyphsRequest(GlyphsRequest &&) = default;
                    GlyphsRequest & operator = (GlyphsRequest &&) = default;

                    std::string text;
                    Info info;
                    bool cacheOnly = false;
                    std::promise<std::vector<std::shared_ptr<Glyph> > > promise;
                };

                constexpr bool isSpace(djv_char_t c)
                {
                    return ' ' == c || '\t' == c;
                }

                constexpr bool isNewline(djv_char_t c)
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

            std::shared_ptr<Glyph> Glyph::create()
            {
                return std::shared_ptr<Glyph>(new Glyph);
            }

            struct System::Private
            {
                FT_Library ftLibrary = nullptr;
                FileSystem::Path fontPath;
                std::map<FamilyID, std::string> fontFileNames;
                std::map<FamilyID, std::string> fontNames;
                std::promise<std::map<FamilyID, std::string> > fontNamesPromise;
                std::map<FamilyID, std::map<FaceID, std::string> > fontFaceNames;
                std::map<FamilyID, std::map<FaceID, FT_Face> > fontFaces;

                std::list<MetricsRequest> metricsQueue;
                std::list<MeasureRequest> measureQueue;
                std::list<TextLinesRequest> textLinesQueue;
                std::list<GlyphsRequest> glyphsQueue;
                std::condition_variable requestCV;
                std::mutex requestMutex;
                std::list<MetricsRequest> metricsRequests;
                std::list<MeasureRequest> measureRequests;
                std::list<TextLinesRequest> textLinesRequests;
                std::list<GlyphsRequest> glyphsRequests;

                std::wstring_convert<std::codecvt_utf8<djv_char_t>, djv_char_t> utf32;
                Memory::Cache<GlyphInfo, std::shared_ptr<Glyph> > glyphCache;
                std::atomic<size_t> glyphCacheSize;
                std::atomic<float> glyphCachePercentageUsed;

                std::shared_ptr<Time::Timer> statsTimer;
                std::thread thread;
                std::atomic<bool> running;

                bool getText(const std::string&, const Info&, std::basic_string<djv_char_t>&, FT_Face&, std::string& error);
                std::shared_ptr<Glyph> getGlyph(const GlyphInfo &);
            };

            void System::_init(const std::shared_ptr<Core::Context>& context)
            {
                ISystem::_init("djv::AV::Font::System", context);

                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<CoreSystem>());

                p.fontPath = _getResourceSystem()->getPath(FileSystem::ResourcePath::FontsDirectory);
                p.glyphCache.setMax(glyphCacheMax);
                p.glyphCacheSize = 0;
                p.glyphCachePercentageUsed = 0.f;

                p.statsTimer = Time::Timer::create(context);
                p.statsTimer->setRepeating(true);
                p.statsTimer->start(
                    Time::getMilliseconds(Time::TimerValue::VerySlow),
                    [this](float)
                {
                    DJV_PRIVATE_PTR();
                    std::stringstream s;
                    s << "Glyph cache: " << p.glyphCacheSize << ", " << p.glyphCachePercentageUsed << "%";
                    _log(s.str());
                });

                p.running = true;
                p.thread = std::thread(
                    [this]
                {
                    DJV_PRIVATE_PTR();
                    _initFreeType();
                    const auto timeout = Time::getValue(Time::TimerValue::Fast);
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
                                    p.textLinesQueue.size() ||
                                    p.glyphsQueue.size();
                            });
                            p.metricsRequests = std::move(p.metricsQueue);
                            p.measureRequests = std::move(p.measureQueue);
                            p.textLinesRequests = std::move(p.textLinesQueue);
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
                        if (p.textLinesRequests.size())
                        {
                            _handleTextLinesRequests();
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
                if (p.thread.joinable())
                {
                    p.thread.join();
                }
            }

            std::shared_ptr<System> System::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(context);
                return out;
            }

            std::future<std::map<FamilyID, std::string> > System::getFontNames()
            {
                auto future = _p->fontNamesPromise.get_future();
                return future;
            }

            std::future<Metrics> System::getMetrics(const Info & info)
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

            std::future<glm::vec2> System::measure(const std::string & text, const Info & info)
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

            std::future<std::vector<TextLine> > System::textLines(const std::string & text, uint16_t maxLineWidth, const Info & info)
            {
                DJV_PRIVATE_PTR();
                TextLinesRequest request;
                request.text         = text;
                request.info         = info;
                request.maxLineWidth = maxLineWidth;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.textLinesQueue.push_back(std::move(request));
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

            void System::cacheGlyphs(const std::string& text, const Info& info)
            {
                DJV_PRIVATE_PTR();
                GlyphsRequest request;
                request.text = text;
                request.info = info;
                request.cacheOnly = true;
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.glyphsQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
            }

            size_t System::getGlyphCacheSize() const
            {
                return _p->glyphCacheSize;
            }

            float System::getGlyphCachePercentage() const
            {
                return _p->glyphCachePercentageUsed;
            }

            void System::_initFreeType()
            {
                DJV_PRIVATE_PTR();
                try
                {
                    FT_Error ftError = FT_Init_FreeType(&p.ftLibrary);
                    if (ftError)
                    {
                        throw std::runtime_error("FreeType cannot be initialized.");
                    }
                    int versionMajor = 0;
                    int versionMinor = 0;
                    int versionPatch = 0;
                    FT_Library_Version(p.ftLibrary, &versionMajor, &versionMinor, &versionPatch);
                    {
                        std::stringstream s;
                        s << "FreeType version: " << versionMajor << "." << versionMinor << "." << versionPatch;
                        _log(s.str());
                    }
                    for (const auto & i : FileSystem::FileInfo::directoryList(p.fontPath))
                    {
                        const std::string & fileName = i.getFileName();

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
                            static FamilyID familyID = 0;
                            static FaceID faceID = 1;
                            ++familyID;
                            p.fontFileNames[familyID] = fileName;
                            p.fontNames[familyID] = ftFace->family_name;
                            p.fontFaceNames[familyID][faceID] = ftFace->style_name;
                            p.fontFaces[familyID][faceID] = ftFace;
                        }
                    }
                    if (!p.fontFaces.size())
                    {
                        throw std::runtime_error("No fonts were found.");
                    }
                    p.fontNamesPromise.set_value(p.fontNames);
                }
                catch (const std::exception & e)
                {
                    _log(e.what());
                }
            }

            void System::_delFreeType()
            {
                DJV_PRIVATE_PTR();
                if (p.ftLibrary)
                {
                    for (const auto & i : p.fontFaces)
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
                for (auto & request : p.metricsRequests)
                {
                    Metrics metrics;
                    const auto family = p.fontFaces.find(request.info.family);
                    if (family != p.fontFaces.end())
                    {
                        const auto font = family->second.find(request.info.face);
                        if (font != family->second.end())
                        {
                            /*FT_Error ftError = FT_Set_Char_Size(
                                font->second,
                                0,
                                static_cast<int>(request.info.size * 64.f),
                                request.info.dpi,
                                request.info.dpi);*/
                            FT_Error ftError = FT_Set_Pixel_Sizes(
                                font->second,
                                0,
                                static_cast<int>(request.info.size));
                            if (!ftError)
                            {
                                metrics.ascender   = font->second->size->metrics.ascender  / 64.f;
                                metrics.descender  = font->second->size->metrics.descender / 64.f;
                                metrics.lineHeight = font->second->size->metrics.height    / 64.f;
                            }
                        }
                    }
                    request.promise.set_value(std::move(metrics));
                }
                p.metricsRequests.clear();
            }

            void System::_handleMeasureRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto & request : p.measureRequests)
                {
                    std::basic_string<djv_char_t> utf32;
                    FT_Face font;
                    std::string error;
                    glm::vec2 size = glm::vec2(0.f, 0.f);
                    if (p.getText(request.text, request.info, utf32, font, error))
                    {
                        glm::vec2 pos(0.f, font->size->metrics.height / 64.f);
                        auto textLine = utf32.end();
                        float textLineX = 0.f;
                        int32_t rsbDeltaPrev = 0;
                        for (auto i = utf32.begin(); i != utf32.end(); ++i)
                        {
                            const auto info = GlyphInfo(*i, request.info);
                            const auto glyph = p.getGlyph(info);
                            int32_t x = 0;
                            if (glyph->imageData)
                            {
                                x = glyph->advance;
                                if (rsbDeltaPrev - glyph->lsbDelta > 32)
                                {
                                    x -= 1;
                                }
                                else if (rsbDeltaPrev - glyph->lsbDelta < -31)
                                {
                                    x += 1;
                                }
                                rsbDeltaPrev = glyph->rsbDelta;
                            }
                            else
                            {
                                rsbDeltaPrev = 0;
                            }

                            if (isNewline(*i))
                            {
                                size.x = std::max(size.x, pos.x);
                                pos.x = 0.f;
                                pos.y += font->size->metrics.height / 64.f;
                                rsbDeltaPrev = 0;
                            }
                            else if (pos.x > 0.f && pos.x + (!isSpace(*i) ? x : 0.f) >= request.maxLineWidth)
                            {
                                if (textLine != utf32.end())
                                {
                                    i = textLine;
                                    textLine = utf32.end();
                                    size.x = std::max(size.x, textLineX);
                                    pos.x = 0.f;
                                    pos.y += font->size->metrics.height / 64.f;
                                }
                                else
                                {
                                    size.x = std::max(size.x, pos.x);
                                    pos.x = x;
                                    pos.y += font->size->metrics.height / 64.f;
                                }
                                rsbDeltaPrev = 0;
                            }
                            else
                            {
                                if (isSpace(*i) && i != utf32.begin())
                                {
                                    textLine = i;
                                    textLineX = pos.x;
                                }
                                pos.x += x;
                            }
                        }
                        size.x = std::max(size.x, pos.x);
                        size.y = pos.y;
                    }
                    else
                    {
                        std::stringstream ss;
                        ss << "Error converting string" << " '" << request.text << "': " << error;
                        _log(ss.str(), LogLevel::Error);
                    }
                    request.promise.set_value(size);
                }
                p.measureRequests.clear();
            }

            void System::_handleTextLinesRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto & request : p.textLinesRequests)
                {
                    std::basic_string<djv_char_t> utf32;
                    FT_Face font;
                    std::string error;
                    std::vector<TextLine> lines;
                    if (p.getText(request.text, request.info, utf32, font, error))
                    {
                        const auto utf32Begin = utf32.begin();
                        glm::vec2 pos = glm::vec2(0.f, font->size->metrics.height / 64.f);
                        auto lineBegin = utf32Begin;
                        auto textLine = utf32.end();
                        float textLineX = 0.f;
                        int32_t rsbDeltaPrev = 0;
                        auto i = utf32.begin();
                        for (; i != utf32.end(); ++i)
                        {
                            const auto info = GlyphInfo(*i, request.info);
                            float x = 0.f;
                            if (const auto glyph = p.getGlyph(info))
                            {
                                x = glyph->advance;
                                if (rsbDeltaPrev - glyph->lsbDelta > 32)
                                {
                                    x -= 1.f;
                                }
                                else if (rsbDeltaPrev - glyph->lsbDelta < -31)
                                {
                                    x += 1.f;
                                }
                                rsbDeltaPrev = glyph->rsbDelta;
                            }
                            else
                            {
                                rsbDeltaPrev = 0;
                            }

                            if (isNewline(*i))
                            {
                                try
                                {
                                    lines.push_back(TextLine(
                                        p.utf32.to_bytes(utf32.substr(lineBegin - utf32.begin(), i - lineBegin)),
                                        glm::vec2(pos.x, font->size->metrics.height / 64.f)));
                                }
                                catch (const std::exception& e)
                                {
                                    std::stringstream ss;
                                    ss << "Error converting string" << " '" << request.text << "': " << e.what();
                                    _log(ss.str(), LogLevel::Error);
                                }
                                pos.x = 0.f;
                                pos.y += font->size->metrics.height / 64.f;
                                lineBegin = i;
                                rsbDeltaPrev = 0;
                            }
                            else if (pos.x > 0.f && pos.x + (!isSpace(*i) ? x : 0) >= request.maxLineWidth)
                            {
                                if (textLine != utf32.end())
                                {
                                    i = textLine;
                                    textLine = utf32.end();
                                    try
                                    {
                                        lines.push_back(TextLine(
                                            p.utf32.to_bytes(utf32.substr(lineBegin - utf32.begin(), i - lineBegin)),
                                            glm::vec2(textLineX, font->size->metrics.height / 64.f)));
                                    }
                                    catch (const std::exception& e)
                                    {
                                        std::stringstream ss;
                                        ss << "Error converting string" << " '" << request.text << "': " << e.what();
                                        _log(ss.str(), LogLevel::Error);
                                    }
                                    pos.x = 0.f;
                                    pos.y += font->size->metrics.height / 64.f;
                                    lineBegin = i + 1;
                                }
                                else
                                {
                                    try
                                    {
                                        lines.push_back(TextLine(
                                        p.utf32.to_bytes(utf32.substr(lineBegin - utf32.begin(), i - lineBegin)),
                                        glm::vec2(pos.x, font->size->metrics.height / 64.f)));
                                    }
                                    catch (const std::exception& e)
                                    {
                                        std::stringstream ss;
                                        ss << "Error converting string" << " '" << request.text << "': " << e.what();
                                        _log(ss.str(), LogLevel::Error);
                                    }
                                    pos.x = x;
                                    pos.y += font->size->metrics.height / 64.f;
                                    lineBegin = i;
                                }
                                rsbDeltaPrev = 0;
                            }
                            else
                            {
                                if (isSpace(*i) && i != utf32.begin())
                                {
                                    textLine = i;
                                    textLineX = pos.x;
                                }
                                pos.x += x;
                            }
                        }
                        if (i != lineBegin)
                        {
                            try
                            {
                                lines.push_back(TextLine(
                                p.utf32.to_bytes(utf32.substr(lineBegin - utf32.begin(), i - lineBegin)),
                                glm::vec2(pos.x, font->size->metrics.height / 64.f)));
                            }
                            catch (const std::exception& e)
                            {
                                std::stringstream ss;
                                ss << "Error converting string" << " '" << request.text << "': " << e.what();
                                _log(ss.str(), LogLevel::Error);
                            }
                        }
                    }
                    else
                    {
                        std::stringstream ss;
                        ss << "Error converting string" << " '" << request.text << "': " << error;
                        _log(ss.str(), LogLevel::Error);
                    }
                    request.promise.set_value(lines);
                }
                p.textLinesRequests.clear();
            }

            void System::_handleGlyphsRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto & request : p.glyphsRequests)
                {
                    std::basic_string<djv_char_t> utf32;
                    try
                    {
                        utf32 = p.utf32.from_bytes(request.text);
                    }
                    catch (const std::exception & e)
                    {
                        std::stringstream ss;
                        ss << "Error converting string" << " '" << request.text << "': " << e.what();
                        _log(ss.str(), LogLevel::Error);
                    }
                    const size_t size = utf32.size();
                    if (request.cacheOnly)
                    {
                        for (size_t i = 0; i < size; ++i)
                        {
                            p.getGlyph(GlyphInfo(utf32[i], request.info));
                        }
                    }
                    else
                    {
                        std::vector<std::shared_ptr<Glyph> > glyphs(size);
                        for (size_t i = 0; i < size; ++i)
                        {
                            glyphs[i] = p.getGlyph(GlyphInfo(utf32[i], request.info));
                        }
                        request.promise.set_value(std::move(glyphs));
                    }
                }
                p.glyphsRequests.clear();
            }

            bool System::Private::getText(
                const std::string& value,
                const Info& info,
                std::basic_string<djv_char_t>& utf32,
                FT_Face& font,
                std::string& error)
            {
                bool out = false;
                const auto family = fontFaces.find(info.family);
                if (family != fontFaces.end())
                {
                    auto i = family->second.find(info.face);
                    if (i != family->second.end())
                    {
                        /*FT_Error ftError = FT_Set_Char_Size(
                            i->second,
                            0,
                            static_cast<int>(info.size * 64.f),
                            info.dpi,
                            info.dpi);*/
                        FT_Error ftError = FT_Set_Pixel_Sizes(
                            i->second,
                            0,
                            static_cast<int>(info.size));
                        if (!ftError)
                        {
                            try
                            {
                                utf32 = this->utf32.from_bytes(value);
                                font = i->second;
                                out = true;
                            }
                            catch (const std::exception & e)
                            {
                                error = e.what();
                            }
                        }
                    }
                }
                return out;
            }

            std::shared_ptr<Glyph> System::Private::getGlyph(const GlyphInfo & info)
            {
                std::shared_ptr<Glyph> out;
                FamilyID family = 0;
                FaceID   face   = 0;
                FT_Face  ftFace = nullptr;
                if (!glyphCache.get(info, out))
                {
                    out = Glyph::create();
                    out->info = info;
                    if (info.info.family != family || info.info.face != face)
                    {
                        const auto i = fontFaces.find(info.info.family);
                        if (i != fontFaces.end())
                        {
                            const auto j = i->second.find(info.info.face);
                            if (j != i->second.end())
                            {
                                family = info.info.family;
                                face   = info.info.face;
                                ftFace = j->second;
                            }
                        }
                    }
                    if (ftFace)
                    {
                        /*FT_Error ftError = FT_Set_Char_Size(
                            ftFace,
                            0,
                            static_cast<int>(request.info.size * 64.f),
                            request.info.dpi,
                            request.info.dpi);*/
                        FT_Error ftError = FT_Set_Pixel_Sizes(
                            ftFace,
                            0,
                            static_cast<int>(info.info.size));
                        if (ftError)
                        {
                            //std::cout << "FT_Set_Char_Size error: " << getFTError(ftError) << std::endl;
                            return nullptr;
                        }

                        if (auto ftGlyphIndex = FT_Get_Char_Index(ftFace, info.code))
                        {
                            ftError = FT_Load_Glyph(ftFace, ftGlyphIndex, FT_LOAD_FORCE_AUTOHINT);
                            if (ftError)
                            {
                                //std::cout << "FT_Load_Glyph error: " << getFTError(ftError) << std::endl;
                                return nullptr;
                            }
                            FT_Render_Mode renderMode = FT_RENDER_MODE_NORMAL;
                            uint8_t renderModeChannels = 1;
                            if (lcdHinting)
                            {
                                renderMode = FT_RENDER_MODE_LCD;
                                renderModeChannels = 3;
                            }
                            ftError = FT_Render_Glyph(ftFace->glyph, renderMode);
                            if (ftError)
                            {
                                //std::cout << "FT_Render_Glyph error: " << getFTError(ftError) << std::endl;
                                return nullptr;
                            }
                            FT_Glyph ftGlyph;
                            ftError = FT_Get_Glyph(ftFace->glyph, &ftGlyph);
                            if (ftError)
                            {
                                //std::cout << "FT_Get_Glyph error: " << getFTError(ftError) << std::endl;
                                return nullptr;
                            }
                            FT_Vector v;
                            v.x = 0;
                            v.y = 0;
                            ftError = FT_Glyph_To_Bitmap(&ftGlyph, renderMode, &v, 0);
                            if (ftError)
                            {
                                //std::cout << "FT_Glyph_To_Bitmap error: " << getFTError(ftError) << std::endl;
                                FT_Done_Glyph(ftGlyph);
                                return nullptr;
                            }
                            FT_BitmapGlyph bitmap = (FT_BitmapGlyph)ftGlyph;
                            const Image::Info imageInfo = Image::Info(
                                bitmap->bitmap.width / static_cast<int>(renderModeChannels),
                                bitmap->bitmap.rows,
                                Image::getIntType(renderModeChannels, 8));
                            auto imageData = Image::Data::create(imageInfo);
                            for (uint16_t y = 0; y < imageInfo.size.h; ++y)
                            {
                                memcpy(
                                    imageData->getData(y),
                                    bitmap->bitmap.buffer + static_cast<size_t>(y) * bitmap->bitmap.pitch,
                                    static_cast<size_t>(imageInfo.size.w) * renderModeChannels);
                            }
                            out->imageData = imageData;
                            out->offset = glm::vec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top);
                            out->advance = ftFace->glyph->advance.x / 64.f;
                            out->lsbDelta = ftFace->glyph->lsb_delta;
                            out->rsbDelta = ftFace->glyph->rsb_delta;
                            FT_Done_Glyph(ftGlyph);
                        }
                        glyphCache.add(info, out);
                        glyphCacheSize = glyphCache.getSize();
                        glyphCachePercentageUsed = glyphCache.getPercentageUsed();
                    }
                }
                return out;
            }

        } // namespace Font
    } // namespace AV
} // namespace djv
