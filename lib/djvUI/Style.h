// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvRender2D/FontSystem.h>

#include <djvImage/Color.h>

#include <djvMath/BBox.h>

#include <djvCore/MapObserver.h>
#include <djvCore/RapidJSONFunc.h>
#include <djvCore/ValueObserver.h>

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

                const Image::Color& getColor(ColorRole) const;

                void setColor(ColorRole, const Image::Color&);

                float getDisabledMult() const;

                void setDisabledMult(float);

                bool operator == (const Palette&) const;

            private:
                std::map<ColorRole, Image::Color> _colors;
                float _disabledMult = .65F;
            };

            //! This class provides size metrics.
            class Metrics
            {
            public:
                Metrics();

                float getMetric(MetricsRole role) const;

                void setMetric(MetricsRole, float);

                bool operator == (const Metrics&) const;

            private:
                std::map<MetricsRole, float> _metrics;
            };

            //! This class provides the UI style.
            class Style : public std::enable_shared_from_this<Style>
            {
                DJV_NON_COPYABLE(Style);
                void _init(const std::shared_ptr<System::Context>&);
                Style();

            public:
                //! Create a new style.
                static std::shared_ptr<Style> create(const std::shared_ptr<System::Context>&);

                //! \name Color Palette
                ///@{

                const Palette& getPalette() const;
                Image::Color getColor(ColorRole) const;
                float getBrightness() const;
                float getContrast() const;

                void setPalette(const Palette&);
                void setBrightness(float);
                void setContrast(float);

                ///@}

                //! \name Size Metrics
                ///@{

                const glm::vec2& getDPI() const;
                const Metrics& getMetrics() const;
                float getScale() const;
                float getMetric(MetricsRole) const;

                void setDPI(const glm::vec2&);
                void setMetrics(const Metrics&);

                ///@}

                //! \name Fonts
                ///@{

                const std::string getFont() const;
                void setFont(const std::string&);

                Render2D::Font::FontInfo getFontInfo(const std::string& family, const std::string& face, MetricsRole) const;
                Render2D::Font::FontInfo getFontInfo(const std::string& face, MetricsRole) const;

                ///@}

                bool isPaletteDirty() const;
                bool isSizeDirty() const;
                bool isFontDirty() const;
                void setClean();

            private:
                Palette _palette;
                float _brightness = 1.F;
                float _contrast = 1.F;
                glm::vec2 _dpi = glm::vec2(Render2D::dpiDefault, Render2D::dpiDefault);
                Metrics _metrics;
                std::string _font = Render2D::Font::familyDefault;
                std::map<Render2D::Font::FamilyID, std::string> _fontNames;
                std::map<std::string, Render2D::Font::FamilyID> _fontNameToId;
                std::map<Render2D::Font::FamilyID, std::map<Render2D::Font::FaceID, std::string> > _fontFaces;
                std::map<std::pair<Render2D::Font::FamilyID, std::string>, Render2D::Font::FaceID> _fontFaceToId;
                std::shared_ptr<Core::ValueObserver<UI::Style::Palette> > _paletteObserver;
                std::shared_ptr<Core::ValueObserver<float> > _brightnessObserver;
                std::shared_ptr<Core::ValueObserver<float> > _contrastObserver;
                std::shared_ptr<Core::ValueObserver<UI::Style::Metrics> > _metricsObserver;
                std::shared_ptr<Core::ValueObserver<std::string> > _fontObserver;
                std::shared_ptr<Core::MapObserver<Render2D::Font::FamilyID, std::string> > _fontNamesObserver;
                std::shared_ptr<Core::MapObserver<Render2D::Font::FamilyID, std::map<Render2D::Font::FaceID, std::string> > > _fontFacesObserver;
                bool _paletteDirty = true;
                bool _sizeDirty = true;
                bool _fontDirty = true;
            };

        } // namespace Style
    } // namespace UI

    rapidjson::Value toJSON(const UI::Style::Palette&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const UI::Style::Metrics&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::Style::Palette&);
    void fromJSON(const rapidjson::Value&, UI::Style::Metrics&);

} // namespace djv

#include <djvUI/StyleInline.h>
