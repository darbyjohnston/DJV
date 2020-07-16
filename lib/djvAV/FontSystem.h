// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/ImageData.h>

#include <djvCore/BBox.h>
#include <djvCore/ISystem.h>
#include <djvCore/MapObserver.h>
#include <djvCore/Range.h>

#include <future>

namespace djv
{
    namespace AV
    {
        //! This namespace provides font functionality.
        namespace Font
        {
            typedef uint16_t FamilyID;
            typedef uint16_t FaceID;

            const std::string familyDefault = "Noto Sans";
            const std::string faceDefault   = "Regular";
            const std::string familyMono    = "Noto Mono";

            //! This class provides font information.
            class FontInfo
            {
            public:
                FontInfo();
                FontInfo(FamilyID, FaceID, uint16_t size, uint16_t DPI);

                FamilyID getFamily() const;
                FaceID   getFace() const;
                uint16_t getSize() const;
                uint16_t getDPI() const;

                bool operator == (const FontInfo&) const;
                bool operator < (const FontInfo&) const;
                
            private:
                FamilyID _family = 1;
                FaceID   _face   = 1;
                uint16_t _size   = 0;
                uint16_t _dpi    = dpiDefault;
                size_t   _hash   = 0;
            };

            //! This struct provides font metrics.
            class Metrics
            {
            public:
                Metrics();
                
                uint16_t ascender   = 0;
                uint16_t descender  = 0;
                uint16_t lineHeight = 0;
            };

            //! This struct provides font glyph information.
            class GlyphInfo
            {
            public:
                GlyphInfo();
                GlyphInfo(uint32_t code, const FontInfo&);

                uint32_t code = 0;
                FontInfo fontInfo;

                bool operator == (const GlyphInfo&) const;
                bool operator < (const GlyphInfo&) const;
            };

            //! This struct provides a font glyph.
            class Glyph
            {
                DJV_NON_COPYABLE(Glyph);

            protected:
                Glyph();

            public:
                static std::shared_ptr<Glyph> create();

                GlyphInfo                    glyphInfo;
                std::shared_ptr<Image::Data> imageData;
                glm::vec2                    offset    = glm::vec2(0.F, 0.F);
                uint16_t                     advance   = 0;
                int32_t                      lsbDelta  = 0;
                int32_t                      rsbDelta  = 0;
            };

            //! This struct provides a line of text.
            class TextLine
            {
            public:
                TextLine();
                TextLine(const std::string&, const glm::vec2&, const std::vector<std::shared_ptr<Glyph> >&);

                std::string                          text;
                glm::vec2                            size = glm::vec2(0.F, 0.F);
                std::vector<std::shared_ptr<Glyph> > glyphs;
            };
            
            //! This class provides a font error.
            class Error : public std::runtime_error
            {
            public:
                explicit Error(const std::string&);
            };

            //! This class provides a font system.
            //!
            //! \todo Add support for gamma correction?
            //! - https://www.freetype.org/freetype2/docs/text-rendering-general.html
            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);

                System();

            public:
                ~System() override;

                //! Create a new font system.
                //! Throws:
                //! - Error
                static std::shared_ptr<System> create(const std::shared_ptr<Core::Context>&);

                //! Observe the font names.
                std::shared_ptr<Core::IMapSubject<FamilyID, std::string> > observeFontNames() const;

                //! Observe the font faces.
                std::shared_ptr<Core::IMapSubject<FamilyID, std::map<FaceID, std::string> > > observeFontFaces() const;

                //! Set whether LCD hinting is enabled.
                void setLCDRendering(bool);

                //! Get font metrics.
                std::future<Metrics> getMetrics(const FontInfo&);

                //! Measure the size of text.
                std::future<glm::vec2> measure(
                    const std::string& text,
                    const FontInfo&    fontInfo,
                    uint16_t           elide    = 0);

                //! Measure the size of glyphs.
                std::future<std::vector<Core::BBox2f> > measureGlyphs(
                    const std::string& text,
                    const FontInfo&    fontInfo,
                    uint16_t           elide    = 0);

                //! Get font glyphs.
                std::future<std::vector<std::shared_ptr<Glyph> > > getGlyphs(
                    const std::string& text,
                    const FontInfo&    fontInfo,
                    uint16_t           elide    = 0);

                //! Break text into lines for wrapping.
                std::future<std::vector<TextLine> > textLines(
                    const std::string& text,
                    uint16_t           maxLineWidth,
                    const FontInfo&    fontInfo);

                //! Request font glyphs to be cached.
                void cacheGlyphs(const std::string& text, const FontInfo&);

                //! Get the glyph cache size.
                size_t getGlyphCacheSize() const;

                //! Get the glyph cache percentage used.
                float getGlyphCachePercentage() const;
            
            private:
                void _initFreeType();
                void _delFreeType();
                void _handleMetricsRequests();
                void _handleMeasureRequests();
                void _handleTextLinesRequests();
                void _handleMeasureGlyphsRequests();
                void _handleGlyphsRequests();

                DJV_PRIVATE();
            };

        } // namespace Font
    } // namespace AV
} // namespace djv

#include <djvAV/FontSystemInline.h>
