// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewControlsWidget.h>

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

#include <djvCore/Context.h>
#include <djvCore/NumericValueModels.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class PosAndZoomWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(PosAndZoomWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                PosAndZoomWidget();

            public:
                ~PosAndZoomWidget() override;

                static std::shared_ptr<PosAndZoomWidget> create(const std::shared_ptr<Context>&);

                void setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _setPos(const glm::vec2&);
                void _setZoom(float);
                void _widgetUpdate();

                std::shared_ptr<MediaWidget> _activeWidget;
                glm::vec2 _viewPos = glm::vec2(0.F, 0.F);
                float _viewZoom = 1.F;
                std::shared_ptr<UI::FloatEdit> _viewPosEdit[2];
                std::shared_ptr<UI::ToolButton> _viewPosResetButton[2];
                std::shared_ptr<UI::FloatEdit> _viewZoomEdit;
                std::shared_ptr<UI::ToolButton> _viewZoomResetButton;
                std::shared_ptr<UI::HorizontalLayout> _viewPosLayout[2];
                std::shared_ptr<UI::HorizontalLayout> _viewZoomLayout;
                std::shared_ptr<UI::FormLayout> _layout;
                std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > _activeWidgetObserver;
                std::shared_ptr<ValueObserver<glm::vec2> > _viewPosObserver;
                std::shared_ptr<ValueObserver<float> > _viewZoomObserver;
            };

            void PosAndZoomWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                for (size_t i = 0; i < 2; ++i)
                {
                    auto edit = UI::FloatEdit::create(context);
                    edit->setRange(FloatRange(-1000000.F, 1000000.F));
                    edit->setSmallIncrement(1.F);
                    edit->setLargeIncrement(10.F);
                    _viewPosEdit[i] = edit;

                    _viewPosResetButton[i] = UI::ToolButton::create(context);
                    _viewPosResetButton[i]->setIcon("djvIconClearSmall");
                    _viewPosResetButton[i]->setInsideMargin(UI::MetricsRole::None);
                }
                _viewZoomEdit = UI::FloatEdit::create(context);
                _viewZoomEdit->setRange(FloatRange(.1F, 1000.F));
                _viewZoomEdit->setSmallIncrement(.1F);
                _viewZoomEdit->setLargeIncrement(1.F);
                _viewZoomResetButton = UI::ToolButton::create(context);
                _viewZoomResetButton->setIcon("djvIconClearSmall");
                _viewZoomResetButton->setInsideMargin(UI::MetricsRole::None);

                _layout = UI::FormLayout::create(context);
                _layout->setMargin(UI::MetricsRole::MarginSmall);
                for (size_t i = 0; i < 2; ++i)
                {
                    _viewPosLayout[i] = UI::HorizontalLayout::create(context);
                    _viewPosLayout[i]->setSpacing(UI::MetricsRole::None);
                    _viewPosLayout[i]->addChild(_viewPosEdit[i]);
                    _viewPosLayout[i]->addChild(_viewPosResetButton[i]);
                    _layout->addChild(_viewPosLayout[i]);
                }
                _viewZoomLayout = UI::HorizontalLayout::create(context);
                _viewZoomLayout->setSpacing(UI::MetricsRole::None);
                _viewZoomLayout->addChild(_viewZoomEdit);
                _viewZoomLayout->addChild(_viewZoomResetButton);
                _layout->addChild(_viewZoomLayout);
                addChild(_layout);

                _widgetUpdate();

                auto weak = std::weak_ptr<PosAndZoomWidget>(std::dynamic_pointer_cast<PosAndZoomWidget>(shared_from_this()));
                _viewPosEdit[0]->setValueCallback(
                    [weak](float value, UI::TextEditReason)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_setPos(glm::vec2(value, widget->_viewPos.y));
                        }
                    });
                _viewPosEdit[1]->setValueCallback(
                    [weak](float value, UI::TextEditReason)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_setPos(glm::vec2(widget->_viewPos.x, value));
                        }
                    });

                _viewPosResetButton[0]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            glm::vec2 pos = widget->_viewPos;
                            pos.x = 0.F;
                            widget->_setPos(pos);
                        }
                    });
                _viewPosResetButton[1]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            glm::vec2 pos = widget->_viewPos;
                            pos.y = 0.F;
                            widget->_setPos(pos);
                        }
                    });

                _viewZoomEdit->setValueCallback(
                    [weak](float value, UI::TextEditReason)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_setZoom(value);
                        }
                    });

                _viewZoomResetButton->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_setZoom(1.F);
                        }
                    });

                if (auto windowSystem = context->getSystemT<WindowSystem>())
                {
                    _activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                        windowSystem->observeActiveWidget(),
                        [weak](const std::shared_ptr<MediaWidget>& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_activeWidget = value;
                                if (widget->_activeWidget)
                                {
                                    widget->_viewPosObserver = ValueObserver<glm::vec2>::create(
                                        widget->_activeWidget->getViewWidget()->observeImagePos(),
                                        [weak](const glm::vec2& value)
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                widget->_viewPos = value;
                                                widget->_widgetUpdate();
                                            }
                                        });
                                    widget->_viewZoomObserver = ValueObserver<float>::create(
                                        widget->_activeWidget->getViewWidget()->observeImageZoom(),
                                        [weak](float value)
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                widget->_viewZoom = value;
                                                widget->_widgetUpdate();
                                            }
                                        });
                                }
                                else
                                {
                                    widget->_viewPosObserver.reset();
                                    widget->_viewZoomObserver.reset();
                                }
                            }
                        });
                }
            }

            PosAndZoomWidget::PosAndZoomWidget()
            {}

            PosAndZoomWidget::~PosAndZoomWidget()
            {}

            std::shared_ptr<PosAndZoomWidget> PosAndZoomWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<PosAndZoomWidget>(new PosAndZoomWidget);
                out->_init(context);
                return out;
            }

            void PosAndZoomWidget::setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
            {
                _layout->setLabelSizeGroup(value);
            }

            void PosAndZoomWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void PosAndZoomWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void PosAndZoomWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    for (size_t i = 0; i < 2; ++i)
                    {
                        _viewPosResetButton[i]->setTooltip(_getText(DJV_TEXT("reset_the_value")));
                    }
                    _viewZoomResetButton->setTooltip(_getText(DJV_TEXT("reset_the_value")));
                    _layout->setText(_viewPosLayout[0], _getText(DJV_TEXT("position_x")) + ":");
                    _layout->setText(_viewPosLayout[1], _getText(DJV_TEXT("position_y")) + ":");
                    _layout->setText(_viewZoomLayout, _getText(DJV_TEXT("zoom")) + ":");
                }
            }

            void PosAndZoomWidget::_setPos(const glm::vec2& value)
            {
                if (auto context = getContext().lock())
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                    viewSettings->setLock(ViewLock::None);
                    _viewPos = value;
                    _widgetUpdate();
                    if (_activeWidget)
                    {
                        _activeWidget->getViewWidget()->setImagePos(_viewPos);
                    }
                }
            }

            void PosAndZoomWidget::_setZoom(float value)
            {
                if (auto context = getContext().lock())
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                    viewSettings->setLock(ViewLock::None);
                    _viewZoom = value;
                    _widgetUpdate();
                    if (_activeWidget)
                    {
                        _activeWidget->getViewWidget()->setImageZoomFocus(value);
                    }
                }
            }

            void PosAndZoomWidget::_widgetUpdate()
            {
                _viewPosEdit[0]->setValue(_viewPos.x);
                _viewPosEdit[1]->setValue(_viewPos.y);
                _viewPosResetButton[0]->setEnabled(_viewPos.x != 0.F);
                _viewPosResetButton[1]->setEnabled(_viewPos.y != 0.F);
                _viewZoomEdit->setValue(_viewZoom);
                _viewZoomResetButton->setEnabled(_viewZoom != 1.F);
            }

            class GridWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(GridWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                GridWidget();

            public:
                ~GridWidget() override;

                static std::shared_ptr<GridWidget> create(const std::shared_ptr<Context>&);

                const std::shared_ptr<UI::ToolButton>& getEnabledButton() const;

                void setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                GridOptions _gridOptions;
                std::shared_ptr<MediaWidget> _activeWidget;
                std::shared_ptr<UI::ToolButton> _gridEnabledButton;
                std::shared_ptr<UI::IntSlider> _gridSizeSlider;
                std::shared_ptr<UI::ColorPickerSwatch> _gridColorPickerSwatch;
                std::shared_ptr<UI::ComboBox> _gridLabelsComboBox;
                std::shared_ptr<UI::ColorPickerSwatch> _gridLabelsColorPickerSwatch;
                std::shared_ptr<UI::FormLayout> _layout;
                std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > _activeWidgetObserver;
                std::shared_ptr<ValueObserver<GridOptions> > _gridOptionsObserver;
            };

            void GridWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                _gridOptions = viewSettings->observeGridOptions()->get();

                _gridEnabledButton = UI::ToolButton::create(context);
                _gridEnabledButton->setButtonType(UI::ButtonType::Toggle);
                _gridEnabledButton->setIcon("djvIconHidden");
                _gridEnabledButton->setCheckedIcon("djvIconVisible");
                _gridEnabledButton->setInsideMargin(UI::MetricsRole::None);
                _gridSizeSlider = UI::IntSlider::create(context);
                _gridSizeSlider->setRange(IntRange(1, 500));
                _gridSizeSlider->setSmallIncrement(1);
                _gridSizeSlider->setLargeIncrement(10);
                _gridColorPickerSwatch = UI::ColorPickerSwatch::create(context);
                _gridColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
                _gridLabelsComboBox = UI::ComboBox::create(context);
                _gridLabelsColorPickerSwatch = UI::ColorPickerSwatch::create(context);
                _gridLabelsColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);

                _layout = UI::FormLayout::create(context);
                _layout->setMargin(UI::MetricsRole::MarginSmall);
                _layout->addChild(_gridSizeSlider);
                _layout->addChild(_gridColorPickerSwatch);
                _layout->addChild(_gridLabelsComboBox);
                _layout->addChild(_gridLabelsColorPickerSwatch);
                addChild(_layout);

                _widgetUpdate();

                auto weak = std::weak_ptr<GridWidget>(std::dynamic_pointer_cast<GridWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<Context>(context);
                _gridEnabledButton->setCheckedCallback(
                    [weak, contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_gridOptions.enabled = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setGridOptions(widget->_gridOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setGridOptions(widget->_gridOptions);
                            }
                        }
                    });
                _gridSizeSlider->setValueCallback(
                    [weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_gridOptions.size = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setGridOptions(widget->_gridOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setGridOptions(widget->_gridOptions);
                            }
                        }
                    });
                _gridColorPickerSwatch->setColorCallback(
                    [weak, contextWeak](const AV::Image::Color& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_gridOptions.color = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setGridOptions(widget->_gridOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setGridOptions(widget->_gridOptions);
                            }
                        }
                    });
                _gridLabelsComboBox->setCallback(
                    [weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_gridOptions.labels = static_cast<GridLabels>(value);
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setGridOptions(widget->_gridOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setGridOptions(widget->_gridOptions);
                            }
                        }
                    });
                _gridLabelsColorPickerSwatch->setColorCallback(
                    [weak, contextWeak](const AV::Image::Color& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_gridOptions.labelsColor = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setGridOptions(widget->_gridOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setGridOptions(widget->_gridOptions);
                            }
                        }
                    });

                if (auto windowSystem = context->getSystemT<WindowSystem>())
                {
                    _activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                        windowSystem->observeActiveWidget(),
                        [weak](const std::shared_ptr<MediaWidget>& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_activeWidget = value;
                                if (widget->_activeWidget)
                                {
                                    widget->_gridOptionsObserver = ValueObserver<GridOptions>::create(
                                        widget->_activeWidget->getViewWidget()->observeGridOptions(),
                                        [weak](const GridOptions& value)
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                widget->_gridOptions = value;
                                                widget->_widgetUpdate();
                                            }
                                        });
                                }
                                else
                                {
                                    widget->_gridOptionsObserver.reset();
                                }
                            }
                        });
                }
            }

            GridWidget::GridWidget()
            {}

            GridWidget::~GridWidget()
            {}

            std::shared_ptr<GridWidget> GridWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<GridWidget>(new GridWidget);
                out->_init(context);
                return out;
            }

            const std::shared_ptr<UI::ToolButton>& GridWidget::getEnabledButton() const
            {
                return _gridEnabledButton;
            }

            void GridWidget::setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
            {
                _layout->setLabelSizeGroup(value);
            }

            void GridWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void GridWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void GridWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _gridEnabledButton->setTooltip(_getText(DJV_TEXT("widget_view_grid_enabled_tooltip")));
                    _layout->setText(_gridSizeSlider, _getText(DJV_TEXT("widget_view_grid_size")) + ":");
                    _layout->setText(_gridColorPickerSwatch, _getText(DJV_TEXT("widget_view_grid_color")) + ":");
                    _layout->setText(_gridLabelsComboBox, _getText(DJV_TEXT("widget_view_grid_labels")) + ":");
                    _layout->setText(_gridLabelsColorPickerSwatch, _getText(DJV_TEXT("widget_view_grid_labels_color")) + ":");
                }
            }

            void GridWidget::_widgetUpdate()
            {
                _gridEnabledButton->setChecked(_gridOptions.enabled);
                _gridSizeSlider->setValue(_gridOptions.size);
                _gridColorPickerSwatch->setColor(_gridOptions.color);
                std::vector<std::string> items;
                for (auto i : getGridLabelsEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    items.push_back(_getText(ss.str()));
                }
                _gridLabelsComboBox->setItems(items);
                _gridLabelsComboBox->setCurrentItem(static_cast<int>(_gridOptions.labels));
                _gridLabelsColorPickerSwatch->setColor(_gridOptions.labelsColor);
            }

            class HUDWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(HUDWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                HUDWidget();

            public:
                ~HUDWidget() override;

                static std::shared_ptr<HUDWidget> create(const std::shared_ptr<Context>&);

                const std::shared_ptr<UI::ToolButton>& getEnabledButton() const;

                void setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                HUDOptions _hudOptions;
                std::shared_ptr<UI::ToolButton> _hudEnabledButton;
                std::shared_ptr<MediaWidget> _activeWidget;
                std::shared_ptr<UI::ColorPickerSwatch> _hudColorPickerSwatch;
                std::shared_ptr<UI::ComboBox> _hudBackgroundComboBox;
                std::shared_ptr<UI::FormLayout> _layout;
                std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > _activeWidgetObserver;
                std::shared_ptr<ValueObserver<HUDOptions> > _hudOptionsObserver;
            };

            void HUDWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                _hudOptions = viewSettings->observeHUDOptions()->get();

                _hudEnabledButton = UI::ToolButton::create(context);
                _hudEnabledButton->setButtonType(UI::ButtonType::Toggle);
                _hudEnabledButton->setIcon("djvIconHidden");
                _hudEnabledButton->setCheckedIcon("djvIconVisible");
                _hudEnabledButton->setInsideMargin(UI::MetricsRole::None);
                _hudColorPickerSwatch = UI::ColorPickerSwatch::create(context);
                _hudColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
                _hudBackgroundComboBox = UI::ComboBox::create(context);

                _layout = UI::FormLayout::create(context);
                _layout->setMargin(UI::MetricsRole::MarginSmall);
                _layout->addChild(_hudColorPickerSwatch);
                _layout->addChild(_hudBackgroundComboBox);
                addChild(_layout);

                _widgetUpdate();

                auto weak = std::weak_ptr<HUDWidget>(std::dynamic_pointer_cast<HUDWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<Context>(context);
                _hudEnabledButton->setCheckedCallback(
                    [weak, contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_hudOptions.enabled = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->setHUDOptions(widget->_hudOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setHUDOptions(widget->_hudOptions);
                            }
                        }
                    });
                _hudColorPickerSwatch->setColorCallback(
                    [weak, contextWeak](const AV::Image::Color& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_hudOptions.color = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->setHUDOptions(widget->_hudOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setHUDOptions(widget->_hudOptions);
                            }
                        }
                    });
                _hudBackgroundComboBox->setCallback(
                    [weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_hudOptions.background = static_cast<HUDBackground>(value);
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->setHUDOptions(widget->_hudOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setHUDOptions(widget->_hudOptions);
                            }
                        }
                    });

                if (auto windowSystem = context->getSystemT<WindowSystem>())
                {
                    _activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                        windowSystem->observeActiveWidget(),
                        [weak](const std::shared_ptr<MediaWidget>& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_activeWidget = value;
                                if (widget->_activeWidget)
                                {
                                    widget->_hudOptionsObserver = ValueObserver<HUDOptions>::create(
                                        widget->_activeWidget->observeHUDOptions(),
                                        [weak](const HUDOptions& value)
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                widget->_hudOptions = value;
                                                widget->_widgetUpdate();
                                            }
                                        });
                                }
                                else
                                {
                                    widget->_hudOptionsObserver.reset();
                                }
                            }
                        });
                }
            }

            HUDWidget::HUDWidget()
            {}

            HUDWidget::~HUDWidget()
            {}

            std::shared_ptr<HUDWidget> HUDWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<HUDWidget>(new HUDWidget);
                out->_init(context);
                return out;
            }

            const std::shared_ptr<UI::ToolButton>& HUDWidget::getEnabledButton() const
            {
                return _hudEnabledButton;
            }

            void HUDWidget::setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
            {
                _layout->setLabelSizeGroup(value);
            }

            void HUDWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void HUDWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void HUDWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _hudEnabledButton->setTooltip(_getText(DJV_TEXT("widget_view_hud_enabled_tooltip")));
                    _layout->setText(_hudColorPickerSwatch, _getText(DJV_TEXT("widget_view_hud_color")) + ":");
                    _layout->setText(_hudBackgroundComboBox, _getText(DJV_TEXT("widget_view_hud_background")) + ":");
                }
            }

            void HUDWidget::_widgetUpdate()
            {
                _hudEnabledButton->setChecked(_hudOptions.enabled);
                _hudColorPickerSwatch->setColor(_hudOptions.color);
                std::vector<std::string> items;
                for (auto i : getHUDBackgroundEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    items.push_back(_getText(ss.str()));
                }
                _hudBackgroundComboBox->setItems(items);
                _hudBackgroundComboBox->setCurrentItem(static_cast<int>(_hudOptions.background));
            }

            class BackgroundWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(BackgroundWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                BackgroundWidget();

            public:
                ~BackgroundWidget() override;

                static std::shared_ptr<BackgroundWidget> create(const std::shared_ptr<Context>&);

                void setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                ViewBackgroundOptions _backgroundOptions;
                std::shared_ptr<MediaWidget> _activeWidget;
                std::shared_ptr<UI::ComboBox> _backgroundComboBox;
                std::shared_ptr<UI::ColorPickerSwatch> _solidPickerSwatch;
                std::shared_ptr<UI::FloatSlider> _checkersSizeSlider;
                std::shared_ptr<UI::ColorPickerSwatch> _checkersColorPickerSwatches[2];
                std::shared_ptr<UI::HorizontalLayout> _checkersColorsLayout;
                std::shared_ptr<UI::FormLayout> _backgroundLayout;
                std::shared_ptr<UI::FormLayout> _solidLayout;
                std::shared_ptr<UI::FormLayout> _checkersLayout;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > _activeWidgetObserver;
                std::shared_ptr<ValueObserver<HUDOptions> > _hudOptionsObserver;
                std::shared_ptr<ValueObserver<ViewBackgroundOptions> > _backgroundOptionsObserver;
            };

            void BackgroundWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                _backgroundOptions = viewSettings->observeBackgroundOptions()->get();

                _backgroundComboBox = UI::ComboBox::create(context);
                _solidPickerSwatch = UI::ColorPickerSwatch::create(context);
                _solidPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
                _checkersSizeSlider = UI::FloatSlider::create(context);
                _checkersSizeSlider->setRange(FloatRange(10.F, 100.F));
                _checkersSizeSlider->setSmallIncrement(1);
                _checkersSizeSlider->setLargeIncrement(10);
                _checkersColorPickerSwatches[0] = UI::ColorPickerSwatch::create(context);
                _checkersColorPickerSwatches[0]->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
                _checkersColorPickerSwatches[0]->setHAlign(UI::HAlign::Fill);
                _checkersColorPickerSwatches[1] = UI::ColorPickerSwatch::create(context);
                _checkersColorPickerSwatches[1]->setHAlign(UI::HAlign::Fill);
                _checkersColorPickerSwatches[1]->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);

                _layout = UI::VerticalLayout::create(context);
                _layout->setMargin(UI::MetricsRole::MarginSmall);
                _layout->setSpacing(UI::MetricsRole::None);
                _backgroundLayout = UI::FormLayout::create(context);
                _backgroundLayout->addChild(_backgroundComboBox);
                _layout->addChild(_backgroundLayout);
                _solidLayout = UI::FormLayout::create(context);
                _solidLayout->addChild(_solidPickerSwatch);
                _layout->addChild(_solidLayout);
                _checkersLayout = UI::FormLayout::create(context);
                _checkersLayout->addChild(_checkersSizeSlider);
                _checkersColorsLayout = UI::HorizontalLayout::create(context);
                _checkersColorsLayout->setSpacing(UI::MetricsRole::SpacingSmall);
                _checkersColorsLayout->addChild(_checkersColorPickerSwatches[0]);
                _checkersColorsLayout->setStretch(_checkersColorPickerSwatches[0], UI::RowStretch::Expand);
                _checkersColorsLayout->addChild(_checkersColorPickerSwatches[1]);
                _checkersColorsLayout->setStretch(_checkersColorPickerSwatches[1], UI::RowStretch::Expand);
                _checkersLayout->addChild(_checkersColorsLayout);
                _layout->addChild(_checkersLayout);
                addChild(_layout);

                _widgetUpdate();

                auto weak = std::weak_ptr<BackgroundWidget>(std::dynamic_pointer_cast<BackgroundWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<Context>(context);
                _backgroundComboBox->setCallback(
                    [weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_backgroundOptions.background = static_cast<ViewBackground>(value);
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setBackgroundOptions(widget->_backgroundOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setBackgroundOptions(widget->_backgroundOptions);
                            }
                        }
                    });
                _solidPickerSwatch->setColorCallback(
                    [weak, contextWeak](const AV::Image::Color& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_backgroundOptions.color = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setBackgroundOptions(widget->_backgroundOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setBackgroundOptions(widget->_backgroundOptions);
                            }
                        }
                    });
                _checkersSizeSlider->setValueCallback(
                    [weak, contextWeak](float value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_backgroundOptions.checkersSize = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setBackgroundOptions(widget->_backgroundOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setBackgroundOptions(widget->_backgroundOptions);
                            }
                        }
                    });
                _checkersColorPickerSwatches[0]->setColorCallback(
                    [weak, contextWeak](const AV::Image::Color& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_backgroundOptions.checkersColors[0] = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setBackgroundOptions(widget->_backgroundOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setBackgroundOptions(widget->_backgroundOptions);
                            }
                        }
                    });
                _checkersColorPickerSwatches[1]->setColorCallback(
                    [weak, contextWeak](const AV::Image::Color& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_backgroundOptions.checkersColors[1] = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setBackgroundOptions(widget->_backgroundOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setBackgroundOptions(widget->_backgroundOptions);
                            }
                        }
                    });

                if (auto windowSystem = context->getSystemT<WindowSystem>())
                {
                    _activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                        windowSystem->observeActiveWidget(),
                        [weak](const std::shared_ptr<MediaWidget>& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_activeWidget = value;
                                if (widget->_activeWidget)
                                {
                                    widget->_backgroundOptionsObserver = ValueObserver<ViewBackgroundOptions>::create(
                                        widget->_activeWidget->getViewWidget()->observeBackgroundOptions(),
                                        [weak](const ViewBackgroundOptions& value)
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                widget->_backgroundOptions = value;
                                                widget->_widgetUpdate();
                                            }
                                        });
                                }
                                else
                                {
                                    widget->_backgroundOptionsObserver.reset();
                                }
                            }
                        });
                }
            }

            BackgroundWidget::BackgroundWidget()
            {}

            BackgroundWidget::~BackgroundWidget()
            {}

            std::shared_ptr<BackgroundWidget> BackgroundWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<BackgroundWidget>(new BackgroundWidget);
                out->_init(context);
                return out;
            }

            void BackgroundWidget::setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
            {
                _backgroundLayout->setLabelSizeGroup(value);
                _solidLayout->setLabelSizeGroup(value);
                _checkersLayout->setLabelSizeGroup(value);
            }

            void BackgroundWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void BackgroundWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void BackgroundWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _backgroundLayout->setText(_backgroundComboBox, _getText(DJV_TEXT("widget_view_background")) + ":");
                    _solidLayout->setText(_solidPickerSwatch, _getText(DJV_TEXT("widget_view_background_color")) + ":");
                    _checkersLayout->setText(_checkersSizeSlider, _getText(DJV_TEXT("widget_view_background_checkers_size")) + ":");
                    _checkersLayout->setText(_checkersColorsLayout, _getText(DJV_TEXT("widget_view_background_checkers_colors")) + ":");
                }
            }

            void BackgroundWidget::_widgetUpdate()
            {
                switch (_backgroundOptions.background)
                {
                case ViewBackground::Solid:
                    _solidLayout->show();
                    _checkersLayout->hide();
                    break;
                case ViewBackground::Checkers:
                    _solidLayout->hide();
                    _checkersLayout->show();
                    break;
                }
                std::vector<std::string> items;
                for (auto i : getViewBackgroundEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    items.push_back(_getText(ss.str()));
                }
                _backgroundComboBox->setItems(items);
                _backgroundComboBox->setCurrentItem(static_cast<int>(_backgroundOptions.background));
                _solidPickerSwatch->setColor(_backgroundOptions.color);
                _checkersSizeSlider->setValue(_backgroundOptions.checkersSize);
                _checkersColorPickerSwatches[0]->setColor(_backgroundOptions.checkersColors[0]);
                _checkersColorPickerSwatches[1]->setColor(_backgroundOptions.checkersColors[1]);
            }

            class BorderWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(BorderWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                BorderWidget();

            public:
                ~BorderWidget() override;

                static std::shared_ptr<BorderWidget> create(const std::shared_ptr<Context>&);

                const std::shared_ptr<UI::ToolButton>& getEnabledButton() const;

                void setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                ViewBackgroundOptions _backgroundOptions;
                std::shared_ptr<MediaWidget> _activeWidget;
                std::shared_ptr<UI::ToolButton> _borderEnabledButton;
                std::shared_ptr<UI::FloatSlider> _borderWidthSlider;
                std::shared_ptr<UI::ColorPickerSwatch> _borderColorPickerSwatch;
                std::shared_ptr<UI::FormLayout> _layout;
                std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > _activeWidgetObserver;
                std::shared_ptr<ValueObserver<ViewBackgroundOptions> > _backgroundOptionsObserver;
            };

            void BorderWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                _borderEnabledButton = UI::ToolButton::create(context);
                _borderEnabledButton->setButtonType(UI::ButtonType::Toggle);
                _borderEnabledButton->setIcon("djvIconHidden");
                _borderEnabledButton->setCheckedIcon("djvIconVisible");
                _borderEnabledButton->setInsideMargin(UI::MetricsRole::None);
                _borderWidthSlider = UI::FloatSlider::create(context);
                _borderWidthSlider->setRange(FloatRange(1.F, 20.F));
                _borderWidthSlider->setSmallIncrement(1.F);
                _borderWidthSlider->setLargeIncrement(5.F);
                _borderColorPickerSwatch = UI::ColorPickerSwatch::create(context);
                _borderColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);

                _layout = UI::FormLayout::create(context);
                _layout->setMargin(UI::MetricsRole::MarginSmall);
                _layout->addChild(_borderWidthSlider);
                _layout->addChild(_borderColorPickerSwatch);
                addChild(_layout);

                _widgetUpdate();

                auto weak = std::weak_ptr<BorderWidget>(std::dynamic_pointer_cast<BorderWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<Context>(context);
                _borderEnabledButton->setCheckedCallback(
                    [weak, contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_backgroundOptions.border = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setBackgroundOptions(widget->_backgroundOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setBackgroundOptions(widget->_backgroundOptions);
                            }
                        }
                    });
                _borderWidthSlider->setValueCallback(
                    [weak, contextWeak](float value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_backgroundOptions.borderWidth = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setBackgroundOptions(widget->_backgroundOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setBackgroundOptions(widget->_backgroundOptions);
                            }
                        }
                    });
                _borderColorPickerSwatch->setColorCallback(
                    [weak, contextWeak](const AV::Image::Color& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_backgroundOptions.borderColor = value;
                                widget->_widgetUpdate();
                                if (widget->_activeWidget)
                                {
                                    widget->_activeWidget->getViewWidget()->setBackgroundOptions(widget->_backgroundOptions);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                                viewSettings->setBackgroundOptions(widget->_backgroundOptions);
                            }
                        }
                    });

                if (auto windowSystem = context->getSystemT<WindowSystem>())
                {
                    _activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                        windowSystem->observeActiveWidget(),
                        [weak](const std::shared_ptr<MediaWidget>& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_activeWidget = value;
                                if (widget->_activeWidget)
                                {
                                    widget->_backgroundOptionsObserver = ValueObserver<ViewBackgroundOptions>::create(
                                        widget->_activeWidget->getViewWidget()->observeBackgroundOptions(),
                                        [weak](const ViewBackgroundOptions& value)
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                widget->_backgroundOptions = value;
                                                widget->_widgetUpdate();
                                            }
                                        });
                                }
                                else
                                {
                                    widget->_backgroundOptionsObserver.reset();
                                }
                            }
                        });
                }
            }

            BorderWidget::BorderWidget()
            {}

            BorderWidget::~BorderWidget()
            {}

            std::shared_ptr<BorderWidget> BorderWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<BorderWidget>(new BorderWidget);
                out->_init(context);
                return out;
            }

            const std::shared_ptr<UI::ToolButton>& BorderWidget::getEnabledButton() const
            {
                return _borderEnabledButton;
            }

            void BorderWidget::setLabelSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
            {
                _layout->setLabelSizeGroup(value);
            }

            void BorderWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void BorderWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void BorderWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _borderEnabledButton->setTooltip(_getText(DJV_TEXT("widget_view_border_enabled_tooltip")));
                    _layout->setText(_borderWidthSlider, _getText(DJV_TEXT("widget_view_border_width")) + ":");
                    _layout->setText(_borderColorPickerSwatch, _getText(DJV_TEXT("widget_view_border_color")) + ":");
                }
            }

            void BorderWidget::_widgetUpdate()
            {
                _borderEnabledButton->setChecked(_backgroundOptions.border);
                _borderWidthSlider->setValue(_backgroundOptions.borderWidth);
                _borderColorPickerSwatch->setColor(_backgroundOptions.borderColor);
            }

        } // namespace

        struct ViewControlsWidget::Private
        {
            std::shared_ptr<PosAndZoomWidget> zoomAndPosWidget;
            std::shared_ptr<GridWidget> gridWidget;
            std::shared_ptr<HUDWidget> hudWidget;
            std::shared_ptr<BackgroundWidget> backgroundWidget;
            std::shared_ptr<BorderWidget> borderWidget;
            std::shared_ptr<UI::LabelSizeGroup> sizeGroup;
            std::map<std::string, std::shared_ptr<UI::Bellows> > bellows;
        };

        void ViewControlsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewControlsWidget");

            p.sizeGroup = UI::LabelSizeGroup::create();

            p.zoomAndPosWidget = PosAndZoomWidget::create(context);
            p.zoomAndPosWidget->setLabelSizeGroup(p.sizeGroup);
            p.gridWidget = GridWidget::create(context);
            p.gridWidget->setLabelSizeGroup(p.sizeGroup);
            p.hudWidget = HUDWidget::create(context);
            p.hudWidget->setLabelSizeGroup(p.sizeGroup);
            p.backgroundWidget = BackgroundWidget::create(context);
            p.backgroundWidget->setLabelSizeGroup(p.sizeGroup);
            p.borderWidget = BorderWidget::create(context);
            p.borderWidget->setLabelSizeGroup(p.sizeGroup);

            p.bellows["View"] = UI::Bellows::create(context);
            p.bellows["View"]->addChild(p.zoomAndPosWidget);
            p.bellows["Grid"] = UI::Bellows::create(context);
            p.bellows["Grid"]->addWidget(p.gridWidget->getEnabledButton());
            p.bellows["Grid"]->addChild(p.gridWidget);
            p.bellows["HUD"] = UI::Bellows::create(context);
            p.bellows["HUD"]->addWidget(p.hudWidget->getEnabledButton());
            p.bellows["HUD"]->addChild(p.hudWidget);
            p.bellows["Background"] = UI::Bellows::create(context);
            p.bellows["Background"]->addChild(p.backgroundWidget);
            p.bellows["Border"] = UI::Bellows::create(context);
            p.bellows["Border"]->addWidget(p.borderWidget->getEnabledButton());
            p.bellows["Border"]->addChild(p.borderWidget);
            
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.bellows["View"]);
            vLayout->addChild(p.bellows["Grid"]);
            vLayout->addChild(p.bellows["HUD"]);
            vLayout->addChild(p.bellows["Background"]);
            vLayout->addChild(p.bellows["Border"]);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setBackgroundRole(UI::ColorRole::Background);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(vLayout);
            addChild(scrollWidget);
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

        std::map<std::string, bool> ViewControlsWidget::getBellowsState() const
        {
            DJV_PRIVATE_PTR();
            std::map<std::string, bool> out;
            for (const auto& i : p.bellows)
            {
                out[i.first] = i.second->isOpen();
            }
            return out;
        }

        void ViewControlsWidget::setBellowsState(const std::map<std::string, bool>& value)
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

        void ViewControlsWidget::_initLayoutEvent(Event::InitLayout&)
        {
            _p->sizeGroup->calcMinimumSize();
        }

        void ViewControlsWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("view_controls")));
                p.bellows["View"]->setText(_getText(DJV_TEXT("view")));
                p.bellows["Grid"]->setText(_getText(DJV_TEXT("view_grid")));
                p.bellows["HUD"]->setText(_getText(DJV_TEXT("view_hud")));
                p.bellows["Background"]->setText(_getText(DJV_TEXT("view_background")));
                p.bellows["Border"]->setText(_getText(DJV_TEXT("view_border")));
            }
        }

    } // namespace ViewApp
} // namespace djv

