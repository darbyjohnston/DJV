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

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>

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
            };

            Palette::Palette() :
                _p(new Private)
            {
                DJV_PRIVATE_PTR();
                p.colors =
                {
                    { ColorRole::None, AV::Image::Color() },
                    { ColorRole::Background, AV::Image::Color(50, 60, 70) },
                    { ColorRole::Foreground, AV::Image::Color(255, 255, 255) },
                    { ColorRole::Border, AV::Image::Color(30, 30, 30) },
                    { ColorRole::Trough, AV::Image::Color(33, 40, 47) },
                    { ColorRole::Button, AV::Image::Color(71, 82, 93) },
                    { ColorRole::Hovered, AV::Image::Color(255, 255, 255, 15) },
                    { ColorRole::Pressed, AV::Image::Color(255, 255, 255, 30) },
                    { ColorRole::Checked, AV::Image::Color(171, 134, 48, 127) },
                    { ColorRole::Disabled, AV::Image::Color(127, 127, 127) },
                    { ColorRole::TooltipBackground, AV::Image::Color(255, 255, 191) },
                    { ColorRole::TooltipForeground, AV::Image::Color(0, 0, 0) },
                    { ColorRole::Overlay, AV::Image::Color(0, 0, 0, 160) },
                    { ColorRole::Shadow, AV::Image::Color(0, 0, 0, 50) }
                };
            }

            Palette::Palette(const Palette& other) :
                _p(new Private)
            {
                *_p = *other._p;
            }

            Palette::~Palette()
            {}

            Palette& Palette::operator = (const Palette& other)
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
                _p->colors[role] = value;
            }

            bool Palette::operator == (const Palette& other) const
            {
                return _p->colors == other._p->colors;
            }

            struct Metrics::Private
            {
                std::map<MetricsRole, float> metrics;
            };

            Metrics::Metrics(const Metrics& other) :
                _p(new Private)
            {
                *_p = *other._p;
            }

            Metrics::~Metrics()
            {}

            Metrics& Metrics::operator = (const Metrics& other)
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
                    { MetricsRole::MarginLarge, 15.f },
                    { MetricsRole::MarginDialog, 50.f },
                    { MetricsRole::Spacing, 10.f },
                    { MetricsRole::SpacingSmall, 5.f },
                    { MetricsRole::SpacingLarge, 30.f },
                    { MetricsRole::Drag, 20.f },
                    { MetricsRole::Icon, static_cast<float>(iconSizeDefault) },
                    { MetricsRole::IconSmall, 16.f },
                    { MetricsRole::FontSmall, 10.f },
                    { MetricsRole::FontMedium, 12.f },
                    { MetricsRole::FontLarge, 16.f },
                    { MetricsRole::FontHeader, 16.f },
                    { MetricsRole::Swatch, 40.f },
                    { MetricsRole::ThumbnailWidthSmall, 100.f },
                    { MetricsRole::ThumbnailHeightSmall, 50.f },
                    { MetricsRole::ThumbnailWidthLarge, 200.f },
                    { MetricsRole::ThumbnailHeightLarge, 100.f },
                    { MetricsRole::Shadow, 5.f },
                    { MetricsRole::ScrollArea, 300.f },
                    { MetricsRole::TextColumn, 200.f },
                    { MetricsRole::Dialog, 400.f },
                    { MetricsRole::TooltipOffset, 10.f },
                    { MetricsRole::Handle, 15.f }
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

            bool Metrics::operator == (const Metrics& other) const
            {
                DJV_PRIVATE_PTR();
                return p.metrics == other._p->metrics;
            }

            struct Style::Private
            {
                Context * context = nullptr;
                Palette palette;
                int dpi = AV::dpiDefault;
                Metrics metrics;
                std::string font = AV::Font::Info::familyDefault;
                bool dirty = true;
            };

            void Style::_init(int dpi, Context * context)
            {
                DJV_PRIVATE_PTR();
                p.context = context;
                p.dpi = dpi;
                _p->dirty = true;
            }

            Style::Style() :
                _p(new Private)
            {}

            Style::~Style()
            {}

            std::shared_ptr<Style> Style::create(int dpi, Context * context)
            {
                auto out = std::shared_ptr<Style>(new Style);
                out->_init(dpi, context);
                return out;
            }

            const Palette& Style::getPalette() const
            {
                return _p->palette;
            }

            const AV::Image::Color& Style::getColor(ColorRole role) const
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

            int Style::getDPI() const
            {
                return _p->dpi;
            }

            const Metrics & Style::getMetrics() const
            {
                return _p->metrics;
            }

            float Style::getScale() const
            {
                return _p->dpi / static_cast<float>(AV::dpiDefault);
            }

            float Style::getMetric(MetricsRole role) const
            {
                return ceilf(_p->metrics.getMetric(role) * getScale());
            }

            void Style::setMetrics(const Metrics & value)
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
                return AV::Font::Info(family, face, ceilf(getMetric(role)), p.dpi);
            }

            AV::Font::Info Style::getFontInfo(const std::string & face, MetricsRole role) const
            {
                DJV_PRIVATE_PTR();
                return AV::Font::Info(p.font, face, ceilf(getMetric(role)), p.dpi);
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

    picojson::value toJSON(const UI::Style::Palette& value)
    {
        picojson::value out(picojson::object_type, true);
        {
            picojson::value object(picojson::object_type, true);
            for (auto role : UI::Style::getColorRoleEnums())
            {
                std::stringstream ss;
                ss << role;
                std::stringstream ss2;
                ss2 << value.getColor(role);
                object.get<picojson::object>()[ss.str()] = picojson::value(ss2.str());
            }
            out.get<picojson::object>()["Roles"] = object;
        }
        return out;
    }

    picojson::value toJSON(const UI::Style::Metrics& value)
    {
        picojson::value out(picojson::object_type, true);
        {
            picojson::value object(picojson::object_type, true);
            for (auto role : UI::Style::getMetricsRoleEnums())
            {
                std::stringstream ss;
                ss << role;
                object.get<picojson::object>()[ss.str()] = picojson::value(std::to_string(value.getMetric(role)));
            }
            out.get<picojson::object>()["Roles"] = object;
        }
        return out;
    }

    void fromJSON(const picojson::value& value, UI::Style::Palette& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Roles" == i.first)
                {
                    if (i.second.is<picojson::object>())
                    {
                        for (const auto& j : i.second.get<picojson::object>())
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
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

    void fromJSON(const picojson::value& value, UI::Style::Metrics& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Roles" == i.first)
                {
                    if (i.second.is<picojson::object>())
                    {
                        for (const auto& j : i.second.get<picojson::object>())
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

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::Style,
        ColorRole,
        DJV_TEXT("None"),
        DJV_TEXT("Background"),
        DJV_TEXT("Foreground"),
        DJV_TEXT("Border"),
        DJV_TEXT("Trough"),
        DJV_TEXT("Button"),
        DJV_TEXT("Hovered"),
        DJV_TEXT("Pressed"),
        DJV_TEXT("Checked"),
        DJV_TEXT("Disabled"),
        DJV_TEXT("TooltipBackground"),
        DJV_TEXT("TooltipForeground"),
        DJV_TEXT("Overlay"),
        DJV_TEXT("Shadow"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::Style,
        MetricsRole,
        DJV_TEXT("None"),
        DJV_TEXT("Border"),
        DJV_TEXT("Margin"),
        DJV_TEXT("MarginSmall"),
        DJV_TEXT("MarginLarge"),
        DJV_TEXT("MarginDialog"),
        DJV_TEXT("Spacing"),
        DJV_TEXT("SpacingSmall"),
        DJV_TEXT("SpacingLarge"),
        DJV_TEXT("Drag"),
        DJV_TEXT("Icon"),
        DJV_TEXT("IconSmall"),
        DJV_TEXT("FontSmall"),
        DJV_TEXT("FontMedium"),
        DJV_TEXT("FontLarge"),
        DJV_TEXT("FontHeader"),
        DJV_TEXT("Swatch"),
        DJV_TEXT("ThumbnailWidthSmall"),
        DJV_TEXT("ThumbnailHeightSmall"),
        DJV_TEXT("ThumbnailWidthLarge"),
        DJV_TEXT("ThumbnailHeightLarge"),
        DJV_TEXT("Shadow"),
        DJV_TEXT("ScrollArea"),
        DJV_TEXT("TextColumn"),
        DJV_TEXT("Dialog"),
        DJV_TEXT("TooltipOffset"),
        DJV_TEXT("Handle"));

} // namespace djv

