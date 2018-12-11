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

#include <djvAV/PixelData.h>

#include <djvCore/ISystem.h>

#include <glm/vec2.hpp>

#include <future>
#include <map>

namespace djv
{
    namespace AV
    {
        struct Font
        {
            Font();
            Font(const std::string& name, float size);

            std::string name;
            float size = 0.f;
        };

        struct FontMetrics
        {
            float ascender = 0.f;
            float descender = 0.f;
            float lineHeight = 0.f;
        };

        struct FontLine
        {
            FontLine();
            FontLine(const std::string& text, const glm::vec2&);

            std::string text;
            glm::vec2 size = glm::vec2(0.f, 0.f);
        };

        struct FontGlyph
        {
            uint32_t code = 0;
            Font font;
            std::shared_ptr<Pixel::Data> pixelData;
            glm::vec2 offset = glm::vec2(0.f, 0.f);
            float advance = 0.f;
        };

        typedef size_t FontGlyphHash;
        FontGlyphHash getFontGlyphHash(uint32_t, const Font&);

        //! This class provides a font system.
        //!
        //! \todo [1.0 M]  Implement better line breaking.
        class FontSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(FontSystem);

        protected:
            void _init(const glm::vec2& dpi, Core::Context *);
            FontSystem();

        public:
            virtual ~FontSystem();

            static std::shared_ptr<FontSystem> create(const glm::vec2& dpi, Core::Context *);

            std::future<std::map<std::string, std::string> > getFileNames();
            std::future<FontMetrics> getMetrics(const Font&);
            std::future<glm::vec2> measure(const std::string& text, const Font&);
            std::future<glm::vec2> measure(const std::string& text, float maxLineWidth, const Font&);
            std::future<std::vector<FontLine> > breakLines(const std::string& text, float maxLineWidth, const Font&);
            std::future<std::vector<std::shared_ptr<FontGlyph> > > getGlyphs(const std::string& text, const Font&);

        private:
            void _initFreeType();
            void _delFreeType();
            void _handleMetricsRequests();
            void _handleMeasureRequests();
            void _handleBreakLinesRequests();
            void _handleGlyphsRequests();

            DJV_PRIVATE();
        };

    } // namespace AV
} // namespace djv
