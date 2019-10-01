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

#include <djvUI/Enum.h>

#include <djvAV/Color.h>
#include <djvAV/FontSystem.h>

#include <djvCore/BBox.h>
#include <djvCore/PicoJSON.h>

#include <glm/vec2.hpp>

namespace djv
{
    namespace UI
    {
        //! This namespace provides styling.
        namespace Style
        {
            //! This constant provides the default icon size.
            const uint16_t iconSizeDefault = 24;

            //! This class provides a color palette.
            class Palette
            {
            public:
                Palette();

                const AV::Image::Color & getColor(ColorRole) const;
                void setColor(ColorRole, const AV::Image::Color &);

                float getDisabledMult() const;
                void setDisabledMult(float);

                bool operator == (const Palette &) const;

            private:
                std::map<ColorRole, AV::Image::Color> _colors;
                float _disabledMult = .65F;
            };

            //! This class provides size metrics.
            class Metrics
            {
            public:
                Metrics();

                float getMetric(MetricsRole role) const;
                void setMetric(MetricsRole, float);

                bool operator == (const Metrics &) const;

            private:
                std::map<MetricsRole, float> _metrics;
            };

            //! This class provides the UI style.
            class Style : public std::enable_shared_from_this<Style>
            {
                DJV_NON_COPYABLE(Style);
                void _init(const std::shared_ptr<Core::Context>&);
                Style();

            public:
                //! Create a new style.
                static std::shared_ptr<Style> create(const std::shared_ptr<Core::Context>&);

                //! \name Color Palette
                ///@{

                const Palette & getPalette() const;
                const AV::Image::Color & getColor(ColorRole) const;

                void setPalette(const Palette &);

                ///@}

                //! \name Size Metrics
                ///@{

                const glm::vec2& getDPI() const;
                const Metrics & getMetrics() const;
                float getScale() const;
                float getMetric(MetricsRole) const;

                void setDPI(const glm::vec2&);
                void setMetrics(const Metrics &);

                ///@}

                //! \name Fonts
                ///@{

                const std::string getFont() const;
                void setFont(const std::string &);

                AV::Font::Info getFontInfo(const std::string & family, const std::string & face, MetricsRole) const;
                AV::Font::Info getFontInfo(const std::string & face, MetricsRole) const;

                ///@}

                bool isDirty() const;
                void setClean();

            private:
                Palette _palette;
                glm::vec2 _dpi = glm::vec2(AV::dpiDefault, AV::dpiDefault);
                Metrics _metrics;
                std::string _font = AV::Font::familyDefault;
                std::map<AV::Font::FamilyID, std::string> _fontNames;
                std::map<std::string, AV::Font::FamilyID> _fontNameToId;
                bool _dirty = true;
            };

        } // namespace Style
    } // namespace UI

    picojson::value toJSON(const UI::Style::Palette &);
    picojson::value toJSON(const UI::Style::Metrics &);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value &, UI::Style::Palette &);
    void fromJSON(const picojson::value &, UI::Style::Metrics &);

} // namespace djv

#include <djvUI/StyleInline.h>
