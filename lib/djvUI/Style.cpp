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
            struct Palette::Private
            {
                std::map<ColorRole, AV::Image::Color> colors;
                float disabledMult = .65f;
            };

            Palette::Palette() :
                _p(new Private)
            {
                DJV_PRIVATE_PTR();
                p.colors =
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
                    { ColorRole::TextFocus, AV::Image::Color(.58f, .32f, .18f, 1.f) },
                    { ColorRole::TooltipBackground, AV::Image::Color(1.f, 1.f, .75f, 1.f) },
                    { ColorRole::TooltipForeground, AV::Image::Color(0.f, 0.f, 0.f, 1.f) },
                    { ColorRole::Overlay, AV::Image::Color(0.f, 0.f, 0.f, .7f) },
                    { ColorRole::OverlayLight, AV::Image::Color(0.f, 0.f, 0.f, .7f) },
                    { ColorRole::Shadow, AV::Image::Color(0.f, 0.f, 0.f, .16f) },
                    { ColorRole::Handle, AV::Image::Color(.58f, .32f, .18f, 1.f) }
                };
            }

            Palette::Palette(const Palette & other) :
                _p(new Private)
            {
                *_p = *other._p;
            }

            Palette::~Palette()
            {}

            Palette & Palette::operator = (const Palette & other)
            {
                *_p = *other._p;
                return *this;
            }

            const AV::Image::Color & Palette::getColor(ColorRole value) const
            {
                return _p->colors.at(value);
            }

            void Palette::setColor(ColorRole role, const AV::Image::Color & value)
            {
                _p->colors[role] = value.getType() != AV::Image::Type::None ?
                    value.convert(AV::Image::Type::RGBA_F32) :
                    AV::Image::Color(AV::Image::Type::RGBA_F32);
            }

            float Palette::getDisabledMult() const
            {
                return _p->disabledMult;
            }

            void Palette::setDisabledMult(float value)
            {
                _p->disabledMult = value;
            }

            bool Palette::operator == (const Palette & other) const
            {
                return _p->colors == other._p->colors &&
                    _p->disabledMult == other._p->disabledMult;
            }

            struct Metrics::Private
            {
                std::map<MetricsRole, float> metrics;
            };

            Metrics::Metrics(const Metrics & other) :
                _p(new Private)
            {
                *_p = *other._p;
            }

            Metrics::~Metrics()
            {}

            Metrics & Metrics::operator = (const Metrics & other)
            {
                *_p = *other._p;
                return *this;
            }

            Metrics::Metrics() :
                _p(new Private)
            {
                DJV_PRIVATE_PTR();
                p.metrics =
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
                    { MetricsRole::Move, 10.f }
                };
            }

            float Metrics::getMetric(MetricsRole role) const
            {
                return _p->metrics.at(role);
            }

            void Metrics::setMetric(MetricsRole role, float value)
            {
                _p->metrics[role] = value;
            }

            bool Metrics::operator == (const Metrics & other) const
            {
                DJV_PRIVATE_PTR();
                return p.metrics == other._p->metrics;
            }

            struct Style::Private
            {
                Palette palette;
                glm::vec2 dpi = glm::vec2(AV::dpiDefault, AV::dpiDefault);
                Metrics metrics;
                std::string font = AV::Font::familyDefault;
                std::map<AV::Font::FamilyID, std::string> fontNames;
                std::map<std::string, AV::Font::FamilyID> fontNameToId;
                bool dirty = true;
            };

            void Style::_init(const std::shared_ptr<Core::Context>& context)
            {
                DJV_PRIVATE_PTR();
                                
                auto fontSystem = context->getSystemT<AV::Font::System>();
                p.fontNames = fontSystem->getFontNames().get();
                for (const auto & i : p.fontNames)
                {
                    p.fontNameToId[i.second] = i.first;
                }

                p.dirty = true;
            }

            Style::Style() :
                _p(new Private)
            {}

            Style::~Style()
            {}

            std::shared_ptr<Style> Style::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<Style>(new Style);
                out->_init(context);
                return out;
            }

            const Palette & Style::getPalette() const
            {
                return _p->palette;
            }

            const AV::Image::Color & Style::getColor(ColorRole role) const
            {
                return _p->palette.getColor(role);
            }

            void Style::setPalette(const Palette & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.palette)
                    return;
                p.palette = value;
                p.dirty = true;
            }

            const glm::vec2& Style::getDPI() const
            {
                return _p->dpi;
            }

            const Metrics & Style::getMetrics() const
            {
                return _p->metrics;
            }

            float Style::getScale() const
            {
                return _p->dpi.x / static_cast<float>(AV::dpiDefault);
            }

            float Style::getMetric(MetricsRole role) const
            {
                return ceilf(_p->metrics.getMetric(role) * getScale());
            }

            void Style::setDPI(const glm::vec2& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.dpi)
                    return;
                p.dpi = value;
                p.dirty = true;
            }

            void Style::setMetrics(const Metrics& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.metrics)
                    return;
                p.metrics = value;
                p.dirty = true;
            }

            const std::string Style::getFont() const
            {
                return _p->font;
            }

            void Style::setFont(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.font)
                    return;
                p.font = value;
                p.dirty = true;
            }

            AV::Font::Info Style::getFontInfo(const std::string & family, const std::string & face, MetricsRole role) const
            {
                DJV_PRIVATE_PTR();
                const auto i = p.fontNameToId.find(family);
                return AV::Font::Info(
                    i != p.fontNameToId.end() ? i->second : 1,
                    1,
                    ceilf(getMetric(role)),
                    static_cast<uint16_t>(p.dpi.x));
            }

            AV::Font::Info Style::getFontInfo(const std::string & face, MetricsRole role) const
            {
                DJV_PRIVATE_PTR();
                const auto i = p.fontNameToId.find(p.font);
                return AV::Font::Info(
                    i != p.fontNameToId.end() ? i->second : 1,
                    1,
                    ceilf(getMetric(role)),
                    static_cast<uint16_t>(p.dpi.x));
            }

            bool Style::isDirty() const
            {
                return _p->dirty;
            }

            void Style::setClean()
            {
                _p->dirty = false;
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

