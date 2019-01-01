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
                p.colors[ColorRole::None] = AV::Image::Color();
                p.colors[ColorRole::Background] = AV::Image::Color(.2f, .21f, .23f);
                p.colors[ColorRole::BackgroundHeader] = AV::Image::Color(.1f, .1f, .1f);
                p.colors[ColorRole::BackgroundScroll] = AV::Image::Color(.15f, .16f, .18f);
                p.colors[ColorRole::Foreground] = AV::Image::Color(.9f, .9f, .9f);
                p.colors[ColorRole::ForegroundDim] = AV::Image::Color(.7f, .7f, .7f);
                p.colors[ColorRole::Border] = AV::Image::Color(.11f, .14f, .17f);
                p.colors[ColorRole::Trough] = AV::Image::Color(.15f, .15f, .15f);
                p.colors[ColorRole::Button] = AV::Image::Color(.27f, .3f, .33f);
                p.colors[ColorRole::Checked] = AV::Image::Color(.2f, .4f, .7f);
                p.colors[ColorRole::CheckedForeground] = AV::Image::Color(.9f, .9f, .9f);
                p.colors[ColorRole::Hover] = AV::Image::Color(1.f, 1.f, 1.f, .1f);
                p.colors[ColorRole::Disabled] = AV::Image::Color(.3f, .3f, .3f);
                p.colors[ColorRole::Overlay] = AV::Image::Color(0.f, 0.f, 0.f, .5f);
                p.colors[ColorRole::Shadow] = AV::Image::Color(0.f, 0.f, 0.f, .2f);
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
                float scale = 1.f;
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
                p.metrics[MetricsRole::None] = 0.f;
                p.metrics[MetricsRole::Border] = 1.f;
                p.metrics[MetricsRole::Margin] = 10.f;
                p.metrics[MetricsRole::MarginSmall] = 5.f;
                p.metrics[MetricsRole::MarginLarge] = 40.f;
                p.metrics[MetricsRole::Spacing] = 10.f;
                p.metrics[MetricsRole::SpacingSmall] = 5.f;
                p.metrics[MetricsRole::SpacingLarge] = 40.f;
                p.metrics[MetricsRole::Drag] = 20.f;
                p.metrics[MetricsRole::Icon] = 25.f;
                p.metrics[MetricsRole::IconLarge] = 100.f;
                p.metrics[MetricsRole::FontSmall] = 9.f;
                p.metrics[MetricsRole::FontMedium] = 12.f;
                p.metrics[MetricsRole::FontLarge] = 24.f;
                p.metrics[MetricsRole::FontExtraLarge] = 48.f;
                p.metrics[MetricsRole::FontExtraExtraLarge] = 512.f;
                p.metrics[MetricsRole::Swatch] = 40.f;
                p.metrics[MetricsRole::Thumbnail] = 300.f;
                p.metrics[MetricsRole::Shadow] = 10.f;
                p.metrics[MetricsRole::ScrollArea] = 200.f;
                p.metrics[MetricsRole::TextColumn] = 200.f;
                p.metrics[MetricsRole::Dialog] = 400.f;
            }

            float Metrics::getMetric(MetricsRole role) const
            {
                return _p->metrics.at(role);
            }

            void Metrics::setMetric(MetricsRole role, float value)
            {
                _p->metrics[role] = value;
            }

            float Metrics::getScale() const
            {
                return _p->scale;
            }

            void Metrics::setScale(float value)
            {
                _p->scale = value;
            }

            bool Metrics::operator == (const Metrics& other) const
            {
                DJV_PRIVATE_PTR();
                return p.metrics == other._p->metrics && p.scale == other._p->scale;
            }

            struct Style::Private
            {
                std::shared_ptr<Settings::Style> settings;
                Palette palette;
                glm::vec2 dpi = glm::vec2(0.f, 0.f);
                Metrics metrics;
                std::string currentLocale;
                std::map<std::string, Settings::FontMap> fonts;
                Settings::FontMap currentFont;
                std::shared_ptr<ValueObserver<Palette> > paletteObserver;
                std::shared_ptr<ValueObserver<Metrics> > metricsObserver;
                std::shared_ptr<ValueObserver<std::string> > currentLocaleObserver;
                std::shared_ptr<MapObserver<std::string, Settings::FontMap> > fontsObserver;
            };

            void Style::_init(Context * context)
            {
                ISystem::_init("djv::UI::Style::Style", context);

                DJV_PRIVATE_PTR();
                p.settings = Settings::Style::create(context);

                auto weak = std::weak_ptr<Style>(std::dynamic_pointer_cast<Style>(shared_from_this()));
                p.paletteObserver = ValueObserver<Palette>::create(
                    p.settings->getCurrentPalette(),
                    [weak](const Palette value)
                {
                    if (auto style = weak.lock())
                    {
                        style->_p->palette = value;
                    }
                });

                p.metricsObserver = ValueObserver<Metrics>::create(
                    p.settings->getCurrentMetrics(),
                    [weak](const Metrics& value)
                {
                    if (auto style = weak.lock())
                    {
                        style->_p->metrics = value;
                    }
                });

                if (auto uiSystem = context->getSystemT<UISystem>().lock())
                {
                    p.currentLocaleObserver = ValueObserver<std::string>::create(
                        uiSystem->getGeneralSettings()->getCurrentLocale(),
                        [weak](const std::string& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->currentLocale = value;
                            system->_updateCurrentFont();
                        }
                    });

                    p.fontsObserver = MapObserver<std::string, Settings::FontMap>::create(
                        uiSystem->getFontSettings()->getFonts(),
                        [weak](const std::map<std::string, Settings::FontMap>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->fonts = value;
                            system->_updateCurrentFont();
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

            const Palette& Style::getPalette() const
            {
                return _p->palette;
            }

            const AV::Image::Color& Style::getColor(ColorRole role) const
            {
                return _p->palette.getColor(role);
            }

            void Style::setPalette(const Palette& value)
            {
                _p->palette = value;
            }

            float Style::getMetric(MetricsRole role) const
            {
                return _p->metrics.getMetric(role) * _p->metrics.getScale();
            }

            void Style::setMetrics(const Metrics& value)
            {
                _p->metrics = value;
            }

            float Style::getScale() const
            {
                return _p->metrics.getScale();
            }

            void Style::setScale(float value)
            {
                _p->metrics.setScale(value);
            }

            const glm::vec2& Style::getDPI() const
            {
                return _p->dpi;
            }

            void Style::setDPI(const glm::vec2& value)
            {
                _p->dpi = value;
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
            }

        } // namespace Style
    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::Style,
        ColorRole,
        DJV_TEXT("None"),
        DJV_TEXT("Background"),
        DJV_TEXT("BackgroundHeader"),
        DJV_TEXT("BackgroundScroll"),
        DJV_TEXT("Foreground"),
        DJV_TEXT("DimForeground"),
        DJV_TEXT("Border"),
        DJV_TEXT("Trough"),
        DJV_TEXT("Button"),
        DJV_TEXT("Checked"),
        DJV_TEXT("CheckedForeground"),
        DJV_TEXT("Hover"),
        DJV_TEXT("Disabled"),
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
        DJV_TEXT("Spacing"),
        DJV_TEXT("SpacingSmall"),
        DJV_TEXT("SpacingLarge"),
        DJV_TEXT("Drag"),
        DJV_TEXT("Icon"),
        DJV_TEXT("IconLarge"),
        DJV_TEXT("FontSmall"),
        DJV_TEXT("FontMedium"),
        DJV_TEXT("FontLarge"),
        DJV_TEXT("FontExtraLarge"),
        DJV_TEXT("FontExtraExtraLarge"),
        DJV_TEXT("Swatch"),
        DJV_TEXT("Thumbnail"),
        DJV_TEXT("Shadow"),
        DJV_TEXT("ScrollArea"),
        DJV_TEXT("TextColumn"),
        DJV_TEXT("Dialog"));

} // namespace djv

