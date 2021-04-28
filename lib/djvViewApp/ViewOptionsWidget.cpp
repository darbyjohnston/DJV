// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewOptionsWidget.h>

#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ViewData.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/ViewSystem.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/Bellows.h>
#include <djvUI/CheckBox.h>
#include <djvUI/ComboBox.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToolButton.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewOptionsGridWidget::Private
        {
            GridOptions gridOptions;

            std::shared_ptr<UI::ToolButton> gridEnabledButton;
            std::shared_ptr<UI::Numeric::IntSlider> gridSizeSlider;
            std::shared_ptr<UIComponents::ColorPickerSwatch> gridColorPickerSwatch;
            std::shared_ptr<UI::ComboBox> gridLabelsComboBox;
            std::shared_ptr<UIComponents::ColorPickerSwatch> gridLabelsColorPickerSwatch;
            std::shared_ptr<UI::FormLayout> layout;

            std::shared_ptr<Observer::Value<GridOptions> > gridOptionsObserver;
        };

        void ViewOptionsGridWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            auto viewSystem = context->getSystemT<ViewSystem>();
            auto viewActions = viewSystem->getActions();
            p.gridEnabledButton = UI::ToolButton::create(context);
            p.gridEnabledButton->addAction(viewActions["Grid"]);
            p.gridEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.gridSizeSlider = UI::Numeric::IntSlider::create(context);
            p.gridSizeSlider->setRange(Math::IntRange(1, 500));
            p.gridSizeSlider->setSmallIncrement(1);
            p.gridSizeSlider->setLargeIncrement(10);
            p.gridColorPickerSwatch = UIComponents::ColorPickerSwatch::create(context);
            p.gridColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.gridColorPickerSwatch->setHAlign(UI::HAlign::Fill);
            p.gridLabelsComboBox = UI::ComboBox::create(context);
            p.gridLabelsColorPickerSwatch = UIComponents::ColorPickerSwatch::create(context);
            p.gridLabelsColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.gridLabelsColorPickerSwatch->setHAlign(UI::HAlign::Fill);

            p.layout = UI::FormLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::MarginSmall);
            p.layout->addChild(p.gridSizeSlider);
            p.layout->addChild(p.gridColorPickerSwatch);
            p.layout->addChild(p.gridLabelsComboBox);
            p.layout->addChild(p.gridLabelsColorPickerSwatch);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewOptionsGridWidget>(std::dynamic_pointer_cast<ViewOptionsGridWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.gridSizeSlider->setValueCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto options = widget->_p->gridOptions;
                            options.size = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(options);
                        }
                    }
                });
            p.gridColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto options = widget->_p->gridOptions;
                            options.color = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(options);
                        }
                    }
                });
            p.gridLabelsComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto options = widget->_p->gridOptions;
                            options.labels = static_cast<GridLabels>(value);
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(options);
                        }
                    }
                });
            p.gridLabelsColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto options = widget->_p->gridOptions;
                            options.labelsColor = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(options);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.gridOptionsObserver = Observer::Value<GridOptions>::create(
                viewSettings->observeGridOptions(),
                [weak](const GridOptions& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->gridOptions = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ViewOptionsGridWidget::ViewOptionsGridWidget() :
            _p(new Private)
        {}

        ViewOptionsGridWidget::~ViewOptionsGridWidget()
        {}

        std::shared_ptr<ViewOptionsGridWidget> ViewOptionsGridWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewOptionsGridWidget>(new ViewOptionsGridWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<UI::ToolButton>& ViewOptionsGridWidget::getEnabledButton() const
        {
            return _p->gridEnabledButton;
        }

        void ViewOptionsGridWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewOptionsGridWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewOptionsGridWidget::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.gridSizeSlider, _getText(DJV_TEXT("widget_view_grid_size")) + ":");
                p.layout->setText(p.gridColorPickerSwatch, _getText(DJV_TEXT("widget_view_grid_color")) + ":");
                p.layout->setText(p.gridLabelsComboBox, _getText(DJV_TEXT("widget_view_grid_labels")) + ":");
                p.layout->setText(p.gridLabelsColorPickerSwatch, _getText(DJV_TEXT("widget_view_grid_labels_color")) + ":");
            }
        }

        void ViewOptionsGridWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.gridSizeSlider->setValue(p.gridOptions.size);
            p.gridColorPickerSwatch->setColor(p.gridOptions.color);
            std::vector<std::string> items;
            for (auto i : getGridLabelsEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.gridLabelsComboBox->setItems(items);
            p.gridLabelsComboBox->setCurrentItem(static_cast<int>(p.gridOptions.labels));
            p.gridLabelsColorPickerSwatch->setColor(p.gridOptions.labelsColor);
        }

        struct ViewOptionsHUDWidget::Private
        {
            HUDOptions hudOptions;

            std::shared_ptr<UI::ToolButton> hudEnabledButton;
            std::shared_ptr<MediaWidget> activeWidget;
            std::shared_ptr<UIComponents::ColorPickerSwatch> hudColorPickerSwatch;
            std::shared_ptr<UI::ComboBox> hudBackgroundComboBox;
            std::shared_ptr<UI::FormLayout> layout;

            std::shared_ptr<Observer::Value<HUDOptions> > hudOptionsObserver;
        };

        void ViewOptionsHUDWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            auto viewSystem = context->getSystemT<ViewSystem>();
            auto viewActions = viewSystem->getActions();
            p.hudEnabledButton = UI::ToolButton::create(context);
            p.hudEnabledButton->addAction(viewActions["HUD"]);
            p.hudEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.hudColorPickerSwatch = UIComponents::ColorPickerSwatch::create(context);
            p.hudColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.hudColorPickerSwatch->setHAlign(UI::HAlign::Fill);
            p.hudBackgroundComboBox = UI::ComboBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::MarginSmall);
            p.layout->addChild(p.hudColorPickerSwatch);
            p.layout->addChild(p.hudBackgroundComboBox);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewOptionsHUDWidget>(std::dynamic_pointer_cast<ViewOptionsHUDWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.hudColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            HUDOptions options = widget->_p->hudOptions;
                            options.color = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setHUDOptions(options);
                        }
                    }
                });
            p.hudBackgroundComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            HUDOptions options = widget->_p->hudOptions;
                            options.background = static_cast<HUDBackground>(value);
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setHUDOptions(options);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.hudOptionsObserver = Observer::Value<HUDOptions>::create(
                viewSettings->observeHUDOptions(),
                [weak](const HUDOptions& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->hudOptions = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ViewOptionsHUDWidget::ViewOptionsHUDWidget() :
            _p(new Private)
        {}

        ViewOptionsHUDWidget::~ViewOptionsHUDWidget()
        {}

        std::shared_ptr<ViewOptionsHUDWidget> ViewOptionsHUDWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewOptionsHUDWidget>(new ViewOptionsHUDWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<UI::ToolButton>& ViewOptionsHUDWidget::getEnabledButton() const
        {
            return _p->hudEnabledButton;
        }

        void ViewOptionsHUDWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewOptionsHUDWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewOptionsHUDWidget::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.hudColorPickerSwatch, _getText(DJV_TEXT("widget_view_hud_color")) + ":");
                p.layout->setText(p.hudBackgroundComboBox, _getText(DJV_TEXT("widget_view_hud_background")) + ":");
            }
        }

        void ViewOptionsHUDWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.hudColorPickerSwatch->setColor(p.hudOptions.color);
            std::vector<std::string> items;
            for (auto i : getHUDBackgroundEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.hudBackgroundComboBox->setItems(items);
            p.hudBackgroundComboBox->setCurrentItem(static_cast<int>(p.hudOptions.background));
        }

        struct ViewOptionsBackgroundWidget::Private
        {
            ViewBackgroundOptions backgroundOptions;

            std::shared_ptr<UI::ComboBox> backgroundComboBox;
            std::shared_ptr<UIComponents::ColorPickerSwatch> solidPickerSwatch;
            std::shared_ptr<UI::Numeric::FloatSlider> checkersSizeSlider;
            std::shared_ptr<UIComponents::ColorPickerSwatch> checkersColorPickerSwatches[2];
            std::shared_ptr<UI::HorizontalLayout> checkersColorsLayout;
            std::shared_ptr<UI::FormLayout> backgroundLayout;
            std::shared_ptr<UI::FormLayout> solidLayout;
            std::shared_ptr<UI::FormLayout> checkersLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Value<ViewBackgroundOptions> > backgroundOptionsObserver;
        };

        void ViewOptionsBackgroundWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            p.backgroundComboBox = UI::ComboBox::create(context);
            p.solidPickerSwatch = UIComponents::ColorPickerSwatch::create(context);
            p.solidPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.solidPickerSwatch->setHAlign(UI::HAlign::Fill);
            p.checkersSizeSlider = UI::Numeric::FloatSlider::create(context);
            p.checkersSizeSlider->setRange(Math::FloatRange(10.F, 100.F));
            p.checkersSizeSlider->setSmallIncrement(1);
            p.checkersSizeSlider->setLargeIncrement(10);
            p.checkersColorPickerSwatches[0] = UIComponents::ColorPickerSwatch::create(context);
            p.checkersColorPickerSwatches[0]->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.checkersColorPickerSwatches[0]->setHAlign(UI::HAlign::Fill);
            p.checkersColorPickerSwatches[1] = UIComponents::ColorPickerSwatch::create(context);
            p.checkersColorPickerSwatches[1]->setHAlign(UI::HAlign::Fill);
            p.checkersColorPickerSwatches[1]->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::MarginSmall);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.backgroundLayout = UI::FormLayout::create(context);
            p.backgroundLayout->addChild(p.backgroundComboBox);
            p.layout->addChild(p.backgroundLayout);
            p.solidLayout = UI::FormLayout::create(context);
            p.solidLayout->addChild(p.solidPickerSwatch);
            p.layout->addChild(p.solidLayout);
            p.checkersLayout = UI::FormLayout::create(context);
            p.checkersLayout->addChild(p.checkersSizeSlider);
            p.checkersColorsLayout = UI::HorizontalLayout::create(context);
            p.checkersColorsLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            p.checkersColorsLayout->addChild(p.checkersColorPickerSwatches[0]);
            p.checkersColorsLayout->setStretch(p.checkersColorPickerSwatches[0]);
            p.checkersColorsLayout->addChild(p.checkersColorPickerSwatches[1]);
            p.checkersColorsLayout->setStretch(p.checkersColorPickerSwatches[1]);
            p.checkersLayout->addChild(p.checkersColorsLayout);
            p.layout->addChild(p.checkersLayout);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewOptionsBackgroundWidget>(std::dynamic_pointer_cast<ViewOptionsBackgroundWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.backgroundComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            ViewBackgroundOptions options = widget->_p->backgroundOptions;
                            options.background = static_cast<ViewBackground>(value);
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(options);
                        }
                    }
                });
            p.solidPickerSwatch->setColorCallback(
                [weak, contextWeak](const Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            ViewBackgroundOptions options = widget->_p->backgroundOptions;
                            options.color = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(options);
                        }
                    }
                });
            p.checkersSizeSlider->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            ViewBackgroundOptions options = widget->_p->backgroundOptions;
                            options.checkersSize = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(options);
                        }
                    }
                });
            p.checkersColorPickerSwatches[0]->setColorCallback(
                [weak, contextWeak](const Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            ViewBackgroundOptions options = widget->_p->backgroundOptions;
                            options.checkersColors[0] = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(options);
                        }
                    }
                });
            p.checkersColorPickerSwatches[1]->setColorCallback(
                [weak, contextWeak](const Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            ViewBackgroundOptions options = widget->_p->backgroundOptions;
                            options.checkersColors[1] = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(options);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.backgroundOptionsObserver = Observer::Value<ViewBackgroundOptions>::create(
                viewSettings->observeBackgroundOptions(),
                [weak](const ViewBackgroundOptions& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundOptions = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ViewOptionsBackgroundWidget::ViewOptionsBackgroundWidget() :
            _p(new Private)
        {}

        ViewOptionsBackgroundWidget::~ViewOptionsBackgroundWidget()
        {}

        std::shared_ptr<ViewOptionsBackgroundWidget> ViewOptionsBackgroundWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewOptionsBackgroundWidget>(new ViewOptionsBackgroundWidget);
            out->_init(context);
            return out;
        }

        void ViewOptionsBackgroundWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewOptionsBackgroundWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewOptionsBackgroundWidget::_initEvent(System::Event::Init& event)
        {
            if (event.getData().text)
            {
                DJV_PRIVATE_PTR();
                p.backgroundLayout->setText(p.backgroundComboBox, _getText(DJV_TEXT("widget_view_background")) + ":");
                p.solidLayout->setText(p.solidPickerSwatch, _getText(DJV_TEXT("widget_view_background_color")) + ":");
                p.checkersLayout->setText(p.checkersSizeSlider, _getText(DJV_TEXT("widget_view_background_checkers_size")) + ":");
                p.checkersLayout->setText(p.checkersColorsLayout, _getText(DJV_TEXT("widget_view_background_checkers_colors")) + ":");
            }
        }

        void ViewOptionsBackgroundWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            switch (p.backgroundOptions.background)
            {
            case ViewBackground::Solid:
                p.solidLayout->show();
                p.checkersLayout->hide();
                break;
            case ViewBackground::Checkers:
                p.solidLayout->hide();
                p.checkersLayout->show();
                break;
            default: break;
            }
            std::vector<std::string> items;
            for (auto i : getViewBackgroundEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.backgroundComboBox->setItems(items);
            p.backgroundComboBox->setCurrentItem(static_cast<int>(p.backgroundOptions.background));
            p.solidPickerSwatch->setColor(p.backgroundOptions.color);
            p.checkersSizeSlider->setValue(p.backgroundOptions.checkersSize);
            p.checkersColorPickerSwatches[0]->setColor(p.backgroundOptions.checkersColors[0]);
            p.checkersColorPickerSwatches[1]->setColor(p.backgroundOptions.checkersColors[1]);
        }

        struct ViewOptionsBorderWidget::Private
        {
            ViewBackgroundOptions backgroundOptions;

            std::shared_ptr<UI::ToolButton> borderEnabledButton;
            std::shared_ptr<UI::Numeric::FloatSlider> borderWidthSlider;
            std::shared_ptr<UIComponents::ColorPickerSwatch> borderColorPickerSwatch;
            std::shared_ptr<UI::FormLayout> layout;

            std::shared_ptr<Observer::Value<ViewBackgroundOptions> > backgroundOptionsObserver;
        };

        void ViewOptionsBorderWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            auto viewSystem = context->getSystemT<ViewSystem>();
            auto viewActions = viewSystem->getActions();
            p.borderEnabledButton = UI::ToolButton::create(context);
            p.borderEnabledButton->addAction(viewActions["Border"]);
            p.borderEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.borderWidthSlider = UI::Numeric::FloatSlider::create(context);
            p.borderWidthSlider->setRange(Math::FloatRange(1.F, 20.F));
            p.borderWidthSlider->setSmallIncrement(1.F);
            p.borderWidthSlider->setLargeIncrement(5.F);
            p.borderColorPickerSwatch = UIComponents::ColorPickerSwatch::create(context);
            p.borderColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.borderColorPickerSwatch->setHAlign(UI::HAlign::Fill);

            p.layout = UI::FormLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::MarginSmall);
            p.layout->addChild(p.borderWidthSlider);
            p.layout->addChild(p.borderColorPickerSwatch);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewOptionsBorderWidget>(std::dynamic_pointer_cast<ViewOptionsBorderWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.borderWidthSlider->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            ViewBackgroundOptions options = widget->_p->backgroundOptions;
                            options.borderWidth = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(options);
                        }
                    }
                });
            p.borderColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            ViewBackgroundOptions options = widget->_p->backgroundOptions;
                            options.borderColor = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(options);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.backgroundOptionsObserver = Observer::Value<ViewBackgroundOptions>::create(
                viewSettings->observeBackgroundOptions(),
                [weak](const ViewBackgroundOptions& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundOptions = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ViewOptionsBorderWidget::ViewOptionsBorderWidget() :
            _p(new Private)
        {}

        ViewOptionsBorderWidget::~ViewOptionsBorderWidget()
        {}

        std::shared_ptr<ViewOptionsBorderWidget> ViewOptionsBorderWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewOptionsBorderWidget>(new ViewOptionsBorderWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<UI::ToolButton>& ViewOptionsBorderWidget::getEnabledButton() const
        {
            return _p->borderEnabledButton;
        }

        void ViewOptionsBorderWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewOptionsBorderWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewOptionsBorderWidget::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.borderEnabledButton->setTooltip(_getText(DJV_TEXT("widget_view_border_enabled_tooltip")));
                p.layout->setText(p.borderWidthSlider, _getText(DJV_TEXT("widget_view_border_width")) + ":");
                p.layout->setText(p.borderColorPickerSwatch, _getText(DJV_TEXT("widget_view_border_color")) + ":");
            }
        }

        void ViewOptionsBorderWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.borderEnabledButton->setChecked(p.backgroundOptions.border);
            p.borderWidthSlider->setValue(p.backgroundOptions.borderWidth);
            p.borderColorPickerSwatch->setColor(p.backgroundOptions.borderColor);
        }

        struct ViewOptionsWidget::Private
        {
            std::shared_ptr<ViewOptionsGridWidget> gridWidget;
            std::shared_ptr<ViewOptionsHUDWidget> hudWidget;
            std::shared_ptr<ViewOptionsBackgroundWidget> backgroundWidget;
            std::shared_ptr<ViewOptionsBorderWidget> borderWidget;
            std::map<std::string, std::shared_ptr<UI::Bellows> > bellows;
            std::shared_ptr<UI::VerticalLayout> layout;
        };

        void ViewOptionsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewOptionsWidget");

            p.gridWidget = ViewOptionsGridWidget::create(context);
            p.hudWidget = ViewOptionsHUDWidget::create(context);
            p.backgroundWidget = ViewOptionsBackgroundWidget::create(context);
            p.borderWidget = ViewOptionsBorderWidget::create(context);

            p.bellows["Grid"] = UI::Bellows::create(context);
            p.bellows["Grid"]->addButtonWidget(p.gridWidget->getEnabledButton());
            p.bellows["Grid"]->addChild(p.gridWidget);
            p.bellows["HUD"] = UI::Bellows::create(context);
            p.bellows["HUD"]->addButtonWidget(p.hudWidget->getEnabledButton());
            p.bellows["HUD"]->addChild(p.hudWidget);
            p.bellows["Background"] = UI::Bellows::create(context);
            p.bellows["Background"]->addChild(p.backgroundWidget);
            p.bellows["Border"] = UI::Bellows::create(context);
            p.bellows["Border"]->addButtonWidget(p.borderWidget->getEnabledButton());
            p.bellows["Border"]->addChild(p.borderWidget);
            
            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.bellows["Grid"]);
            p.layout->addChild(p.bellows["HUD"]);
            p.layout->addChild(p.bellows["Background"]);
            p.layout->addChild(p.bellows["Border"]);
            addChild(p.layout);
        }

        ViewOptionsWidget::ViewOptionsWidget() :
            _p(new Private)
        {}

        ViewOptionsWidget::~ViewOptionsWidget()
        {}

        std::shared_ptr<ViewOptionsWidget> ViewOptionsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewOptionsWidget>(new ViewOptionsWidget);
            out->_init(context);
            return out;
        }

        std::map<std::string, bool> ViewOptionsWidget::getBellowsState() const
        {
            DJV_PRIVATE_PTR();
            std::map<std::string, bool> out;
            for (const auto& i : p.bellows)
            {
                out[i.first] = i.second->isOpen();
            }
            return out;
        }

        void ViewOptionsWidget::setBellowsState(const std::map<std::string, bool>& value)
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : value)
            {
                const auto j = p.bellows.find(i.first);
                if (j != p.bellows.end())
                {
                    j->second->setOpen(i.second, false);
                }
            }
        }

        void ViewOptionsWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewOptionsWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewOptionsWidget::_initEvent(System::Event::Init & event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.bellows["Grid"]->setText(_getText(DJV_TEXT("view_grid")));
                p.bellows["HUD"]->setText(_getText(DJV_TEXT("view_hud")));
                p.bellows["Background"]->setText(_getText(DJV_TEXT("view_background")));
                p.bellows["Border"]->setText(_getText(DJV_TEXT("view_border")));
            }
        }

    } // namespace ViewApp
} // namespace djv

