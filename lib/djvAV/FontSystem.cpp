// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
                const bool lcdHinting = true;

                class MetricsRequest
                {
                public:
                    MetricsRequest() {}
                    MetricsRequest(MetricsRequest && other) = default;

                    FontInfo fontInfo;
                    std::promise<Metrics> promise;
                };

                class MeasureRequest
                {
                public:
                    MeasureRequest() {}
                    MeasureRequest(MeasureRequest&&) = default;

                    std::string text;
                    FontInfo fontInfo;
                    uint16_t maxLineWidth = std::numeric_limits<uint16_t>::max();
                    std::promise<glm::vec2> promise;
                };

                class MeasureGlyphsRequest
                {
                public:
                    MeasureGlyphsRequest() {}
                    MeasureGlyphsRequest(MeasureGlyphsRequest&&) = default;

                    std::string text;
                    FontInfo fontInfo;
                    uint16_t maxLineWidth = std::numeric_limits<uint16_t>::max();
                    std::promise<std::vector<BBox2f> > promise;
                };

                class GlyphsRequest
                {
                public:
                    GlyphsRequest() {}
                    GlyphsRequest(GlyphsRequest &&) = default;

                    std::string text;
                    FontInfo fontInfo;
                    bool cacheOnly = false;
                    std::promise<std::vector<std::shared_ptr<Glyph> > > promise;
                };

                class TextLinesRequest
                {
                public:
                    TextLinesRequest() {}
                    TextLinesRequest(TextLinesRequest&&) = default;

                    std::string text;
                    FontInfo fontInfo;
                    uint16_t maxLineWidth = std::numeric_limits<uint16_t>::max();
                    std::promise<std::vector<TextLine> > promise;
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

            Error::Error(const std::string& what) :
                std::runtime_error(what)
            {}

            struct System::Private
            {
                FT_Library ftLibrary = nullptr;
                FileSystem::Path fontPath;
                std::map<FamilyID, std::string> fontFileNames;
                std::map<FamilyID, std::string> fontNames;
                std::shared_ptr<MapSubject<FamilyID, std::string> > fontNamesSubject;
                std::mutex fontNamesMutex;
                std::shared_ptr<Time::Timer> fontNamesTimer;
                std::map<FamilyID, std::map<FaceID, std::string> > fontFaceNames;
                std::shared_ptr<MapSubject<FamilyID, std::map<FaceID, std::string> > > fontFaceNamesSubject;
                std::map<FamilyID, std::map<FaceID, FT_Face> > fontFaces;
                std::map<std::string, FamilyID> fontNameToID;
                std::map<std::pair<FamilyID, std::string>, FamilyID> fontFaceNameToID;

                std::list<MetricsRequest> metricsQueue;
                std::list<MeasureRequest> measureQueue;
                std::list<MeasureGlyphsRequest> measureGlyphsQueue;
                std::list<GlyphsRequest> glyphsQueue;
                std::list<TextLinesRequest> textLinesQueue;
                std::condition_variable requestCV;
                std::mutex requestMutex;
                std::list<MetricsRequest> metricsRequests;
                std::list<MeasureRequest> measureRequests;
                std::list<MeasureGlyphsRequest> measureGlyphsRequests;
                std::list<GlyphsRequest> glyphsRequests;
                std::list<TextLinesRequest> textLinesRequests;

                std::wstring_convert<std::codecvt_utf8<djv_char_t>, djv_char_t> utf32Convert;
                Memory::Cache<GlyphInfo, std::shared_ptr<Glyph> > glyphCache;
                std::atomic<size_t> glyphCacheSize;
                std::atomic<float> glyphCachePercentageUsed;

                std::shared_ptr<Time::Timer> statsTimer;
                std::thread thread;
                std::atomic<bool> running;

                bool getText(const std::string&, const FontInfo&, std::basic_string<djv_char_t>&, FT_Face&, std::string& error);
                std::shared_ptr<Glyph> getGlyph(const GlyphInfo &);
                void measure(
                    const std::basic_string<djv_char_t>& utf32,
                    const FontInfo&,
                    FT_Face,
                    uint16_t maxLineWidth,
                    glm::vec2&,
                    std::vector<BBox2f>* = nullptr);
            };

            void System::_init(const std::shared_ptr<Core::Context>& context)
            {
                ISystem::_init("djv::AV::Font::System", context);

                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<CoreSystem>());

                p.fontPath = _getResourceSystem()->getPath(FileSystem::ResourcePath::Fonts);
                p.fontNamesSubject = MapSubject<FamilyID, std::string>::create();
                p.fontFaceNamesSubject = MapSubject<FamilyID, std::map<FaceID, std::string> >::create();
                p.glyphCache.setMax(glyphCacheMax);
                p.glyphCacheSize = 0;
                p.glyphCachePercentageUsed = 0.F;

                p.fontNamesTimer = Time::Timer::create(context);
                p.fontNamesTimer->setRepeating(true);
                p.fontNamesTimer->start(
                    Time::getTime(Time::TimerValue::Medium),
                    [this](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                    {
                        DJV_PRIVATE_PTR();
                        std::map<FamilyID, std::string> fontNames;
                        std::map<FamilyID, std::map<FaceID, std::string> > fontFaceNames;
                        {
                            std::unique_lock<std::mutex> lock(p.fontNamesMutex);
                            fontNames = p.fontNames;
                            fontFaceNames = p.fontFaceNames;
                        }
                        p.fontNamesSubject->setIfChanged(fontNames);
                        p.fontFaceNamesSubject->setIfChanged(fontFaceNames);
                    });

                p.statsTimer = Time::Timer::create(context);
                p.statsTimer->setRepeating(true);
                p.statsTimer->start(
                    Time::getTime(Time::TimerValue::VerySlow),
                    [this](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                {
                    DJV_PRIVATE_PTR();
                    std::stringstream ss;
                    ss << "Glyph cache: " << p.glyphCacheSize << ", " << p.glyphCachePercentageUsed << "%";
                    _log(ss.str());
                });

                p.running = true;
                p.thread = std::thread(
                    [this]
                {
                    DJV_PRIVATE_PTR();
                    _initFreeType();
                    while (p.running)
                    {
                        {
                            std::unique_lock<std::mutex> lock(p.requestMutex);
                            p.requestCV.wait_for(
                                lock,
                                Time::getTime(Time::TimerValue::Fast),
                                [this]
                            {
                                DJV_PRIVATE_PTR();
                                return
                                    p.metricsQueue.size() ||
                                    p.measureQueue.size() ||
                                    p.glyphsQueue.size() ||
                                    p.textLinesQueue.size();
                            });
                            p.metricsRequests = std::move(p.metricsQueue);
                            p.measureRequests = std::move(p.measureQueue);
                            p.measureGlyphsRequests = std::move(p.measureGlyphsQueue);
                            p.glyphsRequests = std::move(p.glyphsQueue);
                            p.textLinesRequests = std::move(p.textLinesQueue);
                        }
                        if (p.metricsRequests.size())
                        {
                            _handleMetricsRequests();
                        }
                        if (p.measureRequests.size())
                        {
                            _handleMeasureRequests();
                        }
                        if (p.measureGlyphsRequests.size())
                        {
                            _handleMeasureGlyphsRequests();
                        }
                        if (p.glyphsRequests.size())
                        {
                            _handleGlyphsRequests();
                        }
                        if (p.textLinesRequests.size())
                        {
                            _handleTextLinesRequests();
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
            
            std::shared_ptr<Core::IMapSubject<FamilyID, std::string> > System::observeFontNames() const
            {
                return _p->fontNamesSubject;
            }

            std::shared_ptr<Core::IMapSubject<FamilyID, std::map<FaceID, std::string> > > System::observeFontFaces() const
            {
                return _p->fontFaceNamesSubject;
            }

            std::future<Metrics> System::getMetrics(const FontInfo& fontInfo)
            {
                DJV_PRIVATE_PTR();
                MetricsRequest request;
                request.fontInfo = fontInfo;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.metricsQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            std::future<glm::vec2> System::measure(const std::string& text, const FontInfo& fontInfo)
            {
                DJV_PRIVATE_PTR();
                MeasureRequest request;
                request.text = text;
                request.fontInfo = fontInfo;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.measureQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            std::future<std::vector<BBox2f> > System::measureGlyphs(const std::string& text, const FontInfo& fontInfo)
            {
                DJV_PRIVATE_PTR();
                MeasureGlyphsRequest request;
                request.text = text;
                request.fontInfo = fontInfo;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.measureGlyphsQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            std::future<std::vector<std::shared_ptr<Glyph> > > System::getGlyphs(const std::string& text, const FontInfo& fontInfo)
            {
                DJV_PRIVATE_PTR();
                GlyphsRequest request;
                request.text = text;
                request.fontInfo = fontInfo;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.glyphsQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            std::future<std::vector<TextLine> > System::textLines(const std::string& text, uint16_t maxLineWidth, const FontInfo& fontInfo)
            {
                DJV_PRIVATE_PTR();
                TextLinesRequest request;
                request.text = text;
                request.fontInfo = fontInfo;
                request.maxLineWidth = maxLineWidth;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.textLinesQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            void System::cacheGlyphs(const std::string& text, const FontInfo& fontInfo)
            {
                DJV_PRIVATE_PTR();
                GlyphsRequest request;
                request.text = text;
                request.fontInfo = fontInfo;
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
                        throw Error("FreeType cannot be initialized.");
                    }
                    int versionMajor = 0;
                    int versionMinor = 0;
                    int versionPatch = 0;
                    FT_Library_Version(p.ftLibrary, &versionMajor, &versionMinor, &versionPatch);
                    {
                        std::stringstream ss;
                        ss << "FreeType version: " << versionMajor << "." << versionMinor << "." << versionPatch;
                        _log(ss.str());
                    }
                    for (const auto & i : FileSystem::FileInfo::directoryList(p.fontPath))
                    {
                        const std::string & fileName = i.getFileName();
                        {
                            std::stringstream ss;
                            ss << "Loading font: " << fileName;
                            _log(ss.str());
                        }

                        FT_Face ftFace;
                        ftError = FT_New_Face(p.ftLibrary, fileName.c_str(), 0, &ftFace);
                        if (ftError)
                        {
                            std::stringstream ss;
                            ss << "Cannot load font: " << fileName;
                            _log(ss.str(), LogLevel::Error);
                        }
                        else
                        {
                            std::stringstream ss;
                            ss << "    Family: " << ftFace->family_name << '\n';
                            ss << "    Style: " << ftFace->style_name << '\n';
                            ss << "    Number of glyphs: " << static_cast<int>(ftFace->num_glyphs) << '\n';
                            ss << "    Scalable: " << (FT_IS_SCALABLE(ftFace) ? "true" : "false") << '\n';
                            ss << "    Kerning: " << (FT_HAS_KERNING(ftFace) ? "true" : "false");
                            _log(ss.str());

                            FamilyID familyID = 0;
                            auto j = p.fontNameToID.find(ftFace->family_name);
                            if (j != p.fontNameToID.end())
                            {
                                familyID = j->second;
                            }
                            else
                            {
                                for (auto k : p.fontNameToID)
                                {
                                    familyID = std::max(familyID, k.second);
                                }
                                ++familyID;
                                p.fontNameToID[ftFace->family_name] = familyID;
                            }

                            FaceID faceID = 0;
                            auto k = p.fontFaceNameToID.find(std::make_pair(familyID, ftFace->style_name));
                            if (k != p.fontFaceNameToID.end())
                            {
                                faceID = k->second;
                            }
                            else
                            {
                                for (auto l : p.fontFaceNameToID)
                                {
                                    faceID = std::max(faceID, l.second);
                                }
                                ++faceID;
                                p.fontFaceNameToID[std::make_pair(familyID, ftFace->style_name)] = faceID;
                            }

                            p.fontFileNames[familyID] = fileName;
                            p.fontNames[familyID] = ftFace->family_name;
                            p.fontFaceNames[familyID][faceID] = ftFace->style_name;
                            p.fontFaces[familyID][faceID] = ftFace;
                        }
                    }
                    if (!p.fontFaces.size())
                    {
                        throw Error("No fonts were found.");
                    }
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
                    const auto family = p.fontFaces.find(request.fontInfo.getFamily());
                    if (family != p.fontFaces.end())
                    {
                        const auto font = family->second.find(request.fontInfo.getFace());
                        if (font != family->second.end())
                        {
                            /*FT_Error ftError = FT_Set_Char_Size(
                                font->second,
                                0,
                                static_cast<int>(request.fontInfo.getSize() * 64.F),
                                request.fontInfo.getDPI(),
                                request.fontInfo.getDPI());*/
                            FT_Error ftError = FT_Set_Pixel_Sizes(
                                font->second,
                                0,
                                static_cast<int>(request.fontInfo.getSize()));
                            if (!ftError)
                            {
                                metrics.ascender   = font->second->size->metrics.ascender  / 64.F;
                                metrics.descender  = font->second->size->metrics.descender / 64.F;
                                metrics.lineHeight = font->second->size->metrics.height    / 64.F;
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
                for (auto& request : p.measureRequests)
                {
                    std::basic_string<djv_char_t> utf32;
                    FT_Face font;
                    std::string error;
                    glm::vec2 size = glm::vec2(0.F, 0.F);
                    if (p.getText(request.text, request.fontInfo, utf32, font, error))
                    {
                        p.measure(utf32, request.fontInfo, font, request.maxLineWidth, size);
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

            void System::_handleMeasureGlyphsRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto& request : p.measureGlyphsRequests)
                {
                    std::basic_string<djv_char_t> utf32;
                    FT_Face font;
                    std::string error;
                    glm::vec2 size = glm::vec2(0.F, 0.F);
                    std::vector<BBox2f> glyphGeom;
                    if (p.getText(request.text, request.fontInfo, utf32, font, error))
                    {
                        p.measure(utf32, request.fontInfo, font, request.maxLineWidth, size, &glyphGeom);
                    }
                    else
                    {
                        std::stringstream ss;
                        ss << "Error converting string" << " '" << request.text << "': " << error;
                        _log(ss.str(), LogLevel::Error);
                    }
                    request.promise.set_value(glyphGeom);
                }
                p.measureGlyphsRequests.clear();
            }

            void System::_handleGlyphsRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto & request : p.glyphsRequests)
                {
                    std::basic_string<djv_char_t> utf32;
                    try
                    {
                        utf32 = p.utf32Convert.from_bytes(request.text);
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
                            p.getGlyph(GlyphInfo(utf32[i], request.fontInfo));
                        }
                    }
                    else
                    {
                        std::vector<std::shared_ptr<Glyph> > glyphs(size);
                        for (size_t i = 0; i < size; ++i)
                        {
                            glyphs[i] = p.getGlyph(GlyphInfo(utf32[i], request.fontInfo));
                        }
                        request.promise.set_value(std::move(glyphs));
                    }
                }
                p.glyphsRequests.clear();
            }

            void System::_handleTextLinesRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto& request : p.textLinesRequests)
                {
                    // Input:
                    //   Speckled Dace are capable of |living in an array of habitats
                    //                                ^
                    //                                max width
                    //
                    // Processing:
                    //   Speckled Dace are capable of |living in an array of habitats
                    //   ^                           ^^
                    //   line begin        line break  i
                    //
                    // Output:
                    //   "Speckled Dace are capable of"
                    //   "living in an array of"
                    //   "habitats"

                    std::basic_string<djv_char_t> utf32;
                    FT_Face font;
                    std::string error;
                    std::vector<TextLine> lines;
                    if (p.getText(request.text, request.fontInfo, utf32, font, error))
                    {
                        // Get the glyphs.
                        std::vector<std::shared_ptr<Glyph> > glyphs(utf32.size());
                        const auto utf32Begin = utf32.begin();
                        auto i = utf32Begin;
                        for (; i != utf32.end(); ++i)
                        {
                            const auto info = GlyphInfo(*i, request.fontInfo);
                            auto glyph = p.getGlyph(info);
                            glyphs[i - utf32Begin] = glyph;
                        }

                        glm::vec2 pos = glm::vec2(0.F, font->size->metrics.height / 64.F);
                        auto lineBegin = utf32Begin;
                        auto lineBreak = utf32.end();
                        float lineBreakPos = 0.F;
                        int32_t rsbDeltaPrev = 0;
                        i = utf32Begin;
                        for (; i != utf32.end(); ++i)
                        {
                            // Get the current glyph's advance.
                            float advance = 0.F;
                            if (auto glyph = glyphs[i - utf32Begin])
                            {
                                advance = glyph->advance;
                                if (rsbDeltaPrev - glyph->lsbDelta > 32)
                                {
                                    advance -= 1.F;
                                }
                                else if (rsbDeltaPrev - glyph->lsbDelta < -31)
                                {
                                    advance += 1.F;
                                }
                                rsbDeltaPrev = glyph->rsbDelta;
                            }
                            else
                            {
                                rsbDeltaPrev = 0;
                            }

                            if (isNewline(*i))
                            {
                                // Line break from newline.
                                try
                                {
                                    const size_t offset = lineBegin - utf32.begin();
                                    const size_t size = i - lineBegin;
                                    TextLine line;
                                    line.text = p.utf32Convert.to_bytes(utf32.substr(offset, size));
                                    line.size = glm::vec2(pos.x, font->size->metrics.height / 64.F);
                                    line.glyphs = std::vector<std::shared_ptr<Glyph> >(glyphs.begin() + offset, glyphs.begin() + offset + size);
                                    lines.push_back(line);
                                }
                                catch (const std::exception & e)
                                {
                                    std::stringstream ss;
                                    ss << "Error converting string" << " '" << request.text << "': " << e.what();
                                    _log(ss.str(), LogLevel::Error);
                                }
                                pos.x = 0.F;
                                pos.y += font->size->metrics.height / 64.F;
                                lineBegin = i;
                                lineBreak = utf32.end();
                                rsbDeltaPrev = 0;
                            }
                            else if (pos.x > 0.F && pos.x + (!isSpace(*i) ? advance : 0) >= request.maxLineWidth)
                            {
                                if (lineBreak != utf32.end())
                                {
                                    // Maximum line width reached, rewind to the line break.
                                    i = lineBreak;
                                    lineBreak = utf32.end();
                                    try
                                    {
                                        const size_t offset = lineBegin - utf32.begin();
                                        const size_t size = i - lineBegin;
                                        TextLine line;
                                        line.text = p.utf32Convert.to_bytes(utf32.substr(offset, size));
                                        line.size = glm::vec2(lineBreakPos, font->size->metrics.height / 64.F);
                                        line.glyphs = std::vector<std::shared_ptr<Glyph> >(glyphs.begin() + offset, glyphs.begin() + offset + size);
                                        lines.push_back(line);
                                    }
                                    catch (const std::exception & e)
                                    {
                                        std::stringstream ss;
                                        ss << "Error converting string" << " '" << request.text << "': " << e.what();
                                        _log(ss.str(), LogLevel::Error);
                                    }
                                    pos.x = 0.F;
                                    pos.y += font->size->metrics.height / 64.F;
                                    lineBegin = i + 1;
                                }
                                else
                                {
                                    // Maximum line width reached without a line break.
                                    try
                                    {
                                        const size_t offset = lineBegin - utf32.begin();
                                        const size_t size = i - lineBegin;
                                        TextLine line;
                                        line.text = p.utf32Convert.to_bytes(utf32.substr(offset, size));
                                        line.size = glm::vec2(pos.x, font->size->metrics.height / 64.F);
                                        line.glyphs = std::vector<std::shared_ptr<Glyph> >(glyphs.begin() + offset, glyphs.begin() + offset + size);
                                        lines.push_back(line);
                                    }
                                    catch (const std::exception & e)
                                    {
                                        std::stringstream ss;
                                        ss << "Error converting string" << " '" << request.text << "': " << e.what();
                                        _log(ss.str(), LogLevel::Error);
                                    }
                                    pos.x = advance;
                                    pos.y += font->size->metrics.height / 64.F;
                                    lineBegin = i;
                                    lineBreak = utf32.end();
                                }
                                rsbDeltaPrev = 0;
                            }
                            else
                            {
                                if (isSpace(*i) && i != utf32.begin())
                                {
                                    lineBreak = i;
                                    lineBreakPos = pos.x;
                                }
                                pos.x += advance;
                            }
                        }

                        // Remainder.
                        if (i != lineBegin)
                        {
                            try
                            {
                                const size_t offset = lineBegin - utf32.begin();
                                const size_t size = i - lineBegin;
                                TextLine textLine;
                                textLine.text = p.utf32Convert.to_bytes(utf32.substr(offset, size));
                                textLine.size = glm::vec2(pos.x, font->size->metrics.height / 64.F);
                                textLine.glyphs = std::vector<std::shared_ptr<Glyph> >(glyphs.begin() + offset, glyphs.begin() + offset + size);
                                lines.push_back(textLine);
                            }
                            catch (const std::exception & e)
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

            bool System::Private::getText(
                const std::string& value,
                const FontInfo& fontInfo,
                std::basic_string<djv_char_t>& utf32,
                FT_Face& font,
                std::string& error)
            {
                bool out = false;
                const auto family = fontFaces.find(fontInfo.getFamily());
                if (family != fontFaces.end())
                {
                    auto i = family->second.find(fontInfo.getFace());
                    if (i != family->second.end())
                    {
                        /*FT_Error ftError = FT_Set_Char_Size(
                            i->second,
                            0,
                            static_cast<int>(fontInfo.getSize() * 64.f),
                            fontInfo.getDPI(),
                            fontInfo.getDPI());*/
                        FT_Error ftError = FT_Set_Pixel_Sizes(
                            i->second,
                            0,
                            static_cast<int>(fontInfo.getSize()));
                        if (!ftError)
                        {
                            try
                            {
                                utf32 = utf32Convert.from_bytes(value);
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

            std::shared_ptr<Glyph> System::Private::getGlyph(const GlyphInfo& glyphInfo)
            {
                std::shared_ptr<Glyph> out;
                FT_Face ftFace = nullptr;
                if (!glyphCache.get(glyphInfo, out))
                {
                    out = Glyph::create();
                    out->glyphInfo = glyphInfo;
                    if (glyphInfo.fontInfo.getFamily() != 0 || glyphInfo.fontInfo.getFace() != 0)
                    {
                        const auto i = fontFaces.find(glyphInfo.fontInfo.getFamily());
                        if (i != fontFaces.end())
                        {
                            const auto j = i->second.find(glyphInfo.fontInfo.getFace());
                            if (j != i->second.end())
                            {
                                ftFace = j->second;
                            }
                        }
                    }
                    if (ftFace)
                    {
                        FT_Error ftError = FT_Set_Pixel_Sizes(
                            ftFace,
                            0,
                            static_cast<int>(glyphInfo.fontInfo.getSize()));
                        if (ftError)
                        {
                            //std::cout << "FT_Set_Char_Size error: " << getFTError(ftError) << std::endl;
                            return nullptr;
                        }

                        if (auto ftGlyphIndex = FT_Get_Char_Index(ftFace, glyphInfo.code))
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
                            out->advance = ftFace->glyph->advance.x / 64.F;
                            out->lsbDelta = ftFace->glyph->lsb_delta;
                            out->rsbDelta = ftFace->glyph->rsb_delta;
                            FT_Done_Glyph(ftGlyph);
                        }
                        glyphCache.add(glyphInfo, out);
                        glyphCacheSize = glyphCache.getSize();
                        glyphCachePercentageUsed = glyphCache.getPercentageUsed();
                    }
                }
                return out;
            }

            void System::Private::measure(
                const std::basic_string<djv_char_t>& utf32,
                const FontInfo& fontInfo,
                FT_Face font,
                uint16_t maxLineWidth,
                glm::vec2& size,
                std::vector<BBox2f>* glyphGeom)
            {
                glm::vec2 pos(0.F, font->size->metrics.height / 64.F);
                auto textLine = utf32.end();
                float textLineX = 0.F;
                int32_t rsbDeltaPrev = 0;
                for (auto i = utf32.begin(); i != utf32.end(); ++i)
                {
                    const auto glyphInfo = GlyphInfo(*i, fontInfo);
                    const auto glyph = getGlyph(glyphInfo);

                    if (glyphGeom)
                    {
                        glyphGeom->push_back(BBox2f(
                            pos.x,
                            glyph->advance,
                            glyph->advance,
                            font->size->metrics.height / 64.F));
                    }

                    int32_t x = 0;
                    glm::vec2 posAndSize(0.F, 0.F);
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
                        pos.x = 0.F;
                        pos.y += font->size->metrics.height / 64.F;
                        rsbDeltaPrev = 0;
                    }
                    else if (pos.x > 0.F && pos.x + (!isSpace(*i) ? x : 0.F) >= maxLineWidth)
                    {
                        if (textLine != utf32.end())
                        {
                            i = textLine;
                            textLine = utf32.end();
                            size.x = std::max(size.x, textLineX);
                            pos.x = 0.F;
                            pos.y += font->size->metrics.height / 64.F;
                        }
                        else
                        {
                            size.x = std::max(size.x, pos.x);
                            pos.x = x;
                            pos.y += font->size->metrics.height / 64.F;
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

        } // namespace Font
    } // namespace AV
} // namespace djv
