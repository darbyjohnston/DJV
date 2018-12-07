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

#include <djvUICore/Style.h>

#include <djvUICore/StyleSettings.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>
#include <djvCore/TextSystem.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UICore
    {
        struct Palette::Private
        {
            std::map<ColorRole, AV::Color> colors;
        };

        Palette::Palette() :
            _p(new Private)
        {
            _p->colors[ColorRole::None]              = AV::Color();
            _p->colors[ColorRole::Background]        = AV::Color( .2f,  .21f, .23f);
            _p->colors[ColorRole::BackgroundHeader]  = AV::Color( .1f,  .1f,  .1f);
            _p->colors[ColorRole::BackgroundScroll]  = AV::Color( .15f, .16f, .18f);
            _p->colors[ColorRole::Foreground]        = AV::Color( .9f,  .9f,  .9f);
            _p->colors[ColorRole::ForegroundDim]     = AV::Color( .7f,  .7f,  .7f);
            _p->colors[ColorRole::Border]            = AV::Color( .16f, .19f, .22f);
            _p->colors[ColorRole::Trough]            = AV::Color( .1f,  .1f,  .1f);
            _p->colors[ColorRole::Button]            = AV::Color( .27f, .3f,  .33f);
            _p->colors[ColorRole::Checked]           = AV::Color( .2f,  .4f,  .7f);
            _p->colors[ColorRole::CheckedForeground] = AV::Color( .9f,  .9f,  .9f);
            _p->colors[ColorRole::Hover]             = AV::Color(1.f,  1.f,  1.f, .1f);
            _p->colors[ColorRole::Disabled]          = AV::Color( .3f,  .3f,  .3f);
            _p->colors[ColorRole::Overlay]           = AV::Color(0.f,  0.f,  0.f, .5f);
            _p->colors[ColorRole::Shadow]            = AV::Color(0.f,  0.f,  0.f, .2f);
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

        const AV::Color & Palette::getColor(ColorRole value) const
        {
            return _p->colors.at(value);
        }

        void Palette::setColor(ColorRole role, const AV::Color & value)
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
            _p->metrics[MetricsRole::None]                =   0.f;
            _p->metrics[MetricsRole::Border]              =   2.f;
            _p->metrics[MetricsRole::Margin]              =  10.f;
            _p->metrics[MetricsRole::MarginSmall]         =   5.f;
            _p->metrics[MetricsRole::MarginLarge]         =  40.f;
            _p->metrics[MetricsRole::Spacing]             =  10.f;
            _p->metrics[MetricsRole::SpacingSmall]        =   5.f;
            _p->metrics[MetricsRole::SpacingLarge]        =  40.f;
            _p->metrics[MetricsRole::Drag]                =  20.f;
            _p->metrics[MetricsRole::Icon]                =  25.f;
            _p->metrics[MetricsRole::IconLarge]           = 100.f;
            _p->metrics[MetricsRole::FontSmall]           =  16.f;
            _p->metrics[MetricsRole::FontMedium]          =  18.f;
            _p->metrics[MetricsRole::FontLarge]           =  28.f;
            _p->metrics[MetricsRole::FontExtraLarge]      =  72.f;
            _p->metrics[MetricsRole::FontExtraExtraLarge] = 512.f;
            _p->metrics[MetricsRole::Swatch]              =  40.f;
            _p->metrics[MetricsRole::Thumbnail]           = 300.f;
            _p->metrics[MetricsRole::Shadow]              =  20.f;
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
            return _p->metrics == other._p->metrics && _p->scale == other._p->scale;
        }

        struct Style::Private
        {
            std::shared_ptr<StyleSettings> settings;
            Palette palette;
            glm::vec2 dpi = glm::vec2(0.f, 0.f);
            Metrics metrics;
            std::string currentLocale;
            //std::map<std::string, FontMap> fonts;
            //FontMap currentFont;
            std::shared_ptr<ValueObserver<Palette> > paletteObserver;
            std::shared_ptr<ValueObserver<Metrics> > metricsObserver;
            std::shared_ptr<ValueObserver<std::string> > currentLocaleObserver;
            //std::shared_ptr<MapObserver<std::string, FontMap> > fontsObserver;
        };

        void Style::_init(const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::UICore::Style", context);

            _p->settings = StyleSettings::create(context);

            auto weak = std::weak_ptr<Style>(std::dynamic_pointer_cast<Style>(shared_from_this()));
            _p->paletteObserver = ValueObserver<Palette>::create(
                _p->settings->getCurrentPalette(),
                [weak](const Palette value)
            {
                if (auto style = weak.lock())
                {
                    style->_p->palette = value;
                }
            });

            _p->metricsObserver = ValueObserver<Metrics>::create(
                _p->settings->getCurrentMetrics(),
                [weak](const Metrics& value)
            {
                if (auto style = weak.lock())
                {
                    style->_p->metrics = value;
                }
            });

            /*auto uiSystem = context->getSystemT<System>();
            _p->currentLocaleObserver = ValueObserver<std::string>::create(
                uiSystem->getGeneralSettings()->getCurrentLocale(),
                [weak](const std::string& value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->currentLocale = value;
                    system->_updateCurrentFont();
                }
            });

            _p->fontsObserver = MapObserver<std::string, FontMap>::create(
                uiSystem->getFontSettings()->getFonts(),
                [weak](const std::map<std::string, FontMap>& value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->fonts = value;
                    system->_updateCurrentFont();
                }
            });*/
        }

        Style::Style() :
            _p(new Private)
        {}

        Style::~Style()
        {}

        std::shared_ptr<Style> Style::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<Style>(new Style);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<StyleSettings>& Style::getSettings() const
        {
            return _p->settings;
        }

        const Palette& Style::getPalette() const
        {
            return _p->palette;
        }

        const AV::Color& Style::getColor(ColorRole role) const
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

        /*Graphics::Font Style::getFont(FontFace fontFace, MetricsRole metricsRole) const
        {
            Graphics::Font out;
            const auto i = _p->currentFont.find(fontFace);
            if (i != _p->currentFont.end())
            {
                out.name = i->second;
                out.size = getMetric(metricsRole);
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
                i = _p->fonts.find("default");
                if (i != _p->fonts.end())
                {
                    _p->currentFont = i->second;
                }
            }
        }*/

    } // namespace UICore

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UICore,
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
        UICore,
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
        DJV_TEXT("Shadow"));

} // namespace djv

