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
                    { ColorRole::Background, AV::Image::Color(250, 250, 250) },
                    { ColorRole::Foreground, AV::Image::Color(0, 0, 0) },
                    { ColorRole::Border, AV::Image::Color(200, 200, 200) },
                    { ColorRole::Trough, AV::Image::Color(240, 240, 240) },
                    { ColorRole::Button, AV::Image::Color(200, 200, 200) },
                    { ColorRole::Hovered, AV::Image::Color(0, 0, 0, 15) },
                    { ColorRole::Pressed, AV::Image::Color(0, 0, 0, 30) },
                    { ColorRole::Checked, AV::Image::Color(225, 234, 248) },
                    { ColorRole::Disabled, AV::Image::Color(127, 127, 127) },
                    { ColorRole::HeaderBackground, AV::Image::Color(60, 104, 148) },
                    { ColorRole::HeaderForeground, AV::Image::Color(255, 255, 255) },
                    { ColorRole::HeaderHovered, AV::Image::Color(255, 255, 255, 25) },
                    { ColorRole::HeaderPressed, AV::Image::Color(255, 255, 255, 50) },
                    { ColorRole::HeaderChecked, AV::Image::Color(105, 150, 190) },
                    { ColorRole::HeaderDisabled, AV::Image::Color(127, 127, 127) },
                    { ColorRole::TooltipBackground, AV::Image::Color(255, 255, 191) },
                    { ColorRole::TooltipForeground, AV::Image::Color(0, 0, 0) },
                    { ColorRole::Overlay, AV::Image::Color(0, 0, 0, 127) },
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
                    { MetricsRole::Icon, 24.f },
                    { MetricsRole::IconLarge, 100.f },
                    { MetricsRole::FontSmall, 11.f },
                    { MetricsRole::FontMedium, 14.f },
                    { MetricsRole::FontLarge, 18.f },
                    { MetricsRole::FontHeader, 18.f },
                    { MetricsRole::Swatch, 40.f },
                    { MetricsRole::ThumbnailWidthSmall, 100.f },
                    { MetricsRole::ThumbnailHeightSmall, 50.f },
                    { MetricsRole::ThumbnailWidthLarge, 200.f },
                    { MetricsRole::ThumbnailHeightLarge, 100.f },
                    { MetricsRole::Shadow, 5.f },
                    { MetricsRole::ScrollArea, 200.f },
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

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::Style,
        ColorRole,
        DJV_TEXT("djv::UI::Style", "None"),
        DJV_TEXT("djv::UI::Style", "Background"),
        DJV_TEXT("djv::UI::Style", "Foreground"),
        DJV_TEXT("djv::UI::Style", "Border"),
        DJV_TEXT("djv::UI::Style", "Trough"),
        DJV_TEXT("djv::UI::Style", "Button"),
        DJV_TEXT("djv::UI::Style", "Hovered"),
        DJV_TEXT("djv::UI::Style", "Pressed"),
        DJV_TEXT("djv::UI::Style", "Checked"),
        DJV_TEXT("djv::UI::Style", "Disabled"),
        DJV_TEXT("djv::UI::Style", "HeaderBackground"),
        DJV_TEXT("djv::UI::Style", "HeaderForeground"),
        DJV_TEXT("djv::UI::Style", "HeaderHovered"),
        DJV_TEXT("djv::UI::Style", "HeaderPressed"),
        DJV_TEXT("djv::UI::Style", "HeaderChecked"),
        DJV_TEXT("djv::UI::Style", "HeaderDisabled"),
        DJV_TEXT("djv::UI::Style", "TooltipBackground"),
        DJV_TEXT("djv::UI::Style", "TooltipForeground"),
        DJV_TEXT("djv::UI::Style", "Overlay"),
        DJV_TEXT("djv::UI::Style", "Shadow"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::Style,
        MetricsRole,
        DJV_TEXT("djv::UI::Style", "None"),
        DJV_TEXT("djv::UI::Style", "Border"),
        DJV_TEXT("djv::UI::Style", "Margin"),
        DJV_TEXT("djv::UI::Style", "MarginSmall"),
        DJV_TEXT("djv::UI::Style", "MarginLarge"),
        DJV_TEXT("djv::UI::Style", "MarginDialog"),
        DJV_TEXT("djv::UI::Style", "Spacing"),
        DJV_TEXT("djv::UI::Style", "SpacingSmall"),
        DJV_TEXT("djv::UI::Style", "SpacingLarge"),
        DJV_TEXT("djv::UI::Style", "Drag"),
        DJV_TEXT("djv::UI::Style", "Icon"),
        DJV_TEXT("djv::UI::Style", "IconLarge"),
        DJV_TEXT("djv::UI::Style", "FontSmall"),
        DJV_TEXT("djv::UI::Style", "FontMedium"),
        DJV_TEXT("djv::UI::Style", "FontLarge"),
        DJV_TEXT("djv::UI::Style", "FontHeader"),
        DJV_TEXT("djv::UI::Style", "Swatch"),
        DJV_TEXT("djv::UI::Style", "ThumbnailWidthSmall"),
        DJV_TEXT("djv::UI::Style", "ThumbnailHeightSmall"),
        DJV_TEXT("djv::UI::Style", "ThumbnailWidthLarge"),
        DJV_TEXT("djv::UI::Style", "ThumbnailHeightLarge"),
        DJV_TEXT("djv::UI::Style", "Shadow"),
        DJV_TEXT("djv::UI::Style", "ScrollArea"),
        DJV_TEXT("djv::UI::Style", "TextColumn"),
        DJV_TEXT("djv::UI::Style", "Dialog"),
        DJV_TEXT("djv::UI::Style", "TooltipOffset"),
        DJV_TEXT("djv::UI::Style", "Handle"));

} // namespace djv

