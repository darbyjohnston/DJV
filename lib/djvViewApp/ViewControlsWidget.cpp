// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewControlsWidget.h>

#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/CheckBox.h>
#include <djvUI/ComboBox.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/TabWidget.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Context.h>
#include <djvCore/NumericValueModels.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewControlsWidget::Private
        {
            glm::vec2 viewPos = glm::vec2(0.F, 0.F);
            float viewZoom = 1.F;
            GridOptions gridOptions;
            HUDOptions hudOptions;
            AV::Image::Color backgroundColor;

            std::shared_ptr<MediaWidget> activeWidget;

            std::shared_ptr<UI::FloatEdit> viewPosEdit[2];
            std::shared_ptr<UI::ToolButton> viewPosResetButton;
            std::shared_ptr<UI::FloatEdit> viewZoomEdit;
            std::shared_ptr<UI::ToolButton> viewZoomResetButton;
            std::shared_ptr<UI::FormLayout> viewFormLayout;
            std::shared_ptr<UI::HorizontalLayout> viewPosLayout;
            std::shared_ptr<UI::HorizontalLayout> viewZoomLayout;
            std::shared_ptr<UI::ScrollWidget> viewScrollWidget;

            std::shared_ptr<UI::CheckBox> gridEnabledCheckBox;
            std::shared_ptr<UI::IntSlider> gridSizeSlider;
            std::shared_ptr<UI::ColorPickerSwatch> gridColorPickerSwatch;
            std::shared_ptr<UI::ComboBox> gridLabelsComboBox;
            std::shared_ptr<UI::ColorPickerSwatch> gridLabelsColorPickerSwatch;
            std::shared_ptr<UI::FormLayout> gridFormLayout;
            std::shared_ptr<UI::ScrollWidget> gridScrollWidget;

            std::shared_ptr<UI::CheckBox> hudEnabledCheckBox;
            std::shared_ptr<UI::ColorPickerSwatch> hudColorPickerSwatch;
            std::shared_ptr<UI::ComboBox> hudBackgroundComboBox;
            std::shared_ptr<UI::FormLayout> hudFormLayout;
            std::shared_ptr<UI::ScrollWidget> hudScrollWidget;

            std::shared_ptr<UI::ColorPickerSwatch> backgroundColorPickerSwatch;
            std::shared_ptr<UI::FormLayout> backgroundFormLayout;
            std::shared_ptr<UI::ScrollWidget> backgroundScrollWidget;

            std::shared_ptr<UI::TabWidget> tabWidget;

            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<glm::vec2> > viewPosObserver;
            std::shared_ptr<ValueObserver<float> > viewZoomObserver;
            std::shared_ptr<ValueObserver<GridOptions> > gridOptionsObserver;
            std::shared_ptr<ValueObserver<HUDOptions> > hudOptionsObserver;
            std::shared_ptr<ValueObserver<AV::Image::Color> > backgroundColorObserver;
        };

        void ViewControlsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewControlsWidget");

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.gridOptions = viewSettings->observeGridOptions()->get();
            p.hudOptions = viewSettings->observeHUDOptions()->get();
            p.backgroundColor = viewSettings->observeBackgroundColor()->get();

            for (size_t i = 0; i < 2; ++i)
            {
                p.viewPosEdit[i] = UI::FloatEdit::create(context);
                auto model = FloatValueModel::create();
                model->setRange(FloatRange(-1000000.F, 1000000.F));
                model->setSmallIncrement(1.F);
                model->setLargeIncrement(10.F);
                p.viewPosEdit[i]->setModel(model);
            }
            p.viewPosResetButton = UI::ToolButton::create(context);
            p.viewPosResetButton->setIcon("djvIconCloseSmall");
            p.viewZoomEdit = UI::FloatEdit::create(context);
            auto model = FloatValueModel::create();
            model->setRange(FloatRange(.1F, 1000.F));
            model->setSmallIncrement(.1F);
            model->setLargeIncrement(1.F);
            p.viewZoomEdit->setModel(model);
            p.viewZoomResetButton = UI::ToolButton::create(context);
            p.viewZoomResetButton->setIcon("djvIconCloseSmall");

            p.gridEnabledCheckBox = UI::CheckBox::create(context);
            p.gridSizeSlider = UI::IntSlider::create(context);
            p.gridSizeSlider->setRange(IntRange(1, 500));
            p.gridColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.gridColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.gridLabelsComboBox = UI::ComboBox::create(context);
            p.gridLabelsColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.gridLabelsColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);

            p.hudEnabledCheckBox = UI::CheckBox::create(context);
            p.hudColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.hudColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.hudBackgroundComboBox = UI::ComboBox::create(context);

            p.backgroundColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.backgroundColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);

            p.viewFormLayout = UI::FormLayout::create(context);
            p.viewFormLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.viewFormLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.viewFormLayout->setShadowOverlay({ UI::Side::Top });
            p.viewPosLayout = UI::HorizontalLayout::create(context);
            p.viewPosLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            for (size_t i = 0; i < 2; ++i)
            {
                p.viewPosLayout->addChild(p.viewPosEdit[i]);
            }
            p.viewPosLayout->addChild(p.viewPosResetButton);
            p.viewFormLayout->addChild(p.viewPosLayout);
            p.viewZoomLayout = UI::HorizontalLayout::create(context);
            p.viewZoomLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.viewZoomLayout->addChild(p.viewZoomEdit);
            p.viewZoomLayout->addChild(p.viewZoomResetButton);
            p.viewFormLayout->addChild(p.viewZoomLayout);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            vLayout->addChild(p.viewFormLayout);
            vLayout->setStretch(p.viewFormLayout, UI::RowStretch::Expand);
            p.viewScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.viewScrollWidget->setBorder(false);
            p.viewScrollWidget->addChild(vLayout);

            p.gridFormLayout = UI::FormLayout::create(context);
            p.gridFormLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.gridFormLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.gridFormLayout->setShadowOverlay({ UI::Side::Top });
            p.gridFormLayout->addChild(p.gridEnabledCheckBox);
            p.gridFormLayout->addChild(p.gridSizeSlider);
            p.gridFormLayout->addChild(p.gridColorPickerSwatch);
            p.gridFormLayout->addChild(p.gridLabelsComboBox);
            p.gridFormLayout->addChild(p.gridLabelsColorPickerSwatch);
            p.gridScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.gridScrollWidget->setBorder(false);
            p.gridScrollWidget->addChild(p.gridFormLayout);

            p.hudFormLayout = UI::FormLayout::create(context);
            p.hudFormLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.hudFormLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.hudFormLayout->setShadowOverlay({ UI::Side::Top });
            p.hudFormLayout->addChild(p.hudEnabledCheckBox);
            p.hudFormLayout->addChild(p.hudColorPickerSwatch);
            p.hudFormLayout->addChild(p.hudBackgroundComboBox);
            p.hudScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.hudScrollWidget->setBorder(false);
            p.hudScrollWidget->addChild(p.hudFormLayout);

            p.backgroundFormLayout = UI::FormLayout::create(context);
            p.backgroundFormLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.backgroundFormLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.backgroundFormLayout->setShadowOverlay({ UI::Side::Top });
            p.backgroundFormLayout->addChild(p.backgroundColorPickerSwatch);
            p.backgroundScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.backgroundScrollWidget->setBorder(false);
            p.backgroundScrollWidget->addChild(p.backgroundFormLayout);

            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setBackgroundRole(UI::ColorRole::Background);
            p.tabWidget->setShadowOverlay({ UI::Side::Top });
            p.tabWidget->addChild(p.viewScrollWidget);
            p.tabWidget->addChild(p.gridScrollWidget);
            p.tabWidget->addChild(p.hudScrollWidget);
            p.tabWidget->addChild(p.backgroundScrollWidget);
            addChild(p.tabWidget);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewControlsWidget>(std::dynamic_pointer_cast<ViewControlsWidget>(shared_from_this()));
            p.viewPosEdit[0]->setValueCallback(
                [weak](float value, UI::TextEditReason)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setPos(glm::vec2(value, widget->_p->viewPos.y));
                    }
                });
            p.viewPosEdit[1]->setValueCallback(
                [weak](float value, UI::TextEditReason)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setPos(glm::vec2(widget->_p->viewPos.x, value));
                    }
                });

            p.viewPosResetButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setPos(glm::vec2(0.F, 0.F));
                    }
                });

            p.viewZoomEdit->setValueCallback(
                [weak](float value, UI::TextEditReason)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setZoom(value);
                    }
                });

            p.viewZoomResetButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setZoom(1.F);
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.gridEnabledCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->gridOptions.enabled = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getImageView()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });
            p.gridSizeSlider->setValueCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->gridOptions.size = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getImageView()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });
            p.gridColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const AV::Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->gridOptions.color = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getImageView()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
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
                            widget->_p->gridOptions.labels = static_cast<ImageViewGridLabels>(value);
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getImageView()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });
            p.gridLabelsColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const AV::Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->gridOptions.labelsColor = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getImageView()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });

            p.hudEnabledCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->hudOptions.enabled = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->setHUDOptions(widget->_p->hudOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setHUDOptions(widget->_p->hudOptions);
                    }
                }
            });
            p.hudColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const AV::Image::Color& value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->hudOptions.color = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->setHUDOptions(widget->_p->hudOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setHUDOptions(widget->_p->hudOptions);
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
                        widget->_p->hudOptions.background = static_cast<HUDBackground>(value);
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->setHUDOptions(widget->_p->hudOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setHUDOptions(widget->_p->hudOptions);
                    }
                }
            });

            p.backgroundColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const AV::Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->backgroundColor = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getImageView()->setBackgroundColor(widget->_p->backgroundColor);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundColor(widget->_p->backgroundColor);
                        }
                    }
                });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->activeWidget = value;
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->viewPosObserver = ValueObserver<glm::vec2>::create(
                                    widget->_p->activeWidget->getImageView()->observeImagePos(),
                                    [weak](const glm::vec2& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewPos = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->viewZoomObserver = ValueObserver<float>::create(
                                    widget->_p->activeWidget->getImageView()->observeImageZoom(),
                                    [weak](float value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewZoom = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->gridOptionsObserver = ValueObserver<GridOptions>::create(
                                    widget->_p->activeWidget->getImageView()->observeGridOptions(),
                                    [weak](const GridOptions& value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_p->gridOptions = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_p->hudOptionsObserver = ValueObserver<HUDOptions>::create(
                                    widget->_p->activeWidget->observeHUDOptions(),
                                    [weak](const HUDOptions& value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_p->hudOptions = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_p->backgroundColorObserver = ValueObserver<AV::Image::Color>::create(
                                    widget->_p->activeWidget->getImageView()->observeBackgroundColor(),
                                    [weak](const AV::Image::Color& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->backgroundColor = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->viewPosObserver.reset();
                                widget->_p->viewZoomObserver.reset();
                                widget->_p->gridOptionsObserver.reset();
                                widget->_p->hudOptionsObserver.reset();
                                widget->_p->backgroundColorObserver.reset();
                            }
                        }
                    });
            }
        }

        ViewControlsWidget::ViewControlsWidget() :
            _p(new Private)
        {}

        ViewControlsWidget::~ViewControlsWidget()
        {}

        std::shared_ptr<ViewControlsWidget> ViewControlsWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ViewControlsWidget>(new ViewControlsWidget);
            out->_init(context);
            return out;
        }
        
        int ViewControlsWidget::getCurrentTab() const
        {
            return _p->tabWidget->getCurrentTab();
        }

        void ViewControlsWidget::setCurrentTab(int value)
        {
            _p->tabWidget->setCurrentTab(value);
        }
        
        void ViewControlsWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();

            setTitle(_getText(DJV_TEXT("view_controls")));
            
            p.viewPosResetButton->setTooltip(_getText(DJV_TEXT("reset_the_value")));
            p.viewZoomResetButton->setTooltip(_getText(DJV_TEXT("reset_the_value")));
            p.viewFormLayout->setText(p.viewPosLayout, _getText(DJV_TEXT("position")) + ":");
            p.viewFormLayout->setText(p.viewZoomLayout, _getText(DJV_TEXT("zoom")) + ":");

            p.gridFormLayout->setText(p.gridEnabledCheckBox, _getText(DJV_TEXT("widget_view_grid_enabled")) + ":");
            p.gridFormLayout->setText(p.gridSizeSlider, _getText(DJV_TEXT("widget_view_grid_size")) + ":");
            p.gridFormLayout->setText(p.gridColorPickerSwatch, _getText(DJV_TEXT("widget_view_grid_color")) + ":");
            p.gridFormLayout->setText(p.gridLabelsComboBox, _getText(DJV_TEXT("widget_view_grid_labels")) + ":");
            p.gridFormLayout->setText(p.gridLabelsColorPickerSwatch, _getText(DJV_TEXT("widget_view_grid_labels_color")) + ":");

            p.hudFormLayout->setText(p.hudEnabledCheckBox, _getText(DJV_TEXT("widget_view_hud_enabled")) + ":");
            p.hudFormLayout->setText(p.hudColorPickerSwatch, _getText(DJV_TEXT("widget_view_hud_color")) + ":");
            p.hudFormLayout->setText(p.hudBackgroundComboBox, _getText(DJV_TEXT("widget_view_hud_background")) + ":");

            p.backgroundFormLayout->setText(p.backgroundColorPickerSwatch, _getText(DJV_TEXT("widget_view_background_color")) + ":");
            
            p.tabWidget->setText(p.viewScrollWidget, _getText(DJV_TEXT("view")));
            p.tabWidget->setText(p.gridScrollWidget, _getText(DJV_TEXT("view_grid")));
            p.tabWidget->setText(p.hudScrollWidget, _getText(DJV_TEXT("view_hud")));
            p.tabWidget->setText(p.backgroundScrollWidget, _getText(DJV_TEXT("view_background")));
            
            _widgetUpdate();
        }

        void ViewControlsWidget::_setPos(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                viewSettings->setLock(ImageViewLock::None);
                p.viewPos = value;
                _widgetUpdate();
                if (p.activeWidget)
                {
                    p.activeWidget->getImageView()->setImagePos(p.viewPos);
                }
            }
        }

        void ViewControlsWidget::_setZoom(float value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                viewSettings->setLock(ImageViewLock::None);
                p.viewZoom = value;
                _widgetUpdate();
                if (p.activeWidget)
                {
                    p.activeWidget->getImageView()->setImageZoomFocus(value);
                }
            }
        }

        void ViewControlsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();

            p.viewPosEdit[0]->setValue(p.viewPos.x);
            p.viewPosEdit[1]->setValue(p.viewPos.y);
            p.viewPosResetButton->setEnabled(p.viewPos != glm::vec2(0.F, 0.F));
            p.viewZoomEdit->setValue(p.viewZoom);
            p.viewZoomResetButton->setEnabled(p.viewZoom != 1.F);

            p.gridEnabledCheckBox->setChecked(p.gridOptions.enabled);
            p.gridSizeSlider->setValue(p.gridOptions.size);
            p.gridColorPickerSwatch->setColor(p.gridOptions.color);
            std::vector<std::string> items;
            for (auto i : getImageViewGridLabelsEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.gridLabelsComboBox->setItems(items);
            p.gridLabelsComboBox->setCurrentItem(static_cast<int>(p.gridOptions.labels));
            p.gridLabelsColorPickerSwatch->setColor(p.gridOptions.labelsColor);

            p.hudEnabledCheckBox->setChecked(p.hudOptions.enabled);
            p.hudColorPickerSwatch->setColor(p.hudOptions.color);
            items.clear();
            for (auto i : getHUDBackgroundEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.hudBackgroundComboBox->setItems(items);
            p.hudBackgroundComboBox->setCurrentItem(static_cast<int>(p.hudOptions.background));

            p.backgroundColorPickerSwatch->setColor(p.backgroundColor);
        }

    } // namespace ViewApp
} // namespace djv

