// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvRender2D/FontSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/CoreSystem.h>
#include <djvSystem/FileInfoFunc.h>
#include <djvSystem/ResourceSystem.h>
#include <djvSystem/TimerFunc.h>

#include <djvImage/ImageData.h>

#include <djvCore/Cache.h>
#include <djvCore/StringFunc.h>

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
    namespace Render2D
    {
        namespace Font
        {
            FontInfo::FontInfo() noexcept
            {}

            FontInfo::FontInfo(FamilyID family, FaceID face, uint16_t size, uint16_t dpi) :
                _family(family),
                _face(face),
                _size(size),
                _dpi(dpi)
            {
                Core::Memory::hashCombine(_hash, _family);
                Core::Memory::hashCombine(_hash, _face);
                Core::Memory::hashCombine(_hash, _size);
                Core::Memory::hashCombine(_hash, _dpi);
            }

            Metrics::Metrics() noexcept
            {}

            GlyphInfo::GlyphInfo() noexcept
            {}

            GlyphInfo::GlyphInfo(uint32_t code, const FontInfo& fontInfo) noexcept :
                code(code),
                fontInfo(fontInfo)
            {}

            Glyph::Glyph()
            {}

            TextLine::TextLine()
            {}

            TextLine::TextLine(const std::string & text, const glm::vec2 & size, const std::vector<std::shared_ptr<Glyph> > & glyphs) :
                text(text),
                size(size),
                glyphs(glyphs)
            {}

            namespace
            {
                //! \todo Should this be configurable?
                const size_t glyphCacheMax = 10000;

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
                    uint16_t elide = 0;
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
                    uint16_t elide = 0;
                    uint16_t maxLineWidth = std::numeric_limits<uint16_t>::max();
                    std::promise<std::vector<Math::BBox2f> > promise;
                };

                class GlyphsRequest
                {
                public:
                    GlyphsRequest() {}
                    GlyphsRequest(GlyphsRequest &&) = default;

                    std::string text;
                    FontInfo fontInfo;
                    uint16_t elide = 0;
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

                std::shared_ptr<Image::Data> convert(
                    FT_Bitmap bitmap,
                    uint8_t renderModeChannels)
                {
                    std::shared_ptr<Image::Data> out;
                    Image::Type imageType = Image::getIntType(renderModeChannels, 8);
#if defined(DJV_GL_ES2)
                    imageType = Image::Type::RGBA_U8;
#endif // DJV_GL_ES2
                    const Image::Info imageInfo = Image::Info(
                        bitmap.width / static_cast<int>(renderModeChannels),
                        bitmap.rows,
                        imageType);
                    out = Image::Data::create(imageInfo);
                    for (uint16_t y = 0; y < imageInfo.size.h; ++y)
                    {
                        uint8_t* imageP = out->getData(y);
                        unsigned char* bitmapP = bitmap.buffer + static_cast<int>(y) * bitmap.pitch;
                        switch (renderModeChannels)
                        {
                        case 1:
                            for (uint16_t x = 0; x < imageInfo.size.w; ++x)
                            {
#if defined(DJV_GL_ES2)
                                imageP[x * 4] = imageP[x * 4 + 1] = imageP[x * 4 + 2] = bitmapP[x];
                                imageP[x * 4 + 3] = 0;
#else // DJV_GL_ES2
                                imageP[x] = bitmapP[x];
#endif // DJV_GL_ES2
                            }
                            break;
                        case 3:
                            for (uint16_t x = 0; x < imageInfo.size.w; ++x)
                            {
#if defined(DJV_GL_ES2)
                                imageP[x * 4] = bitmapP[x * 3];
                                imageP[x * 4 + 1] = bitmapP[x * 3 + 1];
                                imageP[x * 4 + 2] = bitmapP[x * 3 + 2];
                                imageP[x * 4 + 3] = 0;
#else // DJV_GL_ES2
                                imageP[x * 3] = bitmapP[x * 3];
                                imageP[x * 3 + 1] = bitmapP[x * 3 + 1];
                                imageP[x * 3 + 2] = bitmapP[x * 3 + 2];
#endif // DJV_GL_ES2
                            }
                            break;
                        }
                    }
                    return out;
                }

            } // namespace

            std::shared_ptr<Glyph> Glyph::create()
            {
                return std::shared_ptr<Glyph>(new Glyph);
            }

            Error::Error(const std::string& what) :
                std::runtime_error(what)
            {}

            struct FontSystem::Private
            {
                bool lcdRendering = true;
                bool lcdRenderingThread = lcdRendering;

                FT_Library ftLibrary = nullptr;
                System::File::Path fontPath;
                std::map<FamilyID, std::string> fontFileNames;
                std::map<FamilyID, std::string> fontNames;
                std::shared_ptr<MapSubject<FamilyID, std::string> > fontNamesSubject;
                std::mutex fontNamesMutex;
                std::shared_ptr<System::Timer> fontNamesTimer;
                std::map<FamilyID, std::map<FaceID, std::string> > fontFaceNames;
                std::shared_ptr<MapSubject<FamilyID, std::map<FaceID, std::string> > > fontFaceNamesSubject;
                std::map<FamilyID, std::map<FaceID, FT_Face> > fontFaces;
                std::map<std::string, FamilyID> fontNameToID;
                std::map<std::pair<FamilyID, std::string>, FamilyID> fontFaceNameToID;
                std::vector< std::pair<FamilyID, FaceID> > symbolFonts;

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

                std::shared_ptr<System::Timer> statsTimer;
                std::thread thread;
                std::atomic<bool> running;

                std::vector<FontInfo> getFontInfoList(const FontInfo&) const;

                FT_Face getFace(FamilyID, FaceID) const;

                std::shared_ptr<Glyph> getGlyph(uint32_t, const std::vector<FontInfo>&);
                
                void measure(
                    const std::basic_string<djv_char_t>& utf32,
                    const std::vector<FontInfo>&,
                    uint16_t maxLineWidth,
                    glm::vec2&,
                    std::vector<Math::BBox2f>* = nullptr);
            };

            void FontSystem::_init(const std::shared_ptr<System::Context>& context)
            {
                ISystem::_init("djv::Render2D::Font::FontSystem", context);

                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<System::CoreSystem>());

                p.fontPath = _getResourceSystem()->getPath(System::File::ResourcePath::Fonts);
                p.fontNamesSubject = MapSubject<FamilyID, std::string>::create();
                p.fontFaceNamesSubject = MapSubject<FamilyID, std::map<FaceID, std::string> >::create();
                p.glyphCache.setMax(glyphCacheMax);
                p.glyphCacheSize = 0;
                p.glyphCachePercentageUsed = 0.F;

                p.fontNamesTimer = System::Timer::create(context);
                p.fontNamesTimer->setRepeating(true);
                p.fontNamesTimer->start(
                    System::getTimerDuration(System::TimerValue::Medium),
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

                p.statsTimer = System::Timer::create(context);
                p.statsTimer->setRepeating(true);
                p.statsTimer->start(
                    System::getTimerDuration(System::TimerValue::VerySlow),
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
                    bool lcdRenderingChanged = false;
                    const Time::Duration threadTimerDuration = System::getTimerDuration(System::TimerValue::Fast);
                    while (p.running)
                    {
                        {
                            std::unique_lock<std::mutex> lock(p.requestMutex);
                            p.requestCV.wait_for(
                                lock,
                                threadTimerDuration,
                                [this]
                            {
                                DJV_PRIVATE_PTR();
                                return
                                    p.lcdRendering != p.lcdRenderingThread ||
                                    p.metricsQueue.size() ||
                                    p.measureQueue.size() ||
                                    p.glyphsQueue.size() ||
                                    p.textLinesQueue.size();
                            });
                            if (p.lcdRendering != p.lcdRenderingThread)
                            {
                                p.lcdRenderingThread = p.lcdRendering;
                                lcdRenderingChanged = true;
                            }
                            p.metricsRequests = std::move(p.metricsQueue);
                            p.measureRequests = std::move(p.measureQueue);
                            p.measureGlyphsRequests = std::move(p.measureGlyphsQueue);
                            p.glyphsRequests = std::move(p.glyphsQueue);
                            p.textLinesRequests = std::move(p.textLinesQueue);
                        }
                        if (lcdRenderingChanged)
                        {
                            p.glyphCache.clear();
                            p.glyphCacheSize = 0;
                            p.glyphCachePercentageUsed = 0.F;
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

            FontSystem::FontSystem() :
                _p(new Private)
            {}

            FontSystem::~FontSystem()
            {
                DJV_PRIVATE_PTR();
                p.running = false;
                if (p.thread.joinable())
                {
                    p.thread.join();
                }
            }

            std::shared_ptr<FontSystem> FontSystem::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = context->getSystemT<FontSystem>();
                if (!out)
                {
                    out = std::shared_ptr<FontSystem>(new FontSystem);
                    out->_init(context);
                }
                return out;
            }
            
            std::shared_ptr<IMapSubject<FamilyID, std::string> > FontSystem::observeFontNames() const
            {
                return _p->fontNamesSubject;
            }

            std::shared_ptr<IMapSubject<FamilyID, std::map<FaceID, std::string> > > FontSystem::observeFontFaces() const
            {
                return _p->fontFaceNamesSubject;
            }

            void FontSystem::setLCDRendering(bool value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.lcdRendering)
                    return;
                std::unique_lock<std::mutex> lock(p.requestMutex);
                p.lcdRendering = value;
            }

            std::future<Metrics> FontSystem::getMetrics(const FontInfo& fontInfo)
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

            std::future<glm::vec2> FontSystem::measure(
                const std::string& text,
                const FontInfo& fontInfo,
                uint16_t elide)
            {
                DJV_PRIVATE_PTR();
                MeasureRequest request;
                request.text = text;
                request.fontInfo = fontInfo;
                request.elide = elide;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.measureQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            std::future<std::vector<Math::BBox2f> > FontSystem::measureGlyphs(
                const std::string& text,
                const FontInfo& fontInfo,
                uint16_t elide)
            {
                DJV_PRIVATE_PTR();
                MeasureGlyphsRequest request;
                request.text = text;
                request.fontInfo = fontInfo;
                request.elide = elide;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.measureGlyphsQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            std::future<std::vector<std::shared_ptr<Glyph> > > FontSystem::getGlyphs(
                const std::string& text,
                const FontInfo& fontInfo,
                uint16_t elide)
            {
                DJV_PRIVATE_PTR();
                GlyphsRequest request;
                request.text = text;
                request.fontInfo = fontInfo;
                request.elide = elide;
                auto future = request.promise.get_future();
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    p.glyphsQueue.push_back(std::move(request));
                }
                p.requestCV.notify_one();
                return future;
            }

            std::future<std::vector<TextLine> > FontSystem::textLines(
                const std::string& text,
                uint16_t maxLineWidth,
                const FontInfo& fontInfo)
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

            void FontSystem::cacheGlyphs(const std::string& text, const FontInfo& fontInfo)
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

            size_t FontSystem::getGlyphCacheSize() const
            {
                return _p->glyphCacheSize;
            }

            float FontSystem::getGlyphCachePercentage() const
            {
                return _p->glyphCachePercentageUsed;
            }

            void FontSystem::_initFreeType()
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
                    for (const auto& i : System::File::directoryList(p.fontPath))
                    {
                        const std::string& fileName = i.getFileName();
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
                            _log(ss.str(), System::LogLevel::Error);
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
                            //! \bug Probably not the best way to do this...
                            if (String::match(ftFace->family_name, "Symbols"))
                            {
                                p.symbolFonts.push_back(std::make_pair(familyID, faceID));
                            }
                            else
                            {
                                std::unique_lock<std::mutex> lock(p.fontNamesMutex);
                                p.fontNames[familyID] = ftFace->family_name;
                                p.fontFaceNames[familyID][faceID] = ftFace->style_name;
                            }
                            p.fontFaces[familyID][faceID] = ftFace;
                        }
                    }
                    if (!p.fontFaces.size())
                    {
                        throw Error("No fonts were found.");
                    }
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
                        for (const auto& j : i.second)
                        {
                            FT_Done_Face(j.second);
                        }
                    }
                    FT_Done_FreeType(p.ftLibrary);
                }
            }

            void FontSystem::_handleMetricsRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto& request : p.metricsRequests)
                {
                    Metrics metrics;
                    if (auto ftFace = p.getFace(request.fontInfo.getFamily(), request.fontInfo.getFace()))
                    {
                        /*FT_Error ftError = FT_Set_Char_Size(
                            ftFace->second,
                            0,
                            static_cast<int>(fontInfo.getSize() * 64.F),
                            request.fontInfo.getDPI(),
                            request.fontInfo.getDPI());*/
                        FT_Error ftError = FT_Set_Pixel_Sizes(
                            ftFace,
                            0,
                            static_cast<int>(request.fontInfo.getSize()));
                        if (!ftError)
                        {
                            metrics.ascender = static_cast<float>(ftFace->size->metrics.ascender) / 64.F;
                            metrics.descender = static_cast<float>(ftFace->size->metrics.descender) / 64.F;
                            metrics.lineHeight = static_cast<float>(ftFace->size->metrics.height) / 64.F;
                        }
                    }
                    request.promise.set_value(std::move(metrics));
                }
                p.metricsRequests.clear();
            }

            void FontSystem::_handleMeasureRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto& request : p.measureRequests)
                {
                    glm::vec2 size = glm::vec2(0.F, 0.F);
                    try
                    {
                        auto utf32 = p.utf32Convert.from_bytes(request.text);
                        const size_t inSize = utf32.size();
                        const size_t outSize = request.elide > 0 ? std::min(inSize, static_cast<size_t>(request.elide)) : inSize;
                        while (utf32.size() > outSize)
                        {
                            utf32.pop_back();
                        }
                        if (outSize < inSize)
                        {
                            utf32.push_back('.');
                            utf32.push_back('.');
                            utf32.push_back('.');
                        }
                        p.measure(utf32, p.getFontInfoList(request.fontInfo), request.maxLineWidth, size);
                    }
                    catch (const std::exception& e)
                    {
                        std::stringstream ss;
                        ss << "Error converting string" << " '" << request.text << "': " << e.what();
                        _log(ss.str(), System::LogLevel::Error);
                    }
                    request.promise.set_value(size);
                }
                p.measureRequests.clear();
            }

            void FontSystem::_handleMeasureGlyphsRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto& request : p.measureGlyphsRequests)
                {
                    glm::vec2 size = glm::vec2(0.F, 0.F);
                    std::vector<Math::BBox2f> glyphGeom;
                    try
                    {
                        auto utf32 = p.utf32Convert.from_bytes(request.text);
                        const size_t inSize = utf32.size();
                        const size_t outSize = request.elide > 0 ? std::min(inSize, static_cast<size_t>(request.elide)) : inSize;
                        while (utf32.size() > outSize)
                        {
                            utf32.pop_back();
                        }
                        if (outSize < inSize)
                        {
                            utf32.push_back('.');
                            utf32.push_back('.');
                            utf32.push_back('.');
                        }
                        p.measure(utf32, p.getFontInfoList(request.fontInfo), request.maxLineWidth, size, &glyphGeom);
                    }
                    catch (const std::exception& e)
                    {
                        std::stringstream ss;
                        ss << "Error converting string" << " '" << request.text << "': " << e.what();
                        _log(ss.str(), System::LogLevel::Error);
                    }
                    request.promise.set_value(glyphGeom);
                }
                p.measureGlyphsRequests.clear();
            }

            void FontSystem::_handleGlyphsRequests()
            {
                DJV_PRIVATE_PTR();
                for (auto& request : p.glyphsRequests)
                {
                    std::basic_string<djv_char_t> utf32;
                    try
                    {
                        utf32 = p.utf32Convert.from_bytes(request.text);
                    }
                    catch (const std::exception& e)
                    {
                        std::stringstream ss;
                        ss << "Error converting string" << " '" << request.text << "': " << e.what();
                        _log(ss.str(), System::LogLevel::Error);
                    }
                    const size_t inSize = utf32.size();
                    const size_t outSize = request.elide > 0 ? std::min(inSize, static_cast<size_t>(request.elide)) : inSize;
                    const bool elided = outSize < inSize;
                    const auto fontInfoList = p.getFontInfoList(request.fontInfo);
                    if (request.cacheOnly)
                    {
                        for (size_t i = 0; i < inSize; ++i)
                        {
                            p.getGlyph(utf32[i], fontInfoList);
                        }
                    }
                    else
                    {
                        std::vector<std::shared_ptr<Glyph> > glyphs(outSize + (elided ? 3 : 0));
                        size_t i = 0;
                        for (; i < outSize; ++i)
                        {
                            glyphs[i] = p.getGlyph(utf32[i], fontInfoList);
                        }
                        if (elided)
                        {
                            glyphs[i] = p.getGlyph('.', fontInfoList);
                            glyphs[i + 1] = p.getGlyph('.', fontInfoList);
                            glyphs[i + 2] = p.getGlyph('.', fontInfoList);
                        }
                        request.promise.set_value(std::move(glyphs));
                    }
                }
                p.glyphsRequests.clear();
            }

            void FontSystem::_handleTextLinesRequests()
            {
                DJV_PRIVATE_PTR();

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

                for (auto& request : p.textLinesRequests)
                {
                    std::vector<TextLine> lines;
                    if (FT_Face ftFace = p.getFace(request.fontInfo.getFamily(), request.fontInfo.getFace()))
                    {
                        /*FT_Error ftError = FT_Set_Char_Size(
                            ftFace->second,
                            0,
                            static_cast<int>(request.fontInfo.getSize() * 64.F),
                            fontInfo.getDPI(),
                            fontInfo.getDPI());*/
                        FT_Error ftError = FT_Set_Pixel_Sizes(
                            ftFace,
                            0,
                            static_cast<int>(request.fontInfo.getSize()));
                        if (ftError)
                        {
                            std::stringstream ss;
                            ss << "Error setting font size" << " '" << request.text << "': " << request.fontInfo.getSize();
                            _log(ss.str(), System::LogLevel::Error);
                        }

                        // Get the glyphs.
                        std::basic_string<djv_char_t> utf32;
                        try
                        {
                            utf32 = p.utf32Convert.from_bytes(request.text);
                        }
                        catch (const std::exception& e)
                        {
                            std::stringstream ss;
                            ss << "Error converting string" << " '" << request.text << "': " << e.what();
                            _log(ss.str(), System::LogLevel::Error);
                        }
                        const auto utf32Begin = utf32.begin();
                        std::vector<std::shared_ptr<Glyph> > glyphs(utf32.size());
                        const auto fontInfoList = p.getFontInfoList(request.fontInfo);
                        for (auto i = utf32Begin; i != utf32.end(); ++i)
                        {
                            glyphs[i - utf32Begin] = p.getGlyph(*i, fontInfoList);
                        }

                        glm::vec2 pos = glm::vec2(0.F, static_cast<float>(ftFace->size->metrics.height) / 64.F);
                        auto lineBegin = utf32.begin();
                        auto lineBreak = utf32.end();
                        float lineBreakPos = 0.F;
                        int32_t rsbDeltaPrev = 0;
                        auto i = utf32.begin();
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
                                    line.size = glm::vec2(pos.x, static_cast<float>(ftFace->size->metrics.height) / 64.F);
                                    line.glyphs = std::vector<std::shared_ptr<Glyph> >(glyphs.begin() + offset, glyphs.begin() + offset + size);
                                    lines.push_back(line);
                                }
                                catch (const std::exception& e)
                                {
                                    std::stringstream ss;
                                    ss << "Error converting string" << " '" << request.text << "': " << e.what();
                                    _log(ss.str(), System::LogLevel::Error);
                                }
                                pos.x = 0.F;
                                pos.y += static_cast<float>(ftFace->size->metrics.height) / 64.F;
                                lineBegin = i;
                                lineBreak = utf32.end();
                                rsbDeltaPrev = 0;
                            }
                            else if (
                                request.maxLineWidth > 0.F &&
                                pos.x > 0.F
                                && pos.x + (!isSpace(*i) ? advance : 0) >= request.maxLineWidth)
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
                                        line.size = glm::vec2(lineBreakPos, static_cast<float>(ftFace->size->metrics.height) / 64.F);
                                        line.glyphs = std::vector<std::shared_ptr<Glyph> >(glyphs.begin() + offset, glyphs.begin() + offset + size);
                                        lines.push_back(line);
                                    }
                                    catch (const std::exception& e)
                                    {
                                        std::stringstream ss;
                                        ss << "Error converting string" << " '" << request.text << "': " << e.what();
                                        _log(ss.str(), System::LogLevel::Error);
                                    }
                                    pos.x = 0.F;
                                    pos.y += static_cast<float>(ftFace->size->metrics.height / 64.F);
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
                                        line.size = glm::vec2(pos.x, static_cast<float>(ftFace->size->metrics.height) / 64.F);
                                        line.glyphs = std::vector<std::shared_ptr<Glyph> >(glyphs.begin() + offset, glyphs.begin() + offset + size);
                                        lines.push_back(line);
                                    }
                                    catch (const std::exception& e)
                                    {
                                        std::stringstream ss;
                                        ss << "Error converting string" << " '" << request.text << "': " << e.what();
                                        _log(ss.str(), System::LogLevel::Error);
                                    }
                                    pos.x = advance;
                                    pos.y += static_cast<float>(ftFace->size->metrics.height) / 64.F;
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
                                textLine.size = glm::vec2(pos.x, static_cast<float>(ftFace->size->metrics.height) / 64.F);
                                textLine.glyphs = std::vector<std::shared_ptr<Glyph> >(glyphs.begin() + offset, glyphs.begin() + offset + size);
                                lines.push_back(textLine);
                            }
                            catch (const std::exception& e)
                            {
                                std::stringstream ss;
                                ss << "Error converting string" << " '" << request.text << "': " << e.what();
                                _log(ss.str(), System::LogLevel::Error);
                            }
                        }
                    }
                    request.promise.set_value(lines);
                }
                p.textLinesRequests.clear();
            }

            std::vector<FontInfo> FontSystem::Private::getFontInfoList(const FontInfo& fontInfo) const
            {
                std::vector<FontInfo> out;
                out.push_back(fontInfo);
                for (const auto& i : symbolFonts)
                {
                    out.push_back(FontInfo(i.first, i.second, fontInfo.getSize(), fontInfo.getDPI()));
                }
                return out;
            }

            FT_Face FontSystem::Private::getFace(FamilyID family, FaceID face) const
            {
                FT_Face out = nullptr;
                const auto i = fontFaces.find(family);
                if (i != fontFaces.end())
                {
                    const auto j = i->second.find(face);
                    if (j != i->second.end())
                    {
                        out = j->second;
                    }
                }
                return out;
            }

            std::shared_ptr<Glyph> FontSystem::Private::getGlyph(uint32_t code, const std::vector<FontInfo>& fontInfoList)
            {
                std::shared_ptr<Glyph> out;
                for (const auto& fontInfo : fontInfoList)
                {
                    if (glyphCache.get(GlyphInfo(code, fontInfo), out))
                    {
                        break;
                    }
                    else if (auto ftFace = getFace(fontInfo.getFamily(), fontInfo.getFace()))
                    {
                        if (auto ftGlyphIndex = FT_Get_Char_Index(ftFace, code))
                        {
                            FT_Error ftError = FT_Set_Pixel_Sizes(
                                ftFace,
                                0,
                                static_cast<int>(fontInfo.getSize()));
                            if (ftError)
                            {
                                //std::cout << "FT_Set_Pixel_Sizes error: " << ftError << std::endl;
                                return nullptr;
                            }

                            ftError = FT_Load_Glyph(ftFace, ftGlyphIndex, FT_LOAD_FORCE_AUTOHINT);
                            if (ftError)
                            {
                                //std::cout << "FT_Load_Glyph error: " << ftError << std::endl;
                                return nullptr;
                            }
                            FT_Render_Mode renderMode = FT_RENDER_MODE_NORMAL;
                            uint8_t renderModeChannels = 1;
                            if (lcdRenderingThread)
                            {
                                renderMode = FT_RENDER_MODE_LCD;
                                renderModeChannels = 3;
                            }
                            ftError = FT_Render_Glyph(ftFace->glyph, renderMode);
                            if (ftError)
                            {
                                //std::cout << "FT_Render_Glyph error: " << ftError << std::endl;
                                return nullptr;
                            }
                            FT_Glyph ftGlyph;
                            ftError = FT_Get_Glyph(ftFace->glyph, &ftGlyph);
                            if (ftError)
                            {
                                //std::cout << "FT_Get_Glyph error: " << ftError << std::endl;
                                return nullptr;
                            }
                            FT_Vector v;
                            v.x = 0;
                            v.y = 0;
                            ftError = FT_Glyph_To_Bitmap(&ftGlyph, renderMode, &v, 0);
                            if (ftError)
                            {
                                //std::cout << "FT_Glyph_To_Bitmap error: " << ftError << std::endl;
                                FT_Done_Glyph(ftGlyph);
                                return nullptr;
                            }

                            out = Glyph::create();
                            out->glyphInfo = GlyphInfo(code, fontInfo);
                            out->imageData = convert(reinterpret_cast<FT_BitmapGlyph>(ftGlyph)->bitmap, renderModeChannels);
                            out->offset = glm::vec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top);
                            out->advance = static_cast<float>(ftFace->glyph->advance.x / 64.F);
                            out->lsbDelta = ftFace->glyph->lsb_delta;
                            out->rsbDelta = ftFace->glyph->rsb_delta;
                            FT_Done_Glyph(ftGlyph);

                            glyphCache.add(out->glyphInfo, out);
                            glyphCacheSize = glyphCache.getSize();
                            glyphCachePercentageUsed = glyphCache.getPercentageUsed();

                            break;
                        }
                    }
                }
                return out;
            }

            void FontSystem::Private::measure(
                const std::basic_string<djv_char_t>& utf32,
                const std::vector<FontInfo>& fontInfoList,
                uint16_t maxLineWidth,
                glm::vec2& size,
                std::vector<Math::BBox2f>* glyphGeom)
            {
                glm::vec2 pos(0.F, 0.F);
                for (const auto& fontInfo : fontInfoList)
                {
                    if (auto ftFace = getFace(fontInfo.getFamily(), fontInfo.getFace()))
                    {
                        /*FT_Error ftError = FT_Set_Char_Size(
                            ftFace->second,
                            0,
                            static_cast<int>(fontInfo.getSize() * 64.F),
                            fontInfo.getDPI(),
                            fontInfo.getDPI());*/
                        FT_Error ftError = FT_Set_Pixel_Sizes(
                            ftFace,
                            0,
                            static_cast<int>(fontInfo.getSize()));
                        if (ftError)
                        {
                            //std::cout << "FT_Set_Pixel_Sizes error: " << ftError << std::endl;
                            break;
                        }

                        pos.y = static_cast<float>(ftFace->size->metrics.height) / 64.F;
                        auto textLine = utf32.end();
                        float textLineX = 0.F;
                        int32_t rsbDeltaPrev = 0;
                        for (auto i = utf32.begin(); i != utf32.end(); ++i)
                        {
                            const auto glyph = getGlyph(*i, fontInfoList);
                            if (glyph && glyphGeom)
                            {
                                glyphGeom->push_back(Math::BBox2f(
                                    pos.x,
                                    glyph->advance,
                                    glyph->advance,
                                    static_cast<float>(ftFace->size->metrics.height) / 64.F));
                            }

                            int32_t x = 0;
                            glm::vec2 posAndSize(0.F, 0.F);
                            if (glyph && glyph->imageData)
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
                                pos.y += static_cast<float>(ftFace->size->metrics.height) / 64.F;
                                rsbDeltaPrev = 0;
                            }
                            else if (
                                pos.x > 0.F &&
                                pos.x + (!isSpace(*i) ? static_cast<float>(x) : 0.F) >= maxLineWidth)
                            {
                                if (textLine != utf32.end())
                                {
                                    i = textLine;
                                    textLine = utf32.end();
                                    size.x = std::max(size.x, textLineX);
                                    pos.x = 0.F;
                                    pos.y += static_cast<float>(ftFace->size->metrics.height) / 64.F;
                                }
                                else
                                {
                                    size.x = std::max(size.x, pos.x);
                                    pos.x = static_cast<float>(x);
                                    pos.y += static_cast<float>(ftFace->size->metrics.height) / 64.F;
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
                                pos.x += static_cast<float>(x);
                            }
                        }
                        break;
                    }
                }
                size.x = std::max(size.x, pos.x);
                size.y = pos.y;
            }

        } // namespace Font
    } // namespace Render2D
} // namespace djv
