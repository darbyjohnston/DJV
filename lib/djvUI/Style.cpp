// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Style.h>

#include <djvUI/IconSystem.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StyleSettings.h>

#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>

#if defined(GetObject)
#undef GetObject
#endif // GetObject

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Style
        {
            Palette::Palette() 
            {
                _colors =
                {
                    // These colors should be specified as RGBA F32.
                    { ColorRole::None, AV::Image::Color(0.F, 0.F, 0.F, 0.F) },
                    { ColorRole::Background, AV::Image::Color(.18F, .18F, .18F, 1.F) },
                    { ColorRole::BackgroundHeader, AV::Image::Color(.4F, .4F, .4F, 1.F) },
                    { ColorRole::BackgroundBellows, AV::Image::Color(.3F, .3F, .3F, 1.F) },
                    { ColorRole::BackgroundToolBar, AV::Image::Color(.35F, .35F, .35F, 1.F) },
                    { ColorRole::Foreground, AV::Image::Color(1.F, 1.F, 1.F, 1.F) },
                    { ColorRole::ForegroundDim, AV::Image::Color(.67F, .67F, .67F, 1.F) },
                    { ColorRole::Border, AV::Image::Color(.25F, .25F, .25F, 1.F) },
                    { ColorRole::BorderButton, AV::Image::Color(.49F, .49F, .49F, 1.F) },
                    { ColorRole::Trough, AV::Image::Color(.15F, .15F, .15F, 1.F) },
                    { ColorRole::Button, AV::Image::Color(.4F, .4F, .4F, 1.F) },
                    { ColorRole::Hovered, AV::Image::Color(1.F, 1.F, 1.F, .06F) },
                    { ColorRole::Pressed, AV::Image::Color(1.F, 1.F, 1.F, .18F) },
                    { ColorRole::Checked, AV::Image::Color(.58F, .32F, .18F, 1.F) },
                    { ColorRole::TextFocus, AV::Image::Color(.7F, .5F, .2F, 1.F) },
                    { ColorRole::TooltipBackground, AV::Image::Color(1.F, 1.F, .75F, 1.F) },
                    { ColorRole::TooltipForeground, AV::Image::Color(0.F, 0.F, 0.F, 1.F) },
                    { ColorRole::Overlay, AV::Image::Color(0.F, 0.F, 0.F, .7F) },
                    { ColorRole::OverlayLight, AV::Image::Color(0.F, 0.F, 0.F, .7F) },
                    { ColorRole::Shadow, AV::Image::Color(0.F, 0.F, 0.F, .16F) },
                    { ColorRole::Handle, AV::Image::Color(.58F, .32F, .18F, 1.F) },
                    { ColorRole::Cached, AV::Image::Color(.32F, .58F, .18F, 1.F) },
                    { ColorRole::Warning, AV::Image::Color(.5F, .5F, .2F, 1.F) },
                    { ColorRole::Error, AV::Image::Color(.5F, .2F, .2F, 1.F) }
                };
            }

            void Palette::setColor(ColorRole role, const AV::Image::Color& value)
            {
                _colors[role] = value.getType() != AV::Image::Type::None ?
                    value.convert(AV::Image::Type::RGBA_F32) :
                    AV::Image::Color(AV::Image::Type::RGBA_F32);
            }

            void Palette::setDisabledMult(float value)
            {
                _disabledMult = value;
            }

            bool Palette::operator == (const Palette& other) const
            {
                return _colors == other._colors &&
                    _disabledMult == other._disabledMult;
            }

            Metrics::Metrics()
            {
                _metrics =
                {
                    { MetricsRole::None, 0.F },
                    { MetricsRole::Border, 1.F },
                    { MetricsRole::BorderTextFocus, 2.F },
                    { MetricsRole::Margin, 10.F },
                    { MetricsRole::MarginSmall, 5.F },
                    { MetricsRole::MarginLarge, 20.F },
                    { MetricsRole::MarginInside, 3.F },
                    { MetricsRole::MarginDialog, 50.F },
                    { MetricsRole::Spacing, 10.F },
                    { MetricsRole::SpacingSmall, 5.F },
                    { MetricsRole::SpacingLarge, 40.F },
                    { MetricsRole::Drag, 20.F },
                    { MetricsRole::Icon, static_cast<float>(iconSizeDefault) },
                    { MetricsRole::IconSmall, 16.F },
                    { MetricsRole::IconMini, 8.F },
                    { MetricsRole::FontSmall, 10.F },
                    { MetricsRole::FontMedium, 12.F },
                    { MetricsRole::FontLarge, 16.F },
                    { MetricsRole::FontHeader, 20.F },
                    { MetricsRole::FontTitle, 32.F },
                    { MetricsRole::Swatch, 50.F },
                    { MetricsRole::SwatchSmall, 16.F },
                    { MetricsRole::Slider, 50.F },
                    { MetricsRole::ScrollArea, 200.F },
                    { MetricsRole::ScrollAreaSmall, 100.F },
                    { MetricsRole::ScrollBar, 15.F },
                    { MetricsRole::ScrollBarSmall, 10.F },
                    { MetricsRole::Menu, 600.F },
                    { MetricsRole::TextColumn, 200.F },
                    { MetricsRole::TextColumnLarge, 300.F },
                    { MetricsRole::SearchBox, 100.F },
                    { MetricsRole::Dialog, 400.F },
                    { MetricsRole::Shadow, 20.F },
                    { MetricsRole::ShadowSmall, 10.F },
                    { MetricsRole::TooltipOffset, 10.F },
                    { MetricsRole::Handle, 10.F },
                    { MetricsRole::Move, 10.F },
                    { MetricsRole::Scrub, 10.F }
                };
            }

            void Metrics::setMetric(MetricsRole role, float value)
            {
                _metrics[role] = value;
            }

            bool Metrics::operator == (const Metrics& other) const
            {
                return _metrics == other._metrics;
            }

            void Style::_init(const std::shared_ptr<Core::Context>& context)
            {
                auto settingsSystem = context->getSystemT<Settings::System>();
                auto styleSettings = settingsSystem->getSettingsT<Settings::Style>();
                auto weak = std::weak_ptr<Style>(shared_from_this());
                _paletteObserver = ValueObserver<UI::Style::Palette>::create(
                    styleSettings->observeCurrentPalette(),
                    [weak](const UI::Style::Palette& value)
                {
                    if (auto style = weak.lock())
                    {
                        style->setPalette(value);
                    }
                });

                _brightnessObserver = ValueObserver<float>::create(
                    styleSettings->observeBrightness(),
                    [weak](float value)
                {
                    if (auto style = weak.lock())
                    {
                        style->setBrightness(value);
                    }
                });

                _contrastObserver = ValueObserver<float>::create(
                    styleSettings->observeContrast(),
                    [weak](float value)
                {
                    if (auto style = weak.lock())
                    {
                        style->setContrast(value);
                    }
                });

                _metricsObserver = ValueObserver<UI::Style::Metrics>::create(
                    styleSettings->observeCurrentMetrics(),
                    [weak](const UI::Style::Metrics& value)
                {
                    if (auto style = weak.lock())
                    {
                        style->setMetrics(value);
                    }
                });

                _fontObserver = ValueObserver<std::string>::create(
                    styleSettings->observeCurrentFont(),
                    [weak](const std::string& value)
                {
                    if (auto style = weak.lock())
                    {
                        style->setFont(value);
                    }
                });

                auto fontSystem = context->getSystemT<AV::Font::System>();
                _fontNamesObserver = MapObserver<AV::Font::FamilyID, std::string>::create(
                    fontSystem->observeFontNames(),
                    [weak](const std::map<AV::Font::FamilyID, std::string>& value)
                    {
                        if (auto style = weak.lock())
                        {
                            style->_fontNames = value;
                            for (const auto& i : style->_fontNames)
                            {
                                style->_fontNameToId[i.second] = i.first;
                            }
                            style->_fontDirty = true;
                        }
                    });

                _fontFacesObserver = MapObserver<AV::Font::FamilyID, std::map<AV::Font::FaceID, std::string> >::create(
                    fontSystem->observeFontFaces(),
                    [weak](const std::map<AV::Font::FamilyID, std::map<AV::Font::FaceID, std::string> >& value)
                    {
                        if (auto style = weak.lock())
                        {
                            style->_fontFaces = value;
                            for (const auto& i : style->_fontFaces)
                            {
                                for (const auto& j : i.second)
                                {
                                    style->_fontFaceToId[std::make_pair(i.first, j.second)] = j.first;
                                }
                            }
                            style->_fontDirty = true;
                        }
                    });
            }

            Style::Style()
            {}

            std::shared_ptr<Style> Style::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<Style>(new Style);
                out->_init(context);
                return out;
            }

            namespace
            {
                float colorCorrection(float value, float brightness, float contrast)
                {
                    return (((value * brightness) - .5F) * contrast) + .5F;
                }

            } // namespace

            AV::Image::Color Style::getColor(ColorRole role) const
            {
                auto color = _palette.getColor(role);
                if (_brightness != 1.F || _contrast != 1.F)
                {
                    switch (color.getType())
                    {
                    case AV::Image::Type::L_F32:
                    case AV::Image::Type::LA_F32:
                        color.setF32(colorCorrection(color.getF32(0), _brightness, _contrast), 0);
                        break;
                    case AV::Image::Type::RGB_F32:
                    case AV::Image::Type::RGBA_F32:
                        color.setF32(colorCorrection(color.getF32(0), _brightness, _contrast), 0);
                        color.setF32(colorCorrection(color.getF32(1), _brightness, _contrast), 1);
                        color.setF32(colorCorrection(color.getF32(2), _brightness, _contrast), 2);
                        break;
                    default:
                        color = color.convert(AV::Image::Type::RGBA_F32);
                        color.setF32(colorCorrection(color.getF32(0), _brightness, _contrast), 0);
                        color.setF32(colorCorrection(color.getF32(1), _brightness, _contrast), 1);
                        color.setF32(colorCorrection(color.getF32(2), _brightness, _contrast), 2);
                        break;
                    }
                }
                return color;
            }

            void Style::setPalette(const Palette& value)
            {
                if (value == _palette)
                    return;
                _palette = value;
                _paletteDirty = true;
            }

            void Style::setBrightness(float value)
            {
                if (value == _brightness)
                    return;
                _brightness = value;
                _paletteDirty = true;
            }

            void Style::setContrast(float value)
            {
                if (value == _contrast)
                    return;
                _contrast = value;
                _paletteDirty = true;
            }

            void Style::setDPI(const glm::vec2& value)
            {
                if (value == _dpi)
                    return;
                _dpi = value;
                _sizeDirty = true;
            }

            void Style::setMetrics(const Metrics& value)
            {
                if (value == _metrics)
                    return;
                _metrics = value;
                _sizeDirty = true;
            }

            void Style::setFont(const std::string& value)
            {
                if (value == _font)
                    return;
                _font = value;
                _fontDirty = true;
            }

            AV::Font::FontInfo Style::getFontInfo(const std::string& family, const std::string& face, MetricsRole role) const
            {
                const auto i = _fontNameToId.find(family.empty() ? AV::Font::familyDefault : family);
                const AV::Font::FamilyID familyID = i != _fontNameToId.end() ? i->second : 1;
                const auto j = _fontFaceToId.find(std::make_pair(familyID, face.empty() ? AV::Font::faceDefault : face));
                return AV::Font::FontInfo(
                    familyID,
                    j != _fontFaceToId.end() ? j->second : 1,
                    ceilf(getMetric(role)),
                    static_cast<uint16_t>(_dpi.x));
            }

            AV::Font::FontInfo Style::getFontInfo(const std::string& face, MetricsRole role) const
            {
                const auto i = _fontNameToId.find(_font);
                const AV::Font::FamilyID familyID = i != _fontNameToId.end() ? i->second : 1;
                const auto j = _fontFaceToId.find(std::make_pair(familyID, face.empty() ? AV::Font::faceDefault : face));
                return AV::Font::FontInfo(
                    familyID,
                    j != _fontFaceToId.end() ? j->second : 1,
                    ceilf(getMetric(role)),
                    static_cast<uint16_t>(_dpi.x));
            }

            void Style::setClean()
            {
                _paletteDirty = false;
                _sizeDirty = false;
                _fontDirty = false;
            }

        } // namespace Style
    } // namespace UI

    rapidjson::Value toJSON(const UI::Style::Palette& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            rapidjson::Value object(rapidjson::kObjectType);
            for (auto role : UI::getColorRoleEnums())
            {
                std::stringstream ss;
                ss << role;
                std::stringstream ss2;
                ss2 << value.getColor(role);
                object.AddMember(rapidjson::Value(ss.str().c_str(), allocator), rapidjson::Value(ss2.str().c_str(), allocator), allocator);
            }
            out.AddMember("Roles", object, allocator);
            out.AddMember("DisabledMult", toJSON(value.getDisabledMult(), allocator), allocator);
        }
        return out;
    }

    rapidjson::Value toJSON(const UI::Style::Metrics& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            rapidjson::Value object(rapidjson::kObjectType);
            for (auto role : UI::getMetricsRoleEnums())
            {
                std::stringstream ss;
                ss << role;
                object.AddMember(rapidjson::Value(ss.str().c_str(), allocator), toJSON(value.getMetric(role), allocator), allocator);
            }
            out.AddMember("Roles", object, allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, UI::Style::Palette& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Roles", i.name.GetString()))
                {
                    if (i.value.IsObject())
                    {
                        for (const auto& j : i.value.GetObject())
                        {
                            UI::ColorRole role = UI::ColorRole::First;
                            std::stringstream ss(j.name.GetString());
                            ss >> role;
                            AV::Image::Color color;
                            fromJSON(j.value, color);
                            out.setColor(role, color);
                        }
                    }
                    else
                    {
                        //! \todo How can we translate this?
                        throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
                    }
                }
                else if (0 == strcmp("DisabledMult", i.name.GetString()))
                {
                    float v = 0.F;
                    fromJSON(i.value, v);
                    out.setDisabledMult(v);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, UI::Style::Metrics& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Roles", i.name.GetString()))
                {
                    if (i.value.IsObject())
                    {
                        for (const auto& j : i.value.GetObject())
                        {
                            UI::MetricsRole role = UI::MetricsRole::First;
                            std::stringstream ss(j.name.GetString());
                            ss >> role;
                            float v = 0.F;
                            fromJSON(j.value, v);
                            out.setMetric(role, v);
                        }
                    }
                    else
                    {
                        //! \todo How can we translate this?
                        throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
                    }
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
