// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/StyleSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StyleSettings.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        struct SizeWidget::Private
        {
            std::vector<std::string> metrics;
            std::string currentMetrics;
            std::shared_ptr<UI::ComboBox> comboBox;
            std::map<size_t, std::string> indexToMetrics;
            std::map<std::string, size_t> metricsToIndex;
            std::shared_ptr<Observer::Map<std::string, UI::Style::Metrics> > metricsObserver;
            std::shared_ptr<Observer::Value<std::string> > currentMetricsObserver;
        };

        void SizeWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UIComponents::SizeWidget");

            p.comboBox = UI::ComboBox::create(context);
            addChild(p.comboBox);

            auto weak = std::weak_ptr<SizeWidget>(std::dynamic_pointer_cast<SizeWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.comboBox->setCallback(
                [weak, contextWeak](int value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto styleSettings = settingsSystem->getSettingsT<UI::Settings::Style>();
                        const auto i = widget->_p->indexToMetrics.find(value);
                        if (i != widget->_p->indexToMetrics.end())
                        {
                            styleSettings->setCurrentMetrics(i->second);
                        }
                    }
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto styleSettings = settingsSystem->getSettingsT<UI::Settings::Style>();
            p.metricsObserver = Observer::Map<std::string, UI::Style::Metrics>::create(
                styleSettings->observeMetrics(),
                [weak](const std::map<std::string, UI::Style::Metrics>& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->metrics.clear();
                    for (const auto& i : value)
                    {
                        widget->_p->metrics.push_back(i.first);
                    }
                    widget->_widgetUpdate();
                }
            });
            p.currentMetricsObserver = Observer::Value<std::string>::create(
                styleSettings->observeCurrentMetricsName(),
                [weak](const std::string& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->currentMetrics = value;
                    widget->_currentItemUpdate();
                }
            });
        }

        SizeWidget::SizeWidget() :
            _p(new Private)
        {}

        std::shared_ptr<SizeWidget> SizeWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<SizeWidget>(new SizeWidget);
            out->_init(context);
            return out;
        }

        void SizeWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void SizeWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void SizeWidget::_initEvent(System::Event::Init& event)
        {
            if (event.getData().text)
            {
                _widgetUpdate();
            }
        }

        void SizeWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.indexToMetrics.clear();
            p.metricsToIndex.clear();
            std::vector<std::string> items;
            for (size_t i = 0; i < p.metrics.size(); ++i)
            {
                const auto& name = p.metrics[i];
                items.push_back(_getText(name));
                p.indexToMetrics[i] = name;
                p.metricsToIndex[name] = i;
            }
            const auto i = p.metricsToIndex.find(p.currentMetrics);
            if (i != p.metricsToIndex.end())
            {
                p.comboBox->setItems(items);
                p.comboBox->setCurrentItem(static_cast<int>(i->second));
            }
        }

        void SizeWidget::_currentItemUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto i = p.metricsToIndex.find(p.currentMetrics);
            if (i != p.metricsToIndex.end())
            {
                p.comboBox->setCurrentItem(static_cast<int>(i->second));
            }
        }

        struct PaletteWidget::Private
        {
            std::vector<std::string> palettes;
            std::string currentPalette;
            std::shared_ptr<UI::ComboBox> comboBox;
            std::map<size_t, std::string> indexToPalette;
            std::map<std::string, size_t> paletteToIndex;
            std::shared_ptr<Observer::Map<std::string, UI::Style::Palette> > palettesObserver;
            std::shared_ptr<Observer::Value<std::string> > currentPaletteObserver;
        };

        void PaletteWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UIComponents::PaletteWidget");

            p.comboBox = UI::ComboBox::create(context);
            addChild(p.comboBox);

            auto weak = std::weak_ptr<PaletteWidget>(std::dynamic_pointer_cast<PaletteWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.comboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto styleSettings = settingsSystem->getSettingsT<UI::Settings::Style>();
                            const auto i = widget->_p->indexToPalette.find(value);
                            if (i != widget->_p->indexToPalette.end())
                            {
                                styleSettings->setCurrentPalette(i->second);
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto styleSettings = settingsSystem->getSettingsT<UI::Settings::Style>();
            p.palettesObserver = Observer::Map<std::string, UI::Style::Palette>::create(
                styleSettings->observePalettes(),
                [weak](const std::map<std::string, UI::Style::Palette > & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->palettes.clear();
                    for (const auto& i : value)
                    {
                        widget->_p->palettes.push_back(i.first);
                    }
                    widget->_widgetUpdate();
                }
            });
            p.currentPaletteObserver = Observer::Value<std::string>::create(
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

        PaletteWidget::PaletteWidget() :
            _p(new Private)
        {}

        std::shared_ptr<PaletteWidget> PaletteWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<PaletteWidget>(new PaletteWidget);
            out->_init(context);
            return out;
        }

        void PaletteWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void PaletteWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void PaletteWidget::_initEvent(System::Event::Init& event)
        {
            if (event.getData().text)
            {
                _widgetUpdate();
            }
        }

        void PaletteWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.indexToPalette.clear();
            p.paletteToIndex.clear();
            std::vector<std::string> items;
            for (size_t i = 0; i < p.palettes.size(); ++i)
            {
                const auto& name = p.palettes[i];
                items.push_back(_getText(name));
                p.indexToPalette[i] = name;
                p.paletteToIndex[name] = i;
            }
            const auto i = p.paletteToIndex.find(p.currentPalette);
            if (i != p.paletteToIndex.end())
            {
                p.comboBox->setItems(items);
                p.comboBox->setCurrentItem(static_cast<int>(i->second));
            }
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

        namespace Settings
        {
            struct StyleWidget::Private
            {
                std::shared_ptr<SizeWidget> sizeWidget;
                std::shared_ptr<PaletteWidget> paletteWidget;
                std::shared_ptr<UI::Numeric::FloatSlider> brightnessSlider;
                std::shared_ptr<UI::Numeric::FloatSlider> contrastSlider;
                std::shared_ptr<UI::FormLayout> layout;
                std::shared_ptr<Observer::Value<float> > brightnessObserver;
                std::shared_ptr<Observer::Value<float> > contrastObserver;
            };

            void StyleWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IWidget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UIComponents::Settings::StyleWidget");

                p.sizeWidget = SizeWidget::create(context);

                p.paletteWidget = PaletteWidget::create(context);

                p.brightnessSlider = UI::Numeric::FloatSlider::create(context);
                p.brightnessSlider->setRange(Math::FloatRange(.5F, 1.5F));
                p.brightnessSlider->setDefault(1.F);
                p.brightnessSlider->setDefaultVisible(true);

                p.contrastSlider = UI::Numeric::FloatSlider::create(context);
                p.contrastSlider->setRange(Math::FloatRange(.5F, 1.5F));
                p.contrastSlider->setDefault(1.F);
                p.contrastSlider->setDefaultVisible(true);

                p.layout = UI::FormLayout::create(context);
                p.layout->addChild(p.sizeWidget);
                p.layout->addChild(p.paletteWidget);
                p.layout->addChild(p.brightnessSlider);
                p.layout->addChild(p.contrastSlider);
                addChild(p.layout);

                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.brightnessSlider->setValueCallback(
                    [contextWeak](float value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto styleSettings = settingsSystem->getSettingsT<UI::Settings::Style>();
                            styleSettings->setBrightness(value);
                        }
                    });

                p.contrastSlider->setValueCallback(
                    [contextWeak](float value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto styleSettings = settingsSystem->getSettingsT<UI::Settings::Style>();
                            styleSettings->setContrast(value);
                        }
                    });

                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                auto styleSettings = settingsSystem->getSettingsT<UI::Settings::Style>();
                auto weak = std::weak_ptr<StyleWidget>(std::dynamic_pointer_cast<StyleWidget>(shared_from_this()));
                p.brightnessObserver = Observer::Value<float>::create(
                    styleSettings->observeBrightness(),
                    [weak](const float value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->brightnessSlider->setValue(value);
                        }
                    });

                p.contrastObserver = Observer::Value<float>::create(
                    styleSettings->observeContrast(),
                    [weak](const float value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->contrastSlider->setValue(value);
                        }
                    });
            }

            StyleWidget::StyleWidget() :
                _p(new Private)
            {}

            std::shared_ptr<StyleWidget> StyleWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<StyleWidget>(new StyleWidget);
                out->_init(context);
                return out;
            }

            std::string StyleWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_general");
            }

            std::string StyleWidget::getSettingsSortKey() const
            {
                return "0";
            }

            void StyleWidget::setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>& value)
            {
                _p->layout->setLabelSizeGroup(value);
            }

            void StyleWidget::_initEvent(System::Event::Init& event)
            {
                IWidget::_initEvent(event);
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.layout->setText(p.sizeWidget, _getText(DJV_TEXT("settings_style_size")) + ":");
                    p.layout->setText(p.paletteWidget, _getText(DJV_TEXT("settings_style_palette")) + ":");
                    p.layout->setText(p.brightnessSlider, _getText(DJV_TEXT("settings_style_brightness")) + ":");
                    p.layout->setText(p.contrastSlider, _getText(DJV_TEXT("settings_style_contrast")) + ":");
                }
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

