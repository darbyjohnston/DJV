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

#include <djvUIComponents/GeneralSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FontSettings.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StyleSettings.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class DisplaySizeWidget : public Widget
            {
                DJV_NON_COPYABLE(DisplaySizeWidget);

            protected:
                void _init(Context *);
                DisplaySizeWidget();

            public:
                static std::shared_ptr<DisplaySizeWidget> create(Context *);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

                void _localeEvent(Event::Locale &) override;

            private:
                void _widgetUpdate();
                void _currentItemUpdate();

                DJV_PRIVATE();
            };

            struct DisplaySizeWidget::Private
            {
                std::vector<std::string> metrics;
                std::string currentMetrics;
                std::shared_ptr<ComboBox> comboBox;
                std::map<size_t, std::string> indexToMetrics;
                std::map<std::string, size_t> metricsToIndex;
                std::shared_ptr<MapObserver<std::string, Style::Metrics> > metricsObserver;
                std::shared_ptr<ValueObserver<std::string> > currentMetricsObserver;
            };

            void DisplaySizeWidget::_init(Context * context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                p.comboBox = ComboBox::create(context);
                addChild(p.comboBox);

                auto weak = std::weak_ptr<DisplaySizeWidget>(std::dynamic_pointer_cast<DisplaySizeWidget>(shared_from_this()));
                p.comboBox->setCallback(
                    [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto settingsSystem = context->getSystemT<Settings::System>())
                        {
                            if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                            {
                                const auto i = widget->_p->indexToMetrics.find(value);
                                if (i != widget->_p->indexToMetrics.end())
                                {
                                    styleSettings->setCurrentMetrics(i->second);
                                }
                            }
                        }
                    }
                });

                if (auto settingsSystem = context->getSystemT<Settings::System>())
                {
                    if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                    {
                        p.metricsObserver = MapObserver<std::string, Style::Metrics>::create(
                            styleSettings->observeMetrics(),
                            [weak, context](const std::map<std::string, Style::Metrics> & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->metrics.clear();
                                for (const auto & i : value)
                                {
                                    widget->_p->metrics.push_back(i.first);
                                }
                                widget->_widgetUpdate();
                            }
                        });
                        p.currentMetricsObserver = ValueObserver<std::string>::create(
                            styleSettings->observeCurrentMetricsName(),
                            [weak](const std::string & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->currentMetrics = value;
                                widget->_currentItemUpdate();
                            }
                        });
                    }
                }
            }

            DisplaySizeWidget::DisplaySizeWidget() :
                _p(new Private)
            {}

            std::shared_ptr<DisplaySizeWidget> DisplaySizeWidget::create(Context * context)
            {
                auto out = std::shared_ptr<DisplaySizeWidget>(new DisplaySizeWidget);
                out->_init(context);
                return out;
            }

            float DisplaySizeWidget::getHeightForWidth(float value) const
            {
                return _p->comboBox->getHeightForWidth(value);
            }

            void DisplaySizeWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_p->comboBox->getMinimumSize());
            }

            void DisplaySizeWidget::_layoutEvent(Event::Layout &)
            {
                _p->comboBox->setGeometry(getGeometry());
            }

            void DisplaySizeWidget::_localeEvent(Event::Locale & event)
            {
                _widgetUpdate();
            }

            void DisplaySizeWidget::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                p.comboBox->clearItems();
                p.indexToMetrics.clear();
                p.metricsToIndex.clear();
                for (size_t i = 0; i < p.metrics.size(); ++i)
                {
                    const auto & name = p.metrics[i];
                    p.comboBox->addItem(_getText(name));
                    p.indexToMetrics[i] = name;
                    p.metricsToIndex[name] = i;
                }
                _currentItemUpdate();
            }

            void DisplaySizeWidget::_currentItemUpdate()
            {
                DJV_PRIVATE_PTR();
                const auto i = p.metricsToIndex.find(p.currentMetrics);
                if (i != p.metricsToIndex.end())
                {
                    p.comboBox->setCurrentItem(static_cast<int>(i->second));
                }
            }

            class PaletteWidget : public Widget
            {
                DJV_NON_COPYABLE(PaletteWidget);

            protected:
                void _init(Context *);
                PaletteWidget();

            public:
                static std::shared_ptr<PaletteWidget> create(Context *);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

                void _localeEvent(Event::Locale &) override;

            private:
                void _widgetUpdate();
                void _currentItemUpdate();

                DJV_PRIVATE();
            };

            struct PaletteWidget::Private
            {
                std::vector<std::string> palettes;
                std::string currentPalette;
                std::shared_ptr<ComboBox> comboBox;
                std::map<size_t, std::string> indexToPalette;
                std::map<std::string, size_t> paletteToIndex;
                std::shared_ptr<MapObserver<std::string, Style::Palette> > palettesObserver;
                std::shared_ptr<ValueObserver<std::string> > currentPaletteObserver;
            };

            void PaletteWidget::_init(Context * context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                p.comboBox = ComboBox::create(context);
                addChild(p.comboBox);

                auto weak = std::weak_ptr<PaletteWidget>(std::dynamic_pointer_cast<PaletteWidget>(shared_from_this()));
                p.comboBox->setCallback(
                    [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto settingsSystem = context->getSystemT<Settings::System>())
                        {
                            if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                            {
                                const auto i = widget->_p->indexToPalette.find(value);
                                if (i != widget->_p->indexToPalette.end())
                                {
                                    styleSettings->setCurrentPalette(i->second);
                                }
                            }
                        }
                    }
                });

                if (auto settingsSystem = context->getSystemT<Settings::System>())
                {
                    if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                    {
                        p.palettesObserver = MapObserver<std::string, Style::Palette>::create(
                            styleSettings->observePalettes(),
                            [weak](const std::map<std::string, Style::Palette > & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->palettes.clear();
                                for (const auto & i : value)
                                {
                                    widget->_p->palettes.push_back(i.first);
                                }
                                widget->_widgetUpdate();
                            }
                        });
                        p.currentPaletteObserver = ValueObserver<std::string>::create(
                            styleSettings->observeCurrentPaletteName(),
                            [weak](const std::string & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->currentPalette = value;
                                widget->_currentItemUpdate();
                            }
                        });
                    }
                }
            }

            PaletteWidget::PaletteWidget() :
                _p(new Private)
            {}

            std::shared_ptr<PaletteWidget> PaletteWidget::create(Context * context)
            {
                auto out = std::shared_ptr<PaletteWidget>(new PaletteWidget);
                out->_init(context);
                return out;
            }

            float PaletteWidget::getHeightForWidth(float value) const
            {
                return _p->comboBox->getHeightForWidth(value);
            }

            void PaletteWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_p->comboBox->getMinimumSize());
            }

            void PaletteWidget::_layoutEvent(Event::Layout &)
            {
                _p->comboBox->setGeometry(getGeometry());
            }

            void PaletteWidget::_localeEvent(Event::Locale & event)
            {
                _widgetUpdate();
            }

            void PaletteWidget::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                p.comboBox->clearItems();
                p.indexToPalette.clear();
                p.paletteToIndex.clear();
                for (size_t i = 0; i < p.palettes.size(); ++i)
                {
                    const auto & name = p.palettes[i];
                    p.comboBox->addItem(_getText(name));
                    p.indexToPalette[i] = name;
                    p.paletteToIndex[name] = i;
                }
                _currentItemUpdate();
            }

            void PaletteWidget::_currentItemUpdate()
            {
                DJV_PRIVATE_PTR();
                const auto i = p.paletteToIndex.find(p.currentPalette);
                if (i != p.paletteToIndex.end())
                {
                    p.comboBox->setCurrentItem(static_cast<int>(i->second));
                }
            }

            class LanguageWidget : public Widget
            {
                DJV_NON_COPYABLE(LanguageWidget);

            protected:
                void _init(Context *);
                LanguageWidget();

            public:
                static std::shared_ptr<LanguageWidget> create(Context *);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

                void _localeEvent(Event::Locale &) override;

            private:
                void _widgetUpdate();
                void _currentItemUpdate();

                DJV_PRIVATE();
            };

            struct LanguageWidget::Private
            {
                std::string locale;
                std::shared_ptr<ComboBox> comboBox;
                std::map<size_t, std::string> indexToLocale;
                std::map<std::string, size_t> localeToIndex;
                std::map<std::string, std::string> localeFonts;
                std::shared_ptr<ValueObserver<std::string> > localeObserver;
                std::shared_ptr<MapObserver<std::string, std::string> > localeFontsObserver;
            };

            void LanguageWidget::_init(Context * context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                p.comboBox = ComboBox::create(context);
                addChild(p.comboBox);

                auto weak = std::weak_ptr<LanguageWidget>(std::dynamic_pointer_cast<LanguageWidget>(shared_from_this()));
                p.comboBox->setCallback(
                    [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto textSystem = context->getSystemT<TextSystem>())
                        {
                            const auto i = widget->_p->indexToLocale.find(value);
                            if (i != widget->_p->indexToLocale.end())
                            {
                                textSystem->setCurrentLocale(i->second);
                            }
                        }
                    }
                });

                if (auto textSystem = context->getSystemT<TextSystem>())
                {
                    p.localeObserver = ValueObserver<std::string>::create(
                        textSystem->observeCurrentLocale(),
                        [weak](const std::string & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->locale = value;
                            widget->_currentItemUpdate();
                        }
                    });
                }

                if (auto settingsSystem = context->getSystemT<Settings::System>())
                {
                    if (auto fontSettings = settingsSystem->getSettingsT<Settings::Font>())
                    {
                        p.localeFontsObserver = MapObserver<std::string, std::string>::create(
                            fontSettings->observeLocaleFonts(),
                            [weak](const std::map<std::string, std::string> & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->localeFonts = value;
                                widget->_widgetUpdate();
                            }
                        });
                    }
                }
            }

            LanguageWidget::LanguageWidget() :
                _p(new Private)
            {}

            std::shared_ptr<LanguageWidget> LanguageWidget::create(Context * context)
            {
                auto out = std::shared_ptr<LanguageWidget>(new LanguageWidget);
                out->_init(context);
                return out;
            }

            float LanguageWidget::getHeightForWidth(float value) const
            {
                return _p->comboBox->getHeightForWidth(value);
            }

            void LanguageWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_p->comboBox->getMinimumSize());
            }

            void LanguageWidget::_layoutEvent(Event::Layout &)
            {
                _p->comboBox->setGeometry(getGeometry());
            }

            void LanguageWidget::_localeEvent(Event::Locale & event)
            {
                _widgetUpdate();
            }

            void LanguageWidget::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                auto context = getContext();
                if (auto textSystem = context->getSystemT<TextSystem>())
                {
                    p.comboBox->clearItems();
                    const auto & locales = textSystem->getLocales();
                    size_t j = 0;
                    for (const auto & i : locales)
                    {
                        std::string font;
                        auto k = _p->localeFonts.find(i);
                        if (k != _p->localeFonts.end())
                        {
                            font = k->second;
                        }
                        else
                        {
                            k = p.localeFonts.find("Default");
                            if (k != p.localeFonts.end())
                            {
                                font = k->second;
                            }
                        }
                        p.comboBox->addItem(_getText(i));
                        p.comboBox->setFont(static_cast<int>(j), font);
                        p.indexToLocale[j] = i;
                        p.localeToIndex[i] = j;
                        ++j;
                    }
                    _currentItemUpdate();
                }
            }

            void LanguageWidget::_currentItemUpdate()
            {
                DJV_PRIVATE_PTR();
                const auto i = p.localeToIndex.find(p.locale);
                if (i != p.localeToIndex.end())
                {
                    p.comboBox->setCurrentItem(static_cast<int>(i->second));
                }
            }

            class TimeUnitsWidget : public Widget
            {
                DJV_NON_COPYABLE(TimeUnitsWidget);

            protected:
                void _init(Context *);
                TimeUnitsWidget();

            public:
                static std::shared_ptr<TimeUnitsWidget> create(Context *);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

                void _localeEvent(Event::Locale &) override;

            private:
                void _widgetUpdate();
                void _currentItemUpdate();

                DJV_PRIVATE();
            };

            struct TimeUnitsWidget::Private
            {
                AV::TimeUnits timeUnits = AV::TimeUnits::First;
                std::shared_ptr<ComboBox> comboBox;
                std::shared_ptr<ValueObserver<AV::TimeUnits> > timeUnitsObserver;
            };

            void TimeUnitsWidget::_init(Context * context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                p.comboBox = ComboBox::create(context);
                addChild(p.comboBox);

                _widgetUpdate();

                auto weak = std::weak_ptr<TimeUnitsWidget>(std::dynamic_pointer_cast<TimeUnitsWidget>(shared_from_this()));
                p.comboBox->setCallback(
                    [weak, context](int value)
                {
                    if (auto system = context->getSystemT<AV::AVSystem>())
                    {
                        system->setTimeUnits(static_cast<AV::TimeUnits>(value));
                    }
                });

                if (auto avSystem = context->getSystemT<AV::AVSystem>())
                {
                    p.timeUnitsObserver = ValueObserver<AV::TimeUnits>::create(
                        avSystem->observeTimeUnits(),
                        [weak](AV::TimeUnits value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->timeUnits = value;
                            widget->_currentItemUpdate();
                        }
                    });
                }
            }

            TimeUnitsWidget::TimeUnitsWidget() :
                _p(new Private)
            {}

            std::shared_ptr<TimeUnitsWidget> TimeUnitsWidget::create(Context * context)
            {
                auto out = std::shared_ptr<TimeUnitsWidget>(new TimeUnitsWidget);
                out->_init(context);
                return out;
            }

            float TimeUnitsWidget::getHeightForWidth(float value) const
            {
                return _p->comboBox->getHeightForWidth(value);
            }

            void TimeUnitsWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_p->comboBox->getMinimumSize());
            }

            void TimeUnitsWidget::_layoutEvent(Event::Layout &)
            {
                _p->comboBox->setGeometry(getGeometry());
            }

            void TimeUnitsWidget::_localeEvent(Event::Locale & event)
            {
                _widgetUpdate();
            }

            void TimeUnitsWidget::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                p.comboBox->clearItems();
                for (auto i : AV::getTimeUnitsEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    p.comboBox->addItem(_getText(ss.str()));
                }
                _currentItemUpdate();
            }

            void TimeUnitsWidget::_currentItemUpdate()
            {
                DJV_PRIVATE_PTR();
                p.comboBox->setCurrentItem(static_cast<int>(p.timeUnits));
            }

        } // namespace

        struct GeneralSettingsWidget::Private
        {
            std::shared_ptr<DisplaySizeWidget> displayWidget;
            std::shared_ptr<PaletteWidget> paletteWidget;
            std::shared_ptr<LanguageWidget> languageWidget;
            std::shared_ptr<TimeUnitsWidget> timeUnitsWidget;
            std::shared_ptr<FormLayout> layout;
        };

        void GeneralSettingsWidget::_init(Context * context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            p.displayWidget = DisplaySizeWidget::create(context);
            p.paletteWidget = PaletteWidget::create(context);
            p.languageWidget = LanguageWidget::create(context);
            p.timeUnitsWidget = TimeUnitsWidget::create(context);

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.displayWidget);
            p.layout->addChild(p.paletteWidget);
            p.layout->addChild(p.languageWidget);
            p.layout->addChild(p.timeUnitsWidget);
            addChild(p.layout);
        }

        GeneralSettingsWidget::GeneralSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<GeneralSettingsWidget> GeneralSettingsWidget::create(Context * context)
        {
            auto out = std::shared_ptr<GeneralSettingsWidget>(new GeneralSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string GeneralSettingsWidget::getName() const
        {
            return DJV_TEXT("General");
        }

        std::string GeneralSettingsWidget::getSortKey() const
        {
            return "A";
        }

        void GeneralSettingsWidget::_localeEvent(Event::Locale & event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.displayWidget, _getText(DJV_TEXT("Display size:")));
            p.layout->setText(p.paletteWidget, _getText(DJV_TEXT("Palette:")));
            p.layout->setText(p.languageWidget, _getText(DJV_TEXT("Language:")));
            p.layout->setText(p.timeUnitsWidget, _getText(DJV_TEXT("Time units:")));
        }

    } // namespace UI
} // namespace djv

