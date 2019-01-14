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

#include <djvUI/FontSettings.h>
#include <djvUI/GeneralSettings.h>
#include <djvUI/StyleSettings.h>
#include <djvUI/UISystem.h>

#include <djvAV/AVSystem.h>

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
                p.colors[ColorRole::None]              = AV::Image::Color();
                p.colors[ColorRole::Background]        = AV::Image::Color( .2f,   .21f,  .23f);
                p.colors[ColorRole::BackgroundText]    = AV::Image::Color( .15f,  .16f,  .18f);
                p.colors[ColorRole::BackgroundHeader]  = AV::Image::Color(.25f,   .3f,   .33f);
                p.colors[ColorRole::BackgroundTooltip] = AV::Image::Color(1.f,   1.f,    .75f);
                p.colors[ColorRole::Foreground]        = AV::Image::Color(1.f,   1.f,   1.f);
                p.colors[ColorRole::ForegroundDim]     = AV::Image::Color( .7f,   .7f,   .7f);
                p.colors[ColorRole::ForegroundHeader]  = AV::Image::Color(1.f,   1.f,   1.f);
                p.colors[ColorRole::ForegroundTooltip] = AV::Image::Color(0.f,   0.f,   0.f);
                p.colors[ColorRole::Border]            = AV::Image::Color( .11f,  .14f,  .17f);
                p.colors[ColorRole::Trough]            = AV::Image::Color( .15f,  .15f,  .15f);
                p.colors[ColorRole::Button]            = AV::Image::Color( .27f,  .3f,   .33f);
                p.colors[ColorRole::Checked]           = AV::Image::Color( .2f,   .4f,   .7f);
                p.colors[ColorRole::CheckedForeground] = AV::Image::Color( .9f,   .9f,   .9f);
                p.colors[ColorRole::Hover]             = AV::Image::Color(1.f,   1.f,   1.f, .1f);
                p.colors[ColorRole::Disabled]          = AV::Image::Color( .5f,   .5f,   .5f);
                p.colors[ColorRole::Overlay]           = AV::Image::Color(0.f,   0.f,   0.f, .5f);
                p.colors[ColorRole::Shadow]            = AV::Image::Color(0.f,   0.f,   0.f, .2f);
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
                p.metrics[MetricsRole::None]           =   0.f;
                p.metrics[MetricsRole::Border]         =   1.f;
                p.metrics[MetricsRole::Margin]         =  10.f;
                p.metrics[MetricsRole::MarginSmall]    =   5.f;
                p.metrics[MetricsRole::MarginLarge]    =  20.f;
                p.metrics[MetricsRole::MarginDialog]   =  50.f;
                p.metrics[MetricsRole::Spacing]        =  10.f;
                p.metrics[MetricsRole::SpacingSmall]   =   5.f;
                p.metrics[MetricsRole::SpacingLarge]   =  20.f;
                p.metrics[MetricsRole::Drag]           =  20.f;
                p.metrics[MetricsRole::Icon]           =  24.f;
                p.metrics[MetricsRole::IconLarge]      = 100.f;
                p.metrics[MetricsRole::FontSmall]      =   6.f;
                p.metrics[MetricsRole::FontMedium]     =   9.f;
                p.metrics[MetricsRole::FontLarge]      =  14.f;
                p.metrics[MetricsRole::FontHeader]     =  18.f;
                p.metrics[MetricsRole::Swatch]         =  40.f;
                p.metrics[MetricsRole::ThumbnailSmall] = 100.f;
                p.metrics[MetricsRole::ThumbnailLarge] = 200.f;
                p.metrics[MetricsRole::Shadow]         =  10.f;
                p.metrics[MetricsRole::ScrollArea]     = 200.f;
                p.metrics[MetricsRole::TextColumn]     = 200.f;
                p.metrics[MetricsRole::Dialog]         = 400.f;
                p.metrics[MetricsRole::TooltipOffset]  =  10.f;
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

            void Style::_init(Context * context)
            {
                DJV_PRIVATE_PTR();
                p.context = context;
                if (auto avSystem = context->getSystemT<AV::AVSystem>().lock())
                {
                    p.dpi = avSystem->getDPI();
                }
                _p->dirty = true;
            }

            Style::Style() :
                _p(new Private)
            {}

            Style::~Style()
            {}

            std::shared_ptr<Style> Style::create(Context * context)
            {
                auto out = std::shared_ptr<Style>(new Style);
                out->_init(context);
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
                return _p->metrics.getMetric(role) * getScale();
            }

            void Style::setDPI(int value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.dpi)
                    return;
                p.dpi = value;
                p.dirty = true;
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
                return AV::Font::Info(family, face, p.metrics.getMetric(role), p.dpi);
            }

            AV::Font::Info Style::getFontInfo(const std::string & face, MetricsRole role) const
            {
                DJV_PRIVATE_PTR();
                return AV::Font::Info(p.font, face, p.metrics.getMetric(role), p.dpi);
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
        DJV_TEXT("djv::UI::Style", "BackgroundText"),
        DJV_TEXT("djv::UI::Style", "BackgroundHeader"),
        DJV_TEXT("djv::UI::Style", "BackgroundTooltip"),
        DJV_TEXT("djv::UI::Style", "Foreground"),
        DJV_TEXT("djv::UI::Style", "ForegroundDim"),
        DJV_TEXT("djv::UI::Style", "ForegroundHeader"),
        DJV_TEXT("djv::UI::Style", "ForegroundTooltip"),
        DJV_TEXT("djv::UI::Style", "Border"),
        DJV_TEXT("djv::UI::Style", "Trough"),
        DJV_TEXT("djv::UI::Style", "Button"),
        DJV_TEXT("djv::UI::Style", "Checked"),
        DJV_TEXT("djv::UI::Style", "CheckedForeground"),
        DJV_TEXT("djv::UI::Style", "Hover"),
        DJV_TEXT("djv::UI::Style", "Disabled"),
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
        DJV_TEXT("djv::UI::Style", "ThumbnailSmall"),
        DJV_TEXT("djv::UI::Style", "ThumbnailLarge"),
        DJV_TEXT("djv::UI::Style", "Shadow"),
        DJV_TEXT("djv::UI::Style", "ScrollArea"),
        DJV_TEXT("djv::UI::Style", "TextColumn"),
        DJV_TEXT("djv::UI::Style", "Dialog"),
        DJV_TEXT("djv::UI::Style", "TooltipOffset"));

} // namespace djv

