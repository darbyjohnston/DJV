// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvAV/Color.h>
#include <djvAV/FontSystem.h>

#include <djvCore/BBox.h>
#include <djvCore/PicoJSON.h>
#include <djvCore/MapObserver.h>

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
                AV::Image::Color getColor(ColorRole) const;
                float getBrightness() const;
                float getContrast() const;

                void setPalette(const Palette &);
                void setBrightness(float);
                void setContrast(float);

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

                bool isPaletteDirty() const;
                bool isSizeDirty() const;
                bool isFontDirty() const;
                void setClean();

            private:
                Palette _palette;
                float _brightness = 1.F;
                float _contrast = 1.F;
                glm::vec2 _dpi = glm::vec2(AV::dpiDefault, AV::dpiDefault);
                Metrics _metrics;
                std::string _font = AV::Font::familyDefault;
                std::map<AV::Font::FamilyID, std::string> _fontNames;
                std::map<std::string, AV::Font::FamilyID> _fontNameToId;
                std::map<AV::Font::FamilyID, std::map<AV::Font::FaceID, std::string> > _fontFaces;
                std::map<std::pair<AV::Font::FamilyID, std::string>, AV::Font::FaceID> _fontFaceToId;
                std::shared_ptr<Core::MapObserver<AV::Font::FamilyID, std::string> > _fontNamesObserver;
                std::shared_ptr<Core::MapObserver<AV::Font::FamilyID, std::map<AV::Font::FaceID, std::string> > > _fontFacesObserver;
                bool _paletteDirty = true;
                bool _sizeDirty = true;
                bool _fontDirty = true;
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
