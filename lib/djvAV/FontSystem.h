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

#pragma once

#include <djvAV/ImageData.h>

#include <djvCore/ISystem.h>
#include <djvCore/UID.h>

#include <glm/vec2.hpp>

#include <future>
#include <map>

namespace djv
{
    namespace AV
    {
        //! This namespace provides font functionality.
        namespace Font
        {
            //! This class provides font information.
            struct Info
            {
                Info();
                Info(const std::string & family, const std::string & face, float size, int DPI);

                std::string family = familyDefault;
                std::string face   = faceDefault;
                float       size   = 0.f;
                int         dpi    = dpiDefault;

                static const std::string familyDefault;
                static const std::string faceDefault;
            };

            //! This struct provides font metrics.
            struct Metrics
            {
                float ascender   = 0.f;
                float descender  = 0.f;
                float lineHeight = 0.f;
            };

            //! This struct provides a line of text.
            //!
            //! \todo Instead of copying the text should we use indices instead?
            struct TextLine
            {
                TextLine();
                TextLine(const std::string& text, const glm::vec2&);

                std::string text;
                glm::vec2   size = glm::vec2(0.f, 0.f);
            };

            //! This class provides font glyph information.
            class GlyphInfo
            {
            public:
                GlyphInfo();
                GlyphInfo(uint32_t code, const Info &);

                inline uint32_t getCode() const;
                inline const Info & getInfo() const;
                inline Core::UID getUID() const;

            private:
                uint32_t  _code = 0;
                Info      _info;
                Core::UID _uid  = 0;
            };

            //! This class provides a font glyph.
            class Glyph
            {
                DJV_NON_COPYABLE(Glyph);

            protected:
                void _init(
                    const GlyphInfo &,
                    const std::shared_ptr<Image::Data> &,
                    const glm::vec2 & offset,
                    float advance);

                Glyph();

            public:
                static std::shared_ptr<Glyph> create(
                    const GlyphInfo &,
                    const std::shared_ptr<Image::Data> &,
                    const glm::vec2 & offset,
                    float advance);

                inline const GlyphInfo & getInfo() const;
                inline const std::shared_ptr<Image::Data> & getImageData() const;
                inline glm::vec2 getOffset() const;
                inline float getAdvance() const;

            private:
                GlyphInfo                    _info;
                std::shared_ptr<Image::Data> _imageData;
                glm::vec2                    _offset    = glm::vec2(0.f, 0.f);
                float                        _advance   = 0.f;
            };

            //! This class provides a font system.
            //!
            //! \todo Why do the fonts seem too large?
            //! \todo Why do the glyphs seem smaller than the metrics?
            //! \todo Add support for LCD pixel sub-sampling.
            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);

            protected:
                void _init(Core::Context *);

                System();

            public:
                virtual ~System();

                static std::shared_ptr<System> create(Core::Context *);

                //! Get the font family and file names.
                std::future<std::map<std::string, std::string> > getFontNames();

                //! Get font metrics.
                std::future<Metrics> getMetrics(const Info&);

                //! Measure the size of text.
                std::future<glm::vec2> measure(const std::string& text, const Info&);

                //! Measure the size of text with word wrapping.
                std::future<glm::vec2> measure(const std::string& text, float maxLineWidth, const Info&);

                //! Break text into lines for word wrapping.
                std::future<std::vector<TextLine> > breakLines(const std::string& text, float maxLineWidth, const Info&);

                //! Get font glyphs.
                std::future<std::vector<std::shared_ptr<Glyph> > > getGlyphs(const std::string& text, const Info&);

            private:
                void _initFreeType();
                void _delFreeType();
                void _handleMetricsRequests();
                void _handleMeasureRequests();
                void _handleBreakLinesRequests();
                void _handleGlyphsRequests();

                DJV_PRIVATE();
            };

        } // namespace Font
    } // namespace AV
} // namespace djv

#include <djvAV/FontSystemInline.h>
