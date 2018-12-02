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

#include <djvUI/ISettings.h>

#include <djvAV/Color.h>

#include <djvCore/Enum.h>
#include <djvCore/ISystem.h>
#include <djvCore/PicoJSON.h>

#include <glm/vec2.hpp>

namespace djv
{
    namespace Core
    {
        class Animation;
    
    } // namespace Core
    
    namespace UI
    {
        class StyleSettings;

        //! This enumeration provides the color roles.
        enum class ColorRole
        {
            None,
            Background,
            BackgroundHeader,
            BackgroundScroll,
            Foreground,
            ForegroundDim,
            Border,
            Trough,
            Button,
            Checked,
            CheckedForeground,
            Hover,
            Disabled,
            Overlay,
            Shadow,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(ColorRole);

        //! This enumeration provides the metrics roles.
        enum class MetricsRole
        {
            None,
            Border,
            Margin,
            MarginSmall,
            MarginLarge,
            Spacing,
            SpacingSmall,
            SpacingLarge,
            Drag,
            Icon,
            IconLarge,
            FontSmall,
            FontMedium,
            FontLarge,
            FontExtraLarge,
            FontExtraExtraLarge,
            Swatch,
            Thumbnail,
            Shadow,

            Count,
            First = Border
        };
        DJV_ENUM_HELPERS(MetricsRole);

        //! This class provides a color palette.
        class Palette
        {
        public:
            Palette();
            Palette(const Palette&);
            ~Palette();
            Palette& operator = (const Palette&);

            const AV::Image::Color& getColor(ColorRole) const;
            void setColor(ColorRole, const AV::Image::Color&);

            bool operator == (const Palette&) const;

        private:
            DJV_PRIVATE();
        };

        //! This class provides size metrics.
        class Metrics
        {
        public:
            Metrics();
            Metrics(const Metrics&);
            ~Metrics();
            Metrics& operator = (const Metrics&);

            //! Get a metric role. Note that this returns the unscaled value.
            float getMetric(MetricsRole role) const;
            void setMetric(MetricsRole, float);

            float getScale() const;
            void setScale(float);

            bool operator == (const Metrics&) const;

        private:
            DJV_PRIVATE();
        };

        //! This struct provides text metrics.
        struct TextMetrics
        {
            float ascender   = 0.f;
            float descender  = 0.f;
            float lineHeight = 0.f;
        };

        //! This class provides the UI style.
        class Style : public Core::ISystem
        {
            DJV_NON_COPYABLE(Style);
            void _init(const std::shared_ptr<Context>&);
            Style();

        public:
            virtual ~Style();

            //! Create a new style.
            static std::shared_ptr<Style> create(const std::shared_ptr<Context>&);

            //! Get the style settings.
            const std::shared_ptr<StyleSettings>& getSettings() const;

            //! \name Color Palette
            ///@{
            
            const Palette & getPalette() const;
            const AV::Image::Color & getColor(ColorRole) const;
            void setPalette(const Palette &);

            ///@}

            //! \name Size Metrics
            ///@{
            
            //! Get a metric role. Note that this returns the scaled value.
            float getMetric(MetricsRole) const;
            void setMetrics(const Metrics&);

            float getScale() const;
            void setScale(float);

            const glm::vec2& getDPI() const;
            void setDPI(const glm::vec2&);

            ///@}

            //! \name Fonts
            ///@{

            //Graphics::Font getFont(FontFace, MetricsRole) const;

            ///@}

        private:
            void _updateCurrentFont();

            DJV_PRIVATE();
        };

    } // namespace UI

    template<>
    inline picojson::value toJSON<UI::Palette>(const UI::Palette&);
    template<>
    inline picojson::value toJSON<std::string, UI::Palette>(const std::map<std::string, UI::Palette>&);
    template<>
    inline picojson::value toJSON<UI::Metrics>(const UI::Metrics&);
    template<>
    inline picojson::value toJSON<std::string, UI::Metrics>(const std::map<std::string, UI::Metrics>&);

    //! Throws:
    //! - std::exception
    template<>
    inline void fromJSON<UI::Palette>(const picojson::value&, UI::Palette&);
    template<>
    inline void fromJSON<std::string, UI::Palette>(const picojson::value&, std::map<std::string, UI::Palette>&);
    template<>
    inline void fromJSON<UI::Metrics>(const picojson::value&, UI::Metrics&);
    template<>
    inline void fromJSON<std::string, UI::Metrics>(const picojson::value&, std::map<std::string, UI::Metrics>&);

    DJV_ENUM_SERIALIZE_HELPERS(UI::ColorRole);
    DJV_ENUM_SERIALIZE_HELPERS(UI::MetricsRole);

} // namespace djv

#include <djvUI/StyleInline.h>
