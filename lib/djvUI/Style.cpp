//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvUI/Style.h>

#include <djvUI/IconSystem.h>

#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>

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
                    { ColorRole::Cached, AV::Image::Color(.32F, .58F, .18F, 1.F) }
                };
            }

            void Palette::setColor(ColorRole role, const AV::Image::Color & value)
            {
                _colors[role] = value.getType() != AV::Image::Type::None ?
                    value.convert(AV::Image::Type::RGBA_F32) :
                    AV::Image::Color(AV::Image::Type::RGBA_F32);
            }

            void Palette::setDisabledMult(float value)
            {
                _disabledMult = value;
            }

            bool Palette::operator == (const Palette & other) const
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
                    { MetricsRole::Margin, 10.F },
                    { MetricsRole::MarginSmall, 5.F },
                    { MetricsRole::MarginLarge, 20.F },
                    { MetricsRole::MarginDialog, 50.F },
                    { MetricsRole::Spacing, 10.F },
                    { MetricsRole::SpacingSmall, 5.F },
                    { MetricsRole::SpacingLarge, 40.F },
                    { MetricsRole::Drag, 20.F },
                    { MetricsRole::Icon, static_cast<float>(iconSizeDefault) },
                    { MetricsRole::IconSmall, 16.F },
                    { MetricsRole::IconMini, 8.F },
                    { MetricsRole::FontSmall, 12.F },
                    { MetricsRole::FontMedium, 14.F },
                    { MetricsRole::FontLarge, 18.F },
                    { MetricsRole::FontHeader, 22.F },
                    { MetricsRole::FontTitle, 32.F },
                    { MetricsRole::Swatch, 50.F },
                    { MetricsRole::SwatchSmall, 25.F },
                    { MetricsRole::Slider, 100.F },
                    { MetricsRole::ScrollArea, 200.F },
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

            bool Metrics::operator == (const Metrics & other) const
            {
                return _metrics == other._metrics;
            }

            void Style::_init(const std::shared_ptr<Core::Context>& context)
            {
                auto fontSystem = context->getSystemT<AV::Font::System>();
                auto weak = std::weak_ptr<Style>(shared_from_this());
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
                            style->_dirty = true;
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
                            style->_dirty = true;
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

            void Style::setPalette(const Palette & value)
            {
                if (value == _palette)
                    return;
                _palette = value;
                _dirty = true;
            }

            void Style::setDPI(const glm::vec2& value)
            {
                if (value == _dpi)
                    return;
                _dpi = value;
                _dirty = true;
            }

            void Style::setMetrics(const Metrics& value)
            {
                if (value == _metrics)
                    return;
                _metrics = value;
                _dirty = true;
            }

            void Style::setFont(const std::string & value)
            {
                if (value == _font)
                    return;
                _font = value;
                _dirty = true;
            }

            AV::Font::Info Style::getFontInfo(const std::string & family, const std::string & face, MetricsRole role) const
            {
                const auto i = _fontNameToId.find(family.empty() ? AV::Font::familyDefault : family);
                const AV::Font::FamilyID familyID = i != _fontNameToId.end() ? i->second : 1;
                const auto j = _fontFaceToId.find(std::make_pair(familyID, face.empty() ? AV::Font::faceDefault : face));
                return AV::Font::Info(
                    familyID,
                    j != _fontFaceToId.end() ? j->second : 1,
                    ceilf(getMetric(role)),
                    static_cast<uint16_t>(_dpi.x));
            }

            AV::Font::Info Style::getFontInfo(const std::string & face, MetricsRole role) const
            {
                const auto i = _fontNameToId.find(_font);
                const AV::Font::FamilyID familyID = i != _fontNameToId.end() ? i->second : 1;
                const auto j = _fontFaceToId.find(std::make_pair(familyID, face.empty() ? AV::Font::faceDefault : face));
                return AV::Font::Info(
                    familyID,
                    j != _fontFaceToId.end() ? j->second : 1,
                    ceilf(getMetric(role)),
                    static_cast<uint16_t>(_dpi.x));
            }

            void Style::setClean()
            {
                _dirty = false;
            }

        } // namespace Style
    } // namespace UI

    picojson::value toJSON(const UI::Style::Palette & value)
    {
        picojson::value out(picojson::object_type, true);
        {
            picojson::value object(picojson::object_type, true);
            for (auto role : UI::getColorRoleEnums())
            {
                std::stringstream ss;
                ss << role;
                std::stringstream ss2;
                ss2 << value.getColor(role);
                object.get<picojson::object>()[ss.str()] = picojson::value(ss2.str());
            }
            out.get<picojson::object>()["Roles"] = object;
            out.get<picojson::object>()["DisabledMult"] = toJSON(value.getDisabledMult());
        }
        return out;
    }

    picojson::value toJSON(const UI::Style::Metrics & value)
    {
        picojson::value out(picojson::object_type, true);
        {
            picojson::value object(picojson::object_type, true);
            for (auto role : UI::getMetricsRoleEnums())
            {
                std::stringstream ss;
                ss << role;
                object.get<picojson::object>()[ss.str()] = picojson::value(std::to_string(value.getMetric(role)));
            }
            out.get<picojson::object>()["Roles"] = object;
        }
        return out;
    }

    void fromJSON(const picojson::value & value, UI::Style::Palette & out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto & i : value.get<picojson::object>())
            {
                if ("Roles" == i.first)
                {
                    if (i.second.is<picojson::object>())
                    {
                        for (const auto & j : i.second.get<picojson::object>())
                        {
                            UI::ColorRole role = UI::ColorRole::First;
                            std::stringstream ss(j.first);
                            ss >> role;
                            if (j.second.is<std::string>())
                            {
                                AV::Image::Color color;
                                std::stringstream ss2;
                                ss2.str(j.second.get<std::string>());
                                ss2 >> color;
                                out.setColor(role, color);
                            }
                            else
                            {
                                throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
                            }
                        }
                    }
                    else
                    {
                        throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
                    }
                }
                else if ("DisabledMult" == i.first)
                {
                    float v = 0.F;
                    fromJSON(i.second, v);
                    out.setDisabledMult(v);
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const picojson::value & value, UI::Style::Metrics & out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto & i : value.get<picojson::object>())
            {
                if ("Roles" == i.first)
                {
                    if (i.second.is<picojson::object>())
                    {
                        for (const auto & j : i.second.get<picojson::object>())
                        {
                            UI::MetricsRole role = UI::MetricsRole::First;
                            std::stringstream ss(j.first);
                            ss >> role;
                            if (j.second.is<std::string>())
                            {
                                out.setMetric(role, std::stof(j.second.get<std::string>()));
                            }
                            else
                            {
                                throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
                            }
                        }
                    }
                    else
                    {
                        throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
                    }
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
