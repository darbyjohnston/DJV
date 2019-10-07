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

            const std::string familyDefault = "Barlow";
            const std::string faceDefault   = "Regular";
            const std::string familyMono    = "Noto Mono";

            //! This class provides font information.
            class Info
            {
            public:
                Info();
                constexpr Info(FamilyID, FaceID, uint16_t size, uint16_t DPI);

                FamilyID family = 1;
                FaceID   face   = 1;
                uint16_t size   = 0;
                uint16_t dpi    = dpiDefault;

                bool operator == (const Info &) const;
                bool operator < (const Info&) const;
            };

            //! This struct provides font metrics.
            struct Metrics
            {
                uint16_t ascender   = 0;
                uint16_t descender  = 0;
                uint16_t lineHeight = 0;
            };

            //! This struct provides a line of text.
            struct TextLine
            {
                TextLine();
                TextLine(const std::string&, const glm::vec2 &);

                std::string text;
                glm::vec2   size = glm::vec2(0.F, 0.F);
            };

            //! This struct provides font glyph information.
            class GlyphInfo
            {
            public:
                GlyphInfo();
                GlyphInfo(uint32_t code, const Info &);

                uint32_t code = 0;
                Info     info;

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

                GlyphInfo                    info;
                std::shared_ptr<Image::Data> imageData;
                glm::vec2                    offset    = glm::vec2(0.F, 0.F);
                uint16_t                     advance   = 0;
                int32_t                      lsbDelta  = 0;
                int32_t                      rsbDelta  = 0;
            };
            
            //! This class provides a font error.
            class Error : public std::runtime_error
            {
            public:
                explicit Error(const std::string&);
            };

            //! This class provides a font system.
            //!
            //! \todo Add support for LCD pixel sub-sampling and gamma correction:
            //! - https://www.freetype.org/freetype2/docs/text-rendering-general.html
            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);

                System();

            public:
                virtual ~System();

                //! Create a new font system.
                //! Throws:
                //! - Error
                static std::shared_ptr<System> create(const std::shared_ptr<Core::Context>&);

                //! Observe the font names.
                std::shared_ptr<Core::IMapSubject<FamilyID, std::string> > observeFontNames() const;

                //! Get font metrics.
                std::future<Metrics> getMetrics(const Info &);

                //! Measure the size of text.
                std::future<glm::vec2> measure(
                    const std::string& text,
                    const Info&        info);

                //! Measure glyphs.
                std::future<std::vector<Core::BBox2f> > measureGlyphs(
                    const std::string& text,
                    const Info&        info);

                //! Break text into lines for wrapping.
                std::future<std::vector<TextLine> > textLines(
                    const std::string& text,
                    uint16_t           maxLineWidth,
                    const Info&        info);

                //! Get font glyphs.
                std::future<std::vector<std::shared_ptr<Glyph> > > getGlyphs(
                    const std::string& text,
                    const Info&        info);

                //! Request font glyphs to be cached.
                void cacheGlyphs(const std::string& text, const Info&);

                //! Get the glyph cache size.
                size_t getGlyphCacheSize() const;

                //! Get the glyph cache percentage used.
                float getGlyphCachePercentage() const;
            
            private:
                void _initFreeType();
                void _delFreeType();
                void _handleMetricsRequests();
                void _handleMeasureRequests();
                void _handleMeasureGlyphsRequests();
                void _handleTextLinesRequests();
                void _handleGlyphsRequests();

                DJV_PRIVATE();
            };

        } // namespace Font
    } // namespace AV
} // namespace djv

#include <djvAV/FontSystemInline.h>
