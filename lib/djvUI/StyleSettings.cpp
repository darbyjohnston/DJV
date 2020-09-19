// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/StyleSettings.h>

#include <djvUI/FontSettings.h>
#include <djvUI/SettingsSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

#if defined(GetObject)
#undef GetObject
#endif // GetObject

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct Style::Private
            {
                std::string currentLocale;
                std::map<std::string, std::string> localeFonts;
                std::shared_ptr<MapSubject<std::string, UI::Style::Palette> > palettes;
                std::shared_ptr<ValueSubject<UI::Style::Palette> > currentPalette;
                std::shared_ptr<ValueSubject<std::string> > currentPaletteName;
                std::shared_ptr<ValueSubject<float> > brightness;
                std::shared_ptr<ValueSubject<float> > contrast;
                std::shared_ptr<MapSubject<std::string, UI::Style::Metrics> > metrics;
                std::shared_ptr<ValueSubject<UI::Style::Metrics> > currentMetrics;
                std::shared_ptr<ValueSubject<std::string> > currentMetricsName;
                std::shared_ptr<ValueSubject<std::string> > currentFont;
                std::shared_ptr<ValueObserver<std::string> > currentLocaleObserver;
                std::shared_ptr<MapObserver<std::string, std::string> > localeFontsObserver;
            };

            void Style::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::Style", context);

                std::map<std::string, UI::Style::Palette> palettes;
                palettes[DJV_TEXT("style_palette_dark")] = UI::Style::Palette();

                UI::Style::Palette palette;
                palette.setColor(ColorRole::Background, Image::Color(.92F, .92F, .92F, 1.F));
                palette.setColor(ColorRole::BackgroundBellows, Image::Color(.85F, .85F, .85F, 1.F));
                palette.setColor(ColorRole::BackgroundHeader, Image::Color(.8F, .8F, .8F, 1.F));
                palette.setColor(ColorRole::BackgroundToolBar, Image::Color(.88F, .88F, .88F, 1.F));
                palette.setColor(ColorRole::Border, Image::Color(.65F, .65F, .65F, 1.F));
                palette.setColor(ColorRole::BorderButton, Image::Color(.85F, .85F, .85F, 1.F));
                palette.setColor(ColorRole::Button, Image::Color(1.F, 1.F, 1.F, 1.F));
                palette.setColor(ColorRole::Checked, Image::Color(.69F, .76F, .89F, 1.F));
                palette.setColor(ColorRole::TextFocus, Image::Color(.49F, .66F, .89F, 1.F));
                palette.setColor(ColorRole::Foreground, Image::Color(0.F, 0.F, 0.F, 1.F));
                palette.setColor(ColorRole::ForegroundDim, Image::Color(.5F, .5F, .5F, 1.F));
                palette.setColor(ColorRole::Handle, Image::Color(.49F, .66F, .89F, 1.F));
                palette.setColor(ColorRole::Hovered, Image::Color(0.F, 0.F, 0.F, .06F));
                palette.setColor(ColorRole::Overlay, Image::Color(0.F, 0.F, 0.F, .7F));
                palette.setColor(ColorRole::OverlayLight, Image::Color(1.F, 1.F, 1.F, .7F));
                palette.setColor(ColorRole::Pressed, Image::Color(0.F, 0.F, 0.F, .1F));
                palette.setColor(ColorRole::Shadow, Image::Color(0.F, 0.F, 0.F, .12F));
                palette.setColor(ColorRole::Trough, Image::Color(.95F, .95F, .95F, 1.F));
                palette.setColor(ColorRole::Cached, Image::Color(.69F, .89F, .76F, 1.F));
                palette.setColor(ColorRole::Warning, Image::Color(.8F, .8F, .2F, 1.F));
                palette.setColor(ColorRole::Error, Image::Color(.8F, .2F, .2F, 1.F));
                palette.setDisabledMult(.5F);
                palettes[DJV_TEXT("style_palette_light")] = palette;
                
                std::map<std::string, UI::Style::Metrics> metricsList;
                UI::Style::Metrics metrics;
                metricsList[DJV_TEXT("style_metrics_medium")] = metrics;

                UI::Style::Metrics smallMetrics;
                UI::Style::Metrics largeMetrics;
                UI::Style::Metrics extraLargeMetrics;
                for (size_t i = 0; i < static_cast<size_t>(UI::MetricsRole::Count); ++i)
                {
                    const UI::MetricsRole m = static_cast<UI::MetricsRole>(i);
                    const float v = metrics.getMetric(m);
                    smallMetrics.setMetric(m, ceilf(v * .75F));
                    largeMetrics.setMetric(m, ceilf(v * 1.5F));
                    extraLargeMetrics.setMetric(m, ceilf(v * 2.F));
                }
                metricsList[DJV_TEXT("style_metrics_small")] = smallMetrics;
                metricsList[DJV_TEXT("style_metrics_large")] = largeMetrics;
                metricsList[DJV_TEXT("style_metrics_extra_large")] = extraLargeMetrics;

                DJV_PRIVATE_PTR();
                p.palettes = MapSubject<std::string, UI::Style::Palette>::create(palettes);
                p.currentPalette = ValueSubject<UI::Style::Palette>::create(palettes["style_palette_dark"]);
                p.currentPaletteName = ValueSubject<std::string>::create("style_palette_dark");
                p.brightness = ValueSubject<float>::create(1.F);
                p.contrast = ValueSubject<float>::create(1.F);
                p.metrics = MapSubject<std::string, UI::Style::Metrics>::create(metricsList);
                p.currentMetrics = ValueSubject<UI::Style::Metrics>::create(metricsList["style_metrics_medium"]);
                p.currentMetricsName = ValueSubject<std::string>::create(("style_metrics_medium"));
                p.currentFont = ValueSubject<std::string>::create(DJV_TEXT("style_font_default"));

                _load();

                auto weak = std::weak_ptr<Style>(std::dynamic_pointer_cast<Style>(shared_from_this()));
                if (auto textSystem = context->getSystemT<System::TextSystem>())
                {
                    p.currentLocaleObserver = ValueObserver<std::string>::create(
                        textSystem->observeCurrentLocale(),
                        [weak](const std::string& value)
                    {
                        if (auto settings = weak.lock())
                        {
                            settings->_p->currentLocale = value;
                            settings->_currentFontUpdate();
                        }
                    });
                }

                auto settingsSystem = context->getSystemT<Settings::SettingsSystem>();
                auto fontSettings = settingsSystem->getSettingsT<Settings::Font>();
                p.localeFontsObserver = MapObserver<std::string, std::string>::create(
                    fontSettings->observeLocaleFonts(),
                    [weak](const std::map<std::string, std::string>& value)
                {
                    if (auto style = weak.lock())
                    {
                        style->_p->localeFonts = value;
                        style->_currentFontUpdate();
                    }
                });
            }

            Style::Style() :
                _p(new Private)
            {}

            Style::~Style()
            {}

            std::shared_ptr<Style> Style::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Style>(new Style);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IMapSubject<std::string, UI::Style::Palette> > Style::observePalettes() const
            {
                return _p->palettes;
            }

            std::shared_ptr<IValueSubject<UI::Style::Palette> > Style::observeCurrentPalette() const
            {
                return _p->currentPalette;
            }

            std::shared_ptr<IValueSubject<std::string> > Style::observeCurrentPaletteName() const
            {
                return _p->currentPaletteName;
            }

            std::shared_ptr<IValueSubject<float> > Style::observeBrightness() const
            {
                return _p->brightness;
            }

            std::shared_ptr<IValueSubject<float> > Style::observeContrast() const
            {
                return _p->contrast;
            }

            void Style::setCurrentPalette(const std::string& name)
            {
                DJV_PRIVATE_PTR();
                if (p.palettes->hasKey(name))
                {
                    const auto palette = p.palettes->getItem(name);
                    p.currentPalette->setIfChanged(palette);
                    p.currentPaletteName->setIfChanged(name);
                }
            }

            void Style::setBrightness(float value)
            {
                DJV_PRIVATE_PTR();
                p.brightness->setIfChanged(value);
            }

            void Style::setContrast(float value)
            {
                DJV_PRIVATE_PTR();
                p.contrast->setIfChanged(value);
            }

            std::shared_ptr<IMapSubject<std::string, UI::Style::Metrics> > Style::observeMetrics() const
            {
                return _p->metrics;
            }

            std::shared_ptr<IValueSubject<UI::Style::Metrics> > Style::observeCurrentMetrics() const
            {
                return _p->currentMetrics;
            }

            std::shared_ptr<IValueSubject<std::string> > Style::observeCurrentMetricsName() const
            {
                return _p->currentMetricsName;
            }

            std::shared_ptr<IValueSubject<std::string> > Style::observeCurrentFont() const
            {
                return _p->currentFont;
            }

            void Style::setCurrentMetrics(const std::string& name)
            {
                DJV_PRIVATE_PTR();
                if (p.metrics->hasKey(name))
                {
                    const auto metrics = p.metrics->getItem(name);
                    p.currentMetrics->setIfChanged(metrics);
                    p.currentMetricsName->setIfChanged(name);
                }
            }

            void Style::load(const rapidjson::Value& value)
            {
                DJV_PRIVATE_PTR();
                if (value.IsObject())
                {
                    read("Palettes", value, p.palettes);
                    read("CurrentPalette", value, p.currentPaletteName);
                    p.currentPalette->setIfChanged(p.palettes->getItem(p.currentPaletteName->get()));
                    read("Brightness", value, p.brightness);
                    read("Contrast", value, p.contrast);
                    read("Metrics", value, p.metrics);
                    read("CurrentMetrics", value, p.currentMetricsName);
                    p.currentMetrics->setIfChanged(p.metrics->getItem(p.currentMetricsName->get()));
                }
            }

            rapidjson::Value Style::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                write("Palettes", p.palettes->get(), out, allocator);
                write("CurrentPalette", p.currentPaletteName->get(), out, allocator);
                write("Brightness", p.brightness->get(), out, allocator);
                write("Contrast", p.contrast->get(), out, allocator);
                write("Metrics", p.metrics->get(), out, allocator);
                write("CurrentMetrics", p.currentMetricsName->get(), out, allocator);
                return out;
            }

            void Style::_currentFontUpdate()
            {
                DJV_PRIVATE_PTR();
                std::string font;
                auto i = p.localeFonts.find(p.currentLocale);
                if (i != p.localeFonts.end())
                {
                    font = i->second;
                }
                else
                {
                    i = p.localeFonts.find("Default");
                    if (i != p.localeFonts.end())
                    {
                        font = i->second;
                    }
                }
                p.currentFont->setIfChanged(font);
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

