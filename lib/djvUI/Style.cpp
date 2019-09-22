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
                    { ColorRole::None, AV::Image::Color(0.f, 0.f, 0.f, 0.f) },
                    { ColorRole::Background, AV::Image::Color(.18f, .18f, .18f, 1.f) },
                    { ColorRole::BackgroundHeader, AV::Image::Color(.4f, .4f, .4f, 1.f) },
                    { ColorRole::BackgroundBellows, AV::Image::Color(.3f, .3f, .3f, 1.f) },
                    { ColorRole::BackgroundToolBar, AV::Image::Color(.35f, .35f, .35f, 1.f) },
                    { ColorRole::Foreground, AV::Image::Color(1.f, 1.f, 1.f, 1.f) },
                    { ColorRole::ForegroundDim, AV::Image::Color(.67f, .67f, .67f, 1.f) },
                    { ColorRole::Border, AV::Image::Color(.25f, .25f, .25f, 1.f) },
                    { ColorRole::BorderButton, AV::Image::Color(.49f, .49f, .49f, 1.f) },
                    { ColorRole::Trough, AV::Image::Color(.15f, .15f, .15f, 1.f) },
                    { ColorRole::Button, AV::Image::Color(.4f, .4f, .4f, 1.f) },
                    { ColorRole::Hovered, AV::Image::Color(1.f, 1.f, 1.f, .06f) },
                    { ColorRole::Pressed, AV::Image::Color(1.f, 1.f, 1.f, .18f) },
                    { ColorRole::Checked, AV::Image::Color(.58f, .32f, .18f, 1.f) },
                    { ColorRole::TextFocus, AV::Image::Color(.7f, .5f, .2f, 1.f) },
                    { ColorRole::TooltipBackground, AV::Image::Color(1.f, 1.f, .75f, 1.f) },
                    { ColorRole::TooltipForeground, AV::Image::Color(0.f, 0.f, 0.f, 1.f) },
                    { ColorRole::Overlay, AV::Image::Color(0.f, 0.f, 0.f, .7f) },
                    { ColorRole::OverlayLight, AV::Image::Color(0.f, 0.f, 0.f, .7f) },
                    { ColorRole::Shadow, AV::Image::Color(0.f, 0.f, 0.f, .16f) },
                    { ColorRole::Handle, AV::Image::Color(.58f, .32f, .18f, 1.f) },
                    { ColorRole::Cached, AV::Image::Color(.32f, .58f, .18f, 1.f) }
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
                    { MetricsRole::None, 0.f },
                    { MetricsRole::Border, 1.f },
                    { MetricsRole::Margin, 10.f },
                    { MetricsRole::MarginSmall, 5.f },
                    { MetricsRole::MarginLarge, 20.f },
                    { MetricsRole::MarginDialog, 50.f },
                    { MetricsRole::Spacing, 10.f },
                    { MetricsRole::SpacingSmall, 5.f },
                    { MetricsRole::SpacingLarge, 40.f },
                    { MetricsRole::Drag, 20.f },
                    { MetricsRole::Icon, static_cast<float>(iconSizeDefault) },
                    { MetricsRole::IconSmall, 16.f },
                    { MetricsRole::FontSmall, 12.f },
                    { MetricsRole::FontMedium, 14.f },
                    { MetricsRole::FontLarge, 18.f },
                    { MetricsRole::FontHeader, 22.f },
                    { MetricsRole::FontTitle, 32.f },
                    { MetricsRole::Swatch, 50.f },
                    { MetricsRole::Slider, 100.f },
                    { MetricsRole::ScrollArea, 200.f },
                    { MetricsRole::Menu, 600.f },
                    { MetricsRole::TextColumn, 200.f },
                    { MetricsRole::TextColumnLarge, 300.f },
                    { MetricsRole::SearchBox, 100.f },
                    { MetricsRole::Dialog, 400.f },
                    { MetricsRole::Shadow, 20.f },
                    { MetricsRole::ShadowSmall, 10.f },
                    { MetricsRole::TooltipOffset, 10.f },
                    { MetricsRole::Handle, 10.f },
                    { MetricsRole::Move, 10.f },
                    { MetricsRole::Scrub, 10.f }
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
                _fontNames = fontSystem->getFontNames().get();
                for (const auto & i : _fontNames)
                {
                    _fontNameToId[i.second] = i.first;
                }

                _dirty = true;
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
                const auto i = _fontNameToId.find(family);
                return AV::Font::Info(
                    i != _fontNameToId.end() ? i->second : 1,
                    1,
                    ceilf(getMetric(role)),
                    static_cast<uint16_t>(_dpi.x));
            }

            AV::Font::Info Style::getFontInfo(const std::string & face, MetricsRole role) const
            {
                const auto i = _fontNameToId.find(_font);
                return AV::Font::Info(
                    i != _fontNameToId.end() ? i->second : 1,
                    1,
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
                                throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
                            }
                        }
                    }
                    else
                    {
                        throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
                    }
                }
                else if ("DisabledMult" == i.first)
                {
                    float v = 0.f;
                    fromJSON(i.second, v);
                    out.setDisabledMult(v);
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
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
                                throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
                            }
                        }
                    }
                    else
                    {
                        throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
                    }
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

} // namespace djv
