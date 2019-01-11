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
#include <djvCore/TextSystem.h>

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
                p.colors[ColorRole::BackgroundHeader]  = AV::Image::Color(.25f,   .30f,  .33f);
                p.colors[ColorRole::BackgroundText]    = AV::Image::Color( .15f,  .16f,  .18f);
                p.colors[ColorRole::BackgroundTooltip] = AV::Image::Color(1.f,   1.f,    .75f);
                p.colors[ColorRole::Foreground]        = AV::Image::Color( .9f,   .9f,   .9f);
                p.colors[ColorRole::ForegroundDim]     = AV::Image::Color( .7f,   .7f,   .7f);
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
                p.metrics[MetricsRole::None]                =   0.f;
                p.metrics[MetricsRole::Border]              =   1.f;
                p.metrics[MetricsRole::Margin]              =  10.f;
                p.metrics[MetricsRole::MarginSmall]         =   5.f;
                p.metrics[MetricsRole::MarginLarge]         =  20.f;
                p.metrics[MetricsRole::Spacing]             =  10.f;
                p.metrics[MetricsRole::SpacingSmall]        =   5.f;
                p.metrics[MetricsRole::SpacingLarge]        =  20.f;
                p.metrics[MetricsRole::Drag]                =  20.f;
                p.metrics[MetricsRole::Icon]                =  24.f;
                p.metrics[MetricsRole::IconLarge]           = 100.f;
                p.metrics[MetricsRole::FontSmall]           =   6.f;
                p.metrics[MetricsRole::FontMedium]          =   9.f;
                p.metrics[MetricsRole::FontLarge]           =  14.f;
                p.metrics[MetricsRole::FontExtraLarge]      =  24.f;
                p.metrics[MetricsRole::FontExtraExtraLarge] = 512.f;
                p.metrics[MetricsRole::Swatch]              =  40.f;
                p.metrics[MetricsRole::ThumbnailSmall]      = 100.f;
                p.metrics[MetricsRole::ThumbnailLarge]      = 200.f;
                p.metrics[MetricsRole::Shadow]              =  10.f;
                p.metrics[MetricsRole::ScrollArea]          = 200.f;
                p.metrics[MetricsRole::TextColumn]          = 200.f;
                p.metrics[MetricsRole::Dialog]              = 400.f;
                p.metrics[MetricsRole::TooltipOffset]       =  10.f;
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
                std::shared_ptr<Settings::Style> settings;
                Palette palette;
                int dpi = AV::dpiDefault;
                Metrics metrics;
                std::string currentLocale;
                std::map<std::string, Settings::FontMap> fonts;
                Settings::FontMap currentFont;
                std::shared_ptr<ValueSubject<bool> > styleChanged;
                std::shared_ptr<ValueObserver<Palette> > paletteObserver;
                std::shared_ptr<ValueObserver<int> > dpiObserver;
                std::shared_ptr<ValueObserver<Metrics> > metricsObserver;
                std::shared_ptr<ValueObserver<std::string> > currentLocaleObserver;
                std::shared_ptr<MapObserver<std::string, Settings::FontMap> > fontsObserver;
            };

            void Style::_init(Context * context)
            {
                ISystem::_init("djv::UI::Style::Style", context);

                DJV_PRIVATE_PTR();
                p.settings = Settings::Style::create(context);
                p.styleChanged = ValueSubject<bool>::create(false);

                auto weak = std::weak_ptr<Style>(std::dynamic_pointer_cast<Style>(shared_from_this()));
                p.paletteObserver = ValueObserver<Palette>::create(
                    p.settings->observeCurrentPalette(),
                    [weak](const Palette value)
                {
                    if (auto style = weak.lock())
                    {
                        style->_p->palette = value;
                        style->_p->styleChanged->setAlways(true);
                    }
                });

                p.dpiObserver = ValueObserver<int>::create(
                    p.settings->observeDPI(),
                    [weak](int value)
                {
                    if (auto style = weak.lock())
                    {
                        style->_p->dpi = value;
                        if (auto avSystem = style->getContext()->getSystemT<AV::AVSystem>().lock())
                        {
                            avSystem->setDPI(value);
                        }
                        style->_p->styleChanged->setAlways(true);
                    }
                });

                p.metricsObserver = ValueObserver<Metrics>::create(
                    p.settings->observeCurrentMetrics(),
                    [weak](const Metrics& value)
                {
                    if (auto style = weak.lock())
                    {
                        style->_p->metrics = value;
                        style->_p->styleChanged->setAlways(true);
                    }
                });

                if (auto uiSystem = context->getSystemT<UISystem>().lock())
                {
                    p.currentLocaleObserver = ValueObserver<std::string>::create(
                        uiSystem->getGeneralSettings()->observeCurrentLocale(),
                        [weak](const std::string& value)
                    {
                        if (auto style = weak.lock())
                        {
                            style->_p->currentLocale = value;
                            style->_updateCurrentFont();
                            style->_p->styleChanged->setAlways(true);
                        }
                    });

                    p.fontsObserver = MapObserver<std::string, Settings::FontMap>::create(
                        uiSystem->getFontSettings()->observeFonts(),
                        [weak](const std::map<std::string, Settings::FontMap>& value)
                    {
                        if (auto style = weak.lock())
                        {
                            style->_p->fonts = value;
                            style->_updateCurrentFont();
                            style->_p->styleChanged->setAlways(true);
                        }
                    });
                }
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

            const std::shared_ptr<Settings::Style>& Style::getSettings() const
            {
                return _p->settings;
            }

            const Palette& Style::getPalette() const
            {
                return _p->palette;
            }

            const AV::Image::Color& Style::getColor(ColorRole role) const
            {
                return _p->palette.getColor(role);
            }

            float Style::getMetric(MetricsRole role) const
            {
                return _p->metrics.getMetric(role) * getScale();
            }
            
            int Style::getDPI() const
            {
                return _p->dpi;
            }

            float Style::getScale() const
            {
                return _p->dpi / static_cast<float>(AV::dpiDefault);
            }

            AV::Font::Info Style::getFont(const std::string & face, MetricsRole metricsRole) const
            {
                AV::Font::Info out;
                const auto i = _p->currentFont.find(face);
                if (i != _p->currentFont.end())
                {
                    out = AV::Font::Info(i->second, i->first, getMetric(metricsRole));
                }
                return out;
            }

            std::shared_ptr<Core::IValueSubject<bool> > Style::observeStyleChanged() const
            {
                return _p->styleChanged;
            }

            void Style::_updateCurrentFont()
            {
                auto i = _p->fonts.find(_p->currentLocale);
                if (i != _p->fonts.end())
                {
                    _p->currentFont = i->second;
                }
                else
                {
                    i = _p->fonts.find("Default");
                    if (i != _p->fonts.end())
                    {
                        _p->currentFont = i->second;
                    }
                }
                for (auto i : _p->currentFont)
                {
                    std::cout << "!!!!!!!!!!!" << i.first << ": " << i.second << std::endl;
                }
            }

        } // namespace Style
    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::Style,
        ColorRole,
        DJV_TEXT("djv::UI::Style", "None"),
        DJV_TEXT("djv::UI::Style", "Background"),
        DJV_TEXT("djv::UI::Style", "BackgroundHeader"),
        DJV_TEXT("djv::UI::Style", "BackgroundText"),
        DJV_TEXT("djv::UI::Style", "BackgroundTooltip"),
        DJV_TEXT("djv::UI::Style", "Foreground"),
        DJV_TEXT("djv::UI::Style", "ForegroundDim"),
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
        DJV_TEXT("djv::UI::Style", "Spacing"),
        DJV_TEXT("djv::UI::Style", "SpacingSmall"),
        DJV_TEXT("djv::UI::Style", "SpacingLarge"),
        DJV_TEXT("djv::UI::Style", "Drag"),
        DJV_TEXT("djv::UI::Style", "Icon"),
        DJV_TEXT("djv::UI::Style", "IconLarge"),
        DJV_TEXT("djv::UI::Style", "FontSmall"),
        DJV_TEXT("djv::UI::Style", "FontMedium"),
        DJV_TEXT("djv::UI::Style", "FontLarge"),
        DJV_TEXT("djv::UI::Style", "FontExtraLarge"),
        DJV_TEXT("djv::UI::Style", "FontExtraExtraLarge"),
        DJV_TEXT("djv::UI::Style", "Swatch"),
        DJV_TEXT("djv::UI::Style", "ThumbnailSmall"),
        DJV_TEXT("djv::UI::Style", "ThumbnailLarge"),
        DJV_TEXT("djv::UI::Style", "Shadow"),
        DJV_TEXT("djv::UI::Style", "ScrollArea"),
        DJV_TEXT("djv::UI::Style", "TextColumn"),
        DJV_TEXT("djv::UI::Style", "Dialog"),
        DJV_TEXT("djv::UI::Style", "TooltipOffset"));

} // namespace djv

