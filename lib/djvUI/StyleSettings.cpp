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

#include <djvUI/StyleSettings.h>

#include <djvUI/FontSettings.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
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
                std::shared_ptr<MapSubject<std::string, UI::Style::Metrics> > metrics;
                std::shared_ptr<ValueSubject<UI::Style::Metrics> > currentMetrics;
                std::shared_ptr<ValueSubject<std::string> > currentMetricsName;
                std::shared_ptr<ValueSubject<std::string> > currentFont;
                std::shared_ptr<ValueObserver<std::string> > currentLocaleObserver;
                std::shared_ptr<MapObserver<std::string, std::string> > localeFontsObserver;
            };

            void Style::_init(Context * context)
            {
                ISettings::_init("djv::UI::Settings::Style", context);

                std::map<std::string, UI::Style::Palette> palettes;
                palettes[DJV_TEXT("Dark")] = UI::Style::Palette();

                UI::Style::Palette palette;
                palette.setColor(ColorRole::Background, AV::Image::Color(250, 250, 250));
                palette.setColor(ColorRole::Foreground, AV::Image::Color(0, 0, 0));
                palette.setColor(ColorRole::Border, AV::Image::Color(200, 200, 200));
                palette.setColor(ColorRole::Trough, AV::Image::Color(230, 230, 230));
                palette.setColor(ColorRole::Button, AV::Image::Color(200, 200, 200));
                palette.setColor(ColorRole::Hovered, AV::Image::Color(0, 0, 0, 15));
                palette.setColor(ColorRole::Pressed, AV::Image::Color(0, 0, 0, 30));
                palette.setColor(ColorRole::Checked, AV::Image::Color(175, 209, 248));
                palette.setColor(ColorRole::Disabled, AV::Image::Color(150, 150, 150));
                palette.setColor(ColorRole::Overlay, AV::Image::Color(255, 255, 255, 160));
                palettes[DJV_TEXT("Light")] = palette;
                
                std::map<std::string, UI::Style::Metrics> metricsList;
                UI::Style::Metrics metrics;
                metricsList[DJV_TEXT("Medium")] = metrics;

                UI::Style::Metrics smallMetrics;
                UI::Style::Metrics largeMetrics;
                UI::Style::Metrics extraLargeMetrics;
                for (size_t i = 0; i < static_cast<size_t>(UI::MetricsRole::Count); ++i)
                {
                    const UI::MetricsRole m = static_cast<UI::MetricsRole>(i);
                    const float v = metrics.getMetric(m);
                    smallMetrics.setMetric(m, ceilf(v * .75f));
                    largeMetrics.setMetric(m, ceilf(v * 1.5f));
                    extraLargeMetrics.setMetric(m, ceilf(v * 2.f));
                }
                metricsList[DJV_TEXT("Small")] = smallMetrics;
                metricsList[DJV_TEXT("Large")] = largeMetrics;
                metricsList[DJV_TEXT("ExtraLarge")] = extraLargeMetrics;

                DJV_PRIVATE_PTR();
                p.palettes = MapSubject<std::string, UI::Style::Palette>::create(palettes);
                p.currentPalette = ValueSubject<UI::Style::Palette>::create(palettes["Dark"]);
                p.currentPaletteName = ValueSubject<std::string>::create("Dark");
                p.metrics = MapSubject<std::string, UI::Style::Metrics>::create(metricsList);
                p.currentMetrics = ValueSubject<UI::Style::Metrics>::create(metricsList["Medium"]);
                p.currentMetricsName = ValueSubject<std::string>::create(DJV_TEXT("Medium"));
                p.currentFont = ValueSubject<std::string>::create(DJV_TEXT("Default"));

                _load();

                auto weak = std::weak_ptr<Style>(std::dynamic_pointer_cast<Style>(shared_from_this()));
                if (auto textSystem = context->getSystemT<TextSystem>().lock())
                {
                    p.currentLocaleObserver = ValueObserver<std::string>::create(
                        textSystem->observeCurrentLocale(),
                        [weak](const std::string& value)
                    {
                        if (auto settings = weak.lock())
                        {
                            settings->_p->currentLocale = value;
                            settings->_updateCurrentFont();
                        }
                    });
                }

                if (auto settingsSystem = context->getSystemT<System>().lock())
                {
                    if (auto fontSettings = settingsSystem->getSettingsT<Settings::Font>())
                    {
                        p.localeFontsObserver = MapObserver<std::string, std::string>::create(
                            fontSettings->observeLocaleFonts(),
                            [weak](const std::map<std::string, std::string>& value)
                        {
                            if (auto style = weak.lock())
                            {
                                style->_p->localeFonts = value;
                                style->_updateCurrentFont();
                            }
                        });
                    }
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

            void Style::load(const picojson::value& value)
            {
                DJV_PRIVATE_PTR();
                if (value.is<picojson::object>())
                {
                    const auto& object = value.get<picojson::object>();
                    read("Palettes", object, p.palettes);
                    read("CurrentPalette", object, p.currentPaletteName);
                    p.currentPalette->setIfChanged(p.palettes->getItem(p.currentPaletteName->get()));
                    read("Metrics", object, p.metrics);
                    read("CurrentMetrics", object, p.currentMetricsName);
                    p.currentMetrics->setIfChanged(p.metrics->getItem(p.currentMetricsName->get()));
                }
            }

            picojson::value Style::save()
            {
                DJV_PRIVATE_PTR();
                picojson::value out(picojson::object_type, true);
                auto& object = out.get<picojson::object>();
                write("Palettes", p.palettes->get(), object);
                write("CurrentPalette", p.currentPaletteName->get(), object);
                write("Metrics", p.metrics->get(), object);
                write("CurrentMetrics", p.currentMetricsName->get(), object);
                return out;
            }

            void Style::_updateCurrentFont()
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

