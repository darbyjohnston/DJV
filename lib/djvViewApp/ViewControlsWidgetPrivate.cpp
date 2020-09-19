// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewControlsWidgetPrivate.h>

#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/View.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/ViewWidget.h>
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

#include <djvMath/NumericValueModels.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewControlsViewWidget::Private
        {
            std::shared_ptr<MediaWidget> activeWidget;
            glm::vec2 viewPos = glm::vec2(0.F, 0.F);
            float viewZoom = 1.F;
            std::shared_ptr<UI::FloatEdit> viewPosEdit[2];
            std::shared_ptr<UI::ToolButton> viewPosResetButton[2];
            std::shared_ptr<UI::FloatEdit> viewZoomEdit;
            std::shared_ptr<UI::ToolButton> viewZoomResetButton;
            std::shared_ptr<UI::HorizontalLayout> viewPosLayout[2];
            std::shared_ptr<UI::HorizontalLayout> viewZoomLayout;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<glm::vec2> > viewPosObserver;
            std::shared_ptr<ValueObserver<float> > viewZoomObserver;
        };

        void ViewControlsViewWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            for (size_t i = 0; i < 2; ++i)
            {
                auto edit = UI::FloatEdit::create(context);
                edit->setRange(Math::FloatRange(-1000000.F, 1000000.F));
                edit->setSmallIncrement(1.F);
                edit->setLargeIncrement(10.F);
                p.viewPosEdit[i] = edit;

                p.viewPosResetButton[i] = UI::ToolButton::create(context);
                p.viewPosResetButton[i]->setIcon("djvIconClearSmall");
                p.viewPosResetButton[i]->setInsideMargin(UI::MetricsRole::None);
            }
            p.viewZoomEdit = UI::FloatEdit::create(context);
            p.viewZoomEdit->setRange(Math::FloatRange(.1F, 1000.F));
            p.viewZoomEdit->setSmallIncrement(.1F);
            p.viewZoomEdit->setLargeIncrement(1.F);
            p.viewZoomResetButton = UI::ToolButton::create(context);
            p.viewZoomResetButton->setIcon("djvIconClearSmall");
            p.viewZoomResetButton->setInsideMargin(UI::MetricsRole::None);

            p.layout = UI::FormLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::MarginSmall);
            for (size_t i = 0; i < 2; ++i)
            {
                p.viewPosLayout[i] = UI::HorizontalLayout::create(context);
                p.viewPosLayout[i]->setSpacing(UI::MetricsRole::None);
                p.viewPosLayout[i]->addChild(p.viewPosEdit[i]);
                p.viewPosLayout[i]->setStretch(p.viewPosEdit[i], UI::RowStretch::Expand);
                p.viewPosLayout[i]->addChild(p.viewPosResetButton[i]);
                p.layout->addChild(p.viewPosLayout[i]);
            }
            p.viewZoomLayout = UI::HorizontalLayout::create(context);
            p.viewZoomLayout->setSpacing(UI::MetricsRole::None);
            p.viewZoomLayout->addChild(p.viewZoomEdit);
            p.viewZoomLayout->setStretch(p.viewZoomEdit, UI::RowStretch::Expand);
            p.viewZoomLayout->addChild(p.viewZoomResetButton);
            p.layout->addChild(p.viewZoomLayout);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewControlsViewWidget>(std::dynamic_pointer_cast<ViewControlsViewWidget>(shared_from_this()));
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

            p.viewPosResetButton[0]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        glm::vec2 pos = widget->_p->viewPos;
                        pos.x = 0.F;
                        widget->_setPos(pos);
                    }
                });
            p.viewPosResetButton[1]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        glm::vec2 pos = widget->_p->viewPos;
                        pos.y = 0.F;
                        widget->_setPos(pos);
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
                                    widget->_p->activeWidget->getViewWidget()->observeImagePos(),
                                    [weak](const glm::vec2& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewPos = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->viewZoomObserver = ValueObserver<float>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeImageZoom(),
                                    [weak](float value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewZoom = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->viewPosObserver.reset();
                                widget->_p->viewZoomObserver.reset();
                            }
                        }
                    });
            }
        }

        ViewControlsViewWidget::ViewControlsViewWidget() :
            _p(new Private)
        {}

        ViewControlsViewWidget::~ViewControlsViewWidget()
        {}

        std::shared_ptr<ViewControlsViewWidget> ViewControlsViewWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewControlsViewWidget>(new ViewControlsViewWidget);
            out->_init(context);
            return out;
        }

        void ViewControlsViewWidget::setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void ViewControlsViewWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewControlsViewWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewControlsViewWidget::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                for (size_t i = 0; i < 2; ++i)
                {
                    p.viewPosResetButton[i]->setTooltip(_getText(DJV_TEXT("reset_the_value")));
                }
                p.viewZoomResetButton->setTooltip(_getText(DJV_TEXT("reset_the_value")));
                p.layout->setText(p.viewPosLayout[0], _getText(DJV_TEXT("position_x")) + ":");
                p.layout->setText(p.viewPosLayout[1], _getText(DJV_TEXT("position_y")) + ":");
                p.layout->setText(p.viewZoomLayout, _getText(DJV_TEXT("zoom")) + ":");
            }
        }

        void ViewControlsViewWidget::_setPos(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                viewSettings->setLock(ViewLock::None);
                p.viewPos = value;
                _widgetUpdate();
                if (p.activeWidget)
                {
                    p.activeWidget->getViewWidget()->setImagePos(p.viewPos);
                }
            }
        }

        void ViewControlsViewWidget::_setZoom(float value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                viewSettings->setLock(ViewLock::None);
                p.viewZoom = value;
                _widgetUpdate();
                if (p.activeWidget)
                {
                    p.activeWidget->getViewWidget()->setImageZoomFocus(value);
                }
            }
        }

        void ViewControlsViewWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.viewPosEdit[0]->setValue(p.viewPos.x);
            p.viewPosEdit[1]->setValue(p.viewPos.y);
            p.viewPosResetButton[0]->setEnabled(p.viewPos.x != 0.F);
            p.viewPosResetButton[1]->setEnabled(p.viewPos.y != 0.F);
            p.viewZoomEdit->setValue(p.viewZoom);
            p.viewZoomResetButton->setEnabled(p.viewZoom != 1.F);
        }

        struct ViewControlsGridWidget::Private
        {
            GridOptions gridOptions;
            std::shared_ptr<MediaWidget> activeWidget;
            std::shared_ptr<UI::ToolButton> gridEnabledButton;
            std::shared_ptr<UI::IntSlider> gridSizeSlider;
            std::shared_ptr<UI::ColorPickerSwatch> gridColorPickerSwatch;
            std::shared_ptr<UI::ComboBox> gridLabelsComboBox;
            std::shared_ptr<UI::ColorPickerSwatch> gridLabelsColorPickerSwatch;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<GridOptions> > gridOptionsObserver;
        };

        void ViewControlsGridWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.gridOptions = viewSettings->observeGridOptions()->get();

            p.gridEnabledButton = UI::ToolButton::create(context);
            p.gridEnabledButton->setButtonType(UI::ButtonType::Toggle);
            p.gridEnabledButton->setIcon("djvIconHidden");
            p.gridEnabledButton->setCheckedIcon("djvIconVisible");
            p.gridEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.gridSizeSlider = UI::IntSlider::create(context);
            p.gridSizeSlider->setRange(Math::IntRange(1, 500));
            p.gridSizeSlider->setSmallIncrement(1);
            p.gridSizeSlider->setLargeIncrement(10);
            p.gridColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.gridColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.gridColorPickerSwatch->setHAlign(UI::HAlign::Fill);
            p.gridLabelsComboBox = UI::ComboBox::create(context);
            p.gridLabelsColorPickerSwatch = UI::ColorPickerSwatch::create(context);
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

            auto weak = std::weak_ptr<ViewControlsGridWidget>(std::dynamic_pointer_cast<ViewControlsGridWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.gridEnabledButton->setCheckedCallback(
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
                                widget->_p->activeWidget->getViewWidget()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
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
                                widget->_p->activeWidget->getViewWidget()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
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
                            widget->_p->gridOptions.color = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
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
                            widget->_p->gridOptions.labels = static_cast<GridLabels>(value);
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
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
                            widget->_p->gridOptions.labelsColor = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
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
                                widget->_p->gridOptionsObserver = ValueObserver<GridOptions>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeGridOptions(),
                                    [weak](const GridOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->gridOptions = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->gridOptionsObserver.reset();
                            }
                        }
                    });
            }
        }

        ViewControlsGridWidget::ViewControlsGridWidget() :
            _p(new Private)
        {}

        ViewControlsGridWidget::~ViewControlsGridWidget()
        {}

        std::shared_ptr<ViewControlsGridWidget> ViewControlsGridWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewControlsGridWidget>(new ViewControlsGridWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<UI::ToolButton>& ViewControlsGridWidget::getEnabledButton() const
        {
            return _p->gridEnabledButton;
        }

        void ViewControlsGridWidget::setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void ViewControlsGridWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewControlsGridWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewControlsGridWidget::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.gridEnabledButton->setTooltip(_getText(DJV_TEXT("widget_view_grid_enabled_tooltip")));
                p.layout->setText(p.gridSizeSlider, _getText(DJV_TEXT("widget_view_grid_size")) + ":");
                p.layout->setText(p.gridColorPickerSwatch, _getText(DJV_TEXT("widget_view_grid_color")) + ":");
                p.layout->setText(p.gridLabelsComboBox, _getText(DJV_TEXT("widget_view_grid_labels")) + ":");
                p.layout->setText(p.gridLabelsColorPickerSwatch, _getText(DJV_TEXT("widget_view_grid_labels_color")) + ":");
            }
        }

        void ViewControlsGridWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.gridEnabledButton->setChecked(p.gridOptions.enabled);
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

        struct ViewControlsHUDWidget::Private
        {
            HUDOptions hudOptions;
            std::shared_ptr<UI::ToolButton> hudEnabledButton;
            std::shared_ptr<MediaWidget> activeWidget;
            std::shared_ptr<UI::ColorPickerSwatch> hudColorPickerSwatch;
            std::shared_ptr<UI::ComboBox> hudBackgroundComboBox;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<HUDOptions> > hudOptionsObserver;
        };

        void ViewControlsHUDWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.hudOptions = viewSettings->observeHUDOptions()->get();

            p.hudEnabledButton = UI::ToolButton::create(context);
            p.hudEnabledButton->setButtonType(UI::ButtonType::Toggle);
            p.hudEnabledButton->setIcon("djvIconHidden");
            p.hudEnabledButton->setCheckedIcon("djvIconVisible");
            p.hudEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.hudColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.hudColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.hudColorPickerSwatch->setHAlign(UI::HAlign::Fill);
            p.hudBackgroundComboBox = UI::ComboBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::MarginSmall);
            p.layout->addChild(p.hudColorPickerSwatch);
            p.layout->addChild(p.hudBackgroundComboBox);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewControlsHUDWidget>(std::dynamic_pointer_cast<ViewControlsHUDWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.hudEnabledButton->setCheckedCallback(
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
                                widget->_p->activeWidget->getViewWidget()->setHUDOptions(widget->_p->hudOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setHUDOptions(widget->_p->hudOptions);
                        }
                    }
                });
            p.hudColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->hudOptions.color = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setHUDOptions(widget->_p->hudOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
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
                                widget->_p->activeWidget->getViewWidget()->setHUDOptions(widget->_p->hudOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setHUDOptions(widget->_p->hudOptions);
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
                                widget->_p->hudOptionsObserver = ValueObserver<HUDOptions>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeHUDOptions(),
                                    [weak](const HUDOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->hudOptions = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->hudOptionsObserver.reset();
                            }
                        }
                    });
            }
        }

        ViewControlsHUDWidget::ViewControlsHUDWidget() :
            _p(new Private)
        {}

        ViewControlsHUDWidget::~ViewControlsHUDWidget()
        {}

        std::shared_ptr<ViewControlsHUDWidget> ViewControlsHUDWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewControlsHUDWidget>(new ViewControlsHUDWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<UI::ToolButton>& ViewControlsHUDWidget::getEnabledButton() const
        {
            return _p->hudEnabledButton;
        }

        void ViewControlsHUDWidget::setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void ViewControlsHUDWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewControlsHUDWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewControlsHUDWidget::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.hudEnabledButton->setTooltip(_getText(DJV_TEXT("widget_view_hud_enabled_tooltip")));
                p.layout->setText(p.hudColorPickerSwatch, _getText(DJV_TEXT("widget_view_hud_color")) + ":");
                p.layout->setText(p.hudBackgroundComboBox, _getText(DJV_TEXT("widget_view_hud_background")) + ":");
            }
        }

        void ViewControlsHUDWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.hudEnabledButton->setChecked(p.hudOptions.enabled);
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

        struct ViewControlsBackgroundWidget::Private
        {
            ViewBackgroundOptions backgroundOptions;
            std::shared_ptr<MediaWidget> activeWidget;
            std::shared_ptr<UI::ComboBox> backgroundComboBox;
            std::shared_ptr<UI::ColorPickerSwatch> solidPickerSwatch;
            std::shared_ptr<UI::FloatSlider> checkersSizeSlider;
            std::shared_ptr<UI::ColorPickerSwatch> checkersColorPickerSwatches[2];
            std::shared_ptr<UI::HorizontalLayout> checkersColorsLayout;
            std::shared_ptr<UI::FormLayout> backgroundLayout;
            std::shared_ptr<UI::FormLayout> solidLayout;
            std::shared_ptr<UI::FormLayout> checkersLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<HUDOptions> > hudOptionsObserver;
            std::shared_ptr<ValueObserver<ViewBackgroundOptions> > backgroundOptionsObserver;
        };

        void ViewControlsBackgroundWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.backgroundOptions = viewSettings->observeBackgroundOptions()->get();

            p.backgroundComboBox = UI::ComboBox::create(context);
            p.solidPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.solidPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.solidPickerSwatch->setHAlign(UI::HAlign::Fill);
            p.checkersSizeSlider = UI::FloatSlider::create(context);
            p.checkersSizeSlider->setRange(Math::FloatRange(10.F, 100.F));
            p.checkersSizeSlider->setSmallIncrement(1);
            p.checkersSizeSlider->setLargeIncrement(10);
            p.checkersColorPickerSwatches[0] = UI::ColorPickerSwatch::create(context);
            p.checkersColorPickerSwatches[0]->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.checkersColorPickerSwatches[0]->setHAlign(UI::HAlign::Fill);
            p.checkersColorPickerSwatches[1] = UI::ColorPickerSwatch::create(context);
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
            p.checkersColorsLayout->setStretch(p.checkersColorPickerSwatches[0], UI::RowStretch::Expand);
            p.checkersColorsLayout->addChild(p.checkersColorPickerSwatches[1]);
            p.checkersColorsLayout->setStretch(p.checkersColorPickerSwatches[1], UI::RowStretch::Expand);
            p.checkersLayout->addChild(p.checkersColorsLayout);
            p.layout->addChild(p.checkersLayout);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewControlsBackgroundWidget>(std::dynamic_pointer_cast<ViewControlsBackgroundWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.backgroundComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->backgroundOptions.background = static_cast<ViewBackground>(value);
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
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
                            widget->_p->backgroundOptions.color = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
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
                            widget->_p->backgroundOptions.checkersSize = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
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
                            widget->_p->backgroundOptions.checkersColors[0] = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
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
                            widget->_p->backgroundOptions.checkersColors[1] = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
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
                                widget->_p->backgroundOptionsObserver = ValueObserver<ViewBackgroundOptions>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeBackgroundOptions(),
                                    [weak](const ViewBackgroundOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->backgroundOptions = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->backgroundOptionsObserver.reset();
                            }
                        }
                    });
            }
        }

        ViewControlsBackgroundWidget::ViewControlsBackgroundWidget() :
            _p(new Private)
        {}

        ViewControlsBackgroundWidget::~ViewControlsBackgroundWidget()
        {}

        std::shared_ptr<ViewControlsBackgroundWidget> ViewControlsBackgroundWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewControlsBackgroundWidget>(new ViewControlsBackgroundWidget);
            out->_init(context);
            return out;
        }

        void ViewControlsBackgroundWidget::setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
        {
            DJV_PRIVATE_PTR();
            p.backgroundLayout->setLabelSizeGroup(value);
            p.solidLayout->setLabelSizeGroup(value);
            p.checkersLayout->setLabelSizeGroup(value);
        }

        void ViewControlsBackgroundWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewControlsBackgroundWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewControlsBackgroundWidget::_initEvent(System::Event::Init& event)
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

        void ViewControlsBackgroundWidget::_widgetUpdate()
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

        struct ViewControlsBorderWidget::Private
        {
            ViewBackgroundOptions backgroundOptions;
            std::shared_ptr<MediaWidget> activeWidget;
            std::shared_ptr<UI::ToolButton> borderEnabledButton;
            std::shared_ptr<UI::FloatSlider> borderWidthSlider;
            std::shared_ptr<UI::ColorPickerSwatch> borderColorPickerSwatch;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<ViewBackgroundOptions> > backgroundOptionsObserver;
        };

        void ViewControlsBorderWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            p.borderEnabledButton = UI::ToolButton::create(context);
            p.borderEnabledButton->setButtonType(UI::ButtonType::Toggle);
            p.borderEnabledButton->setIcon("djvIconHidden");
            p.borderEnabledButton->setCheckedIcon("djvIconVisible");
            p.borderEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.borderWidthSlider = UI::FloatSlider::create(context);
            p.borderWidthSlider->setRange(Math::FloatRange(1.F, 20.F));
            p.borderWidthSlider->setSmallIncrement(1.F);
            p.borderWidthSlider->setLargeIncrement(5.F);
            p.borderColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.borderColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.borderColorPickerSwatch->setHAlign(UI::HAlign::Fill);

            p.layout = UI::FormLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::MarginSmall);
            p.layout->addChild(p.borderWidthSlider);
            p.layout->addChild(p.borderColorPickerSwatch);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewControlsBorderWidget>(std::dynamic_pointer_cast<ViewControlsBorderWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.borderEnabledButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->backgroundOptions.border = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
                        }
                    }
                });
            p.borderWidthSlider->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->backgroundOptions.borderWidth = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
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
                            widget->_p->backgroundOptions.borderColor = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
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
                                widget->_p->backgroundOptionsObserver = ValueObserver<ViewBackgroundOptions>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeBackgroundOptions(),
                                    [weak](const ViewBackgroundOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->backgroundOptions = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->backgroundOptionsObserver.reset();
                            }
                        }
                    });
            }
        }

        ViewControlsBorderWidget::ViewControlsBorderWidget() :
            _p(new Private)
        {}

        ViewControlsBorderWidget::~ViewControlsBorderWidget()
        {}

        std::shared_ptr<ViewControlsBorderWidget> ViewControlsBorderWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewControlsBorderWidget>(new ViewControlsBorderWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<UI::ToolButton>& ViewControlsBorderWidget::getEnabledButton() const
        {
            return _p->borderEnabledButton;
        }

        void ViewControlsBorderWidget::setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void ViewControlsBorderWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewControlsBorderWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewControlsBorderWidget::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.borderEnabledButton->setTooltip(_getText(DJV_TEXT("widget_view_border_enabled_tooltip")));
                p.layout->setText(p.borderWidthSlider, _getText(DJV_TEXT("widget_view_border_width")) + ":");
                p.layout->setText(p.borderColorPickerSwatch, _getText(DJV_TEXT("widget_view_border_color")) + ":");
            }
        }

        void ViewControlsBorderWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.borderEnabledButton->setChecked(p.backgroundOptions.border);
            p.borderWidthSlider->setValue(p.backgroundOptions.borderWidth);
            p.borderColorPickerSwatch->setColor(p.backgroundOptions.borderColor);
        }

    } // namespace ViewApp
} // namespace djv

