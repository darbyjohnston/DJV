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

#include <djvViewApp/ViewControlsWidget.h>

#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/Action.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/TabWidget.h>
#include <djvUI/ToolButton.h>
#include <djvUI/ToggleButton.h>

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
            AV::Image::Color backgroundColor;

            std::shared_ptr<MediaWidget> activeWidget;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;

            std::shared_ptr<UI::FloatEdit> viewPosEdit[2];
            std::shared_ptr<UI::ToolButton> viewPosResetButton;
            std::shared_ptr<UI::FloatEdit> viewZoomEdit;
            std::shared_ptr<UI::ToolButton> viewZoomResetButton;
            std::shared_ptr<UI::FormLayout> viewFormLayout;
            std::shared_ptr<UI::HorizontalLayout> viewPosLayout;
            std::shared_ptr<UI::HorizontalLayout> viewZoomLayout;
            std::shared_ptr<UI::ScrollWidget> viewScrollWidget;

            std::shared_ptr<UI::ToggleButton> gridEnabledButton;
            std::shared_ptr<UI::IntSlider> gridSizeSlider;
            std::shared_ptr<UI::ColorPickerSwatch> gridColorPickerSwatch;
            std::shared_ptr<UI::ToggleButton> gridLabelsButton;
            std::shared_ptr<UI::FormLayout> gridFormLayout;
            std::shared_ptr<UI::ScrollWidget> gridScrollWidget;

            std::shared_ptr<UI::ColorPickerSwatch> backgroundColorPickerSwatch;
            std::shared_ptr<UI::FormLayout> backgroundFormLayout;
            std::shared_ptr<UI::ScrollWidget> backgroundScrollWidget;

            std::shared_ptr<UI::TabWidget> tabWidget;

            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<glm::vec2> > viewPosObserver;
            std::shared_ptr<ValueObserver<float> > viewZoomObserver;
            std::shared_ptr<ValueObserver<GridOptions> > gridOptionsObserver;
            std::shared_ptr<ValueObserver<AV::Image::Color> > backgroundColorObserver;
        };

        void ViewControlsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewControlsWidget");

            p.actions["GridSettings"] = UI::Action::create();
            p.actions["BackgroundSettings"] = UI::Action::create();

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
            model->setRange(FloatRange(.1F, 100.F));
            model->setSmallIncrement(.1F);
            model->setLargeIncrement(1.F);
            p.viewZoomEdit->setModel(model);
            p.viewZoomResetButton = UI::ToolButton::create(context);
            p.viewZoomResetButton->setIcon("djvIconCloseSmall");

            p.gridEnabledButton = UI::ToggleButton::create(context);
            p.gridSizeSlider = UI::IntSlider::create(context);
            p.gridSizeSlider->setRange(IntRange(1, 500));
            p.gridColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.gridColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.gridLabelsButton = UI::ToggleButton::create(context);

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
            p.gridFormLayout->addChild(p.gridEnabledButton);
            p.gridFormLayout->addChild(p.gridSizeSlider);
            p.gridFormLayout->addChild(p.gridColorPickerSwatch);
            p.gridFormLayout->addChild(p.gridLabelsButton);
            p.gridScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.gridScrollWidget->setBorder(false);
            p.gridScrollWidget->addChild(p.gridFormLayout);

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

            p.gridLabelsButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->gridOptions.labels = value;
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

            setTitle(_getText(DJV_TEXT("View Controls")));
            
            p.actions["GridSettings"]->setText(_getText(DJV_TEXT("Set as Default")));
            p.actions["BackgroundSettings"]->setText(_getText(DJV_TEXT("Set as Default")));

            p.viewPosResetButton->setTooltip(_getText(DJV_TEXT("Reset the value.")));
            p.viewZoomResetButton->setTooltip(_getText(DJV_TEXT("Reset the value.")));
            p.viewFormLayout->setText(p.viewPosLayout, _getText(DJV_TEXT("Position")) + ":");
            p.viewFormLayout->setText(p.viewZoomLayout, _getText(DJV_TEXT("Zoom")) + ":");

            p.gridFormLayout->setText(p.gridEnabledButton, _getText(DJV_TEXT("Enabled")) + ":");
            p.gridFormLayout->setText(p.gridSizeSlider, _getText(DJV_TEXT("Size")) + ":");
            p.gridFormLayout->setText(p.gridColorPickerSwatch, _getText(DJV_TEXT("Color")) + ":");
            p.gridFormLayout->setText(p.gridLabelsButton, _getText(DJV_TEXT("Labels")) + ":");

            p.backgroundFormLayout->setText(p.backgroundColorPickerSwatch, _getText(DJV_TEXT("Color")) + ":");
            
            p.tabWidget->setText(p.viewScrollWidget, _getText(DJV_TEXT("View")));
            p.tabWidget->setText(p.gridScrollWidget, _getText(DJV_TEXT("Grid")));
            p.tabWidget->setText(p.backgroundScrollWidget, _getText(DJV_TEXT("Background")));
            
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

            p.gridEnabledButton->setChecked(p.gridOptions.enabled);
            p.gridSizeSlider->setValue(p.gridOptions.size);
            p.gridColorPickerSwatch->setColor(p.gridOptions.color);
            p.gridLabelsButton->setChecked(p.gridOptions.labels);
            
            p.backgroundColorPickerSwatch->setColor(p.backgroundColor);
        }

    } // namespace ViewApp
} // namespace djv

