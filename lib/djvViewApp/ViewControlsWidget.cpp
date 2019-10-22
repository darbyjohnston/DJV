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
#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/TabWidget.h>
#include <djvUI/ToolButton.h>
#include <djvUI/ToggleButton.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewControlsWidget::Private
        {
            GridOptions gridOptions;
            AV::Image::Color backgroundColor;

            std::shared_ptr<MediaWidget> activeWidget;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;

            std::shared_ptr<UI::ToggleButton> gridEnabledButton;
            std::shared_ptr<UI::IntSlider> gridSizeSlider;
            std::shared_ptr<UI::ColorPickerSwatch> gridColorPickerSwatch;
            std::shared_ptr<UI::ToggleButton> gridLabelsButton;
            std::shared_ptr<UI::ToolButton> gridSettingsButton;
            std::shared_ptr<UI::FormLayout> gridFormLayout;
            std::shared_ptr<UI::VerticalLayout> gridLayout;

            std::shared_ptr<UI::ColorPickerSwatch> backgroundColorPickerSwatch;
            std::shared_ptr<UI::ToolButton> backgroundSettingsButton;
            std::shared_ptr<UI::FormLayout> backgroundFormLayout;
            std::shared_ptr<UI::VerticalLayout> backgroundLayout;

            std::shared_ptr<UI::TabWidget> tabWidget;

            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<GridOptions> > gridOptionsObserver;
            std::shared_ptr<ValueObserver<GridOptions> > gridOptionsObserver2;
            std::shared_ptr<ValueObserver<AV::Image::Color> > backgroundColorObserver;
            std::shared_ptr<ValueObserver<AV::Image::Color> > backgroundColorObserver2;
        };

        void ViewControlsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewControlsWidget");

            p.actions["GridSettings"] = UI::Action::create();
            p.actions["BackgroundSettings"] = UI::Action::create();

            p.gridEnabledButton = UI::ToggleButton::create(context);

            p.gridSizeSlider = UI::IntSlider::create(context);
            p.gridSizeSlider->setRange(IntRange(1, 500));

            p.gridColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.gridColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);

            p.gridLabelsButton = UI::ToggleButton::create(context);

            p.gridSettingsButton = UI::ToolButton::create(context);
            p.gridSettingsButton->setIcon("djvIconSettings");

            p.gridFormLayout = UI::FormLayout::create(context);
            p.gridFormLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.gridFormLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.gridFormLayout->setShadowOverlay({ UI::Side::Top });
            p.gridFormLayout->addChild(p.gridEnabledButton);
            p.gridFormLayout->addChild(p.gridSizeSlider);
            p.gridFormLayout->addChild(p.gridColorPickerSwatch);
            p.gridFormLayout->addChild(p.gridLabelsButton);
            p.gridLayout = UI::VerticalLayout::create(context);
            p.gridLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            p.gridLayout->addChild(p.gridFormLayout);
            p.gridLayout->setStretch(p.gridFormLayout, UI::RowStretch::Expand);
            p.gridLayout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout->addExpander();
            hLayout->addChild(p.gridSettingsButton);
            p.gridLayout->addChild(hLayout);

            p.backgroundColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.backgroundColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);

            p.backgroundSettingsButton = UI::ToolButton::create(context);
            p.backgroundSettingsButton->setIcon("djvIconSettings");

            p.backgroundFormLayout = UI::FormLayout::create(context);
            p.backgroundFormLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.backgroundFormLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.backgroundFormLayout->setShadowOverlay({ UI::Side::Top });
            p.backgroundFormLayout->addChild(p.backgroundColorPickerSwatch);

            p.backgroundLayout = UI::VerticalLayout::create(context);
            p.backgroundLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            p.backgroundLayout->addChild(p.backgroundFormLayout);
            p.backgroundLayout->setStretch(p.backgroundFormLayout, UI::RowStretch::Expand);
            p.backgroundLayout->addSeparator();
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout->addExpander();
            hLayout->addChild(p.backgroundSettingsButton);
            p.backgroundLayout->addChild(hLayout);

            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setBackgroundRole(UI::ColorRole::Background);
            p.tabWidget->setShadowOverlay({ UI::Side::Top });
            p.tabWidget->addChild(p.gridLayout);
            p.tabWidget->addChild(p.backgroundLayout);
            addChild(p.tabWidget);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewControlsWidget>(std::dynamic_pointer_cast<ViewControlsWidget>(shared_from_this()));
            p.gridEnabledButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->gridOptions.enabled = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });

            p.gridSizeSlider->setValueCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->gridOptions.size = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });

            p.gridColorPickerSwatch->setColorCallback(
                [weak](const AV::Image::Color& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->gridOptions.color = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });

            p.gridLabelsButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->gridOptions.labels = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.gridSettingsButton->setClickedCallback(
                [weak, contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });

            p.backgroundColorPickerSwatch->setColorCallback(
                [weak](const AV::Image::Color& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundColor = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getImageView()->setBackgroundColor(widget->_p->backgroundColor);
                        }
                    }
                });

            p.backgroundSettingsButton->setClickedCallback(
                [weak, contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
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
                                widget->_p->gridOptionsObserver.reset();
                                widget->_p->backgroundColorObserver.reset();
                            }
                        }
                    });
            }

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.gridOptionsObserver2 = ValueObserver<GridOptions>::create(
                viewSettings->observeGridOptions(),
                [weak](const GridOptions& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->gridOptions = value;
                        widget->_widgetUpdate();
                    }
                });

            p.backgroundColorObserver2 = ValueObserver<AV::Image::Color>::create(
                viewSettings->observeBackgroundColor(),
                [weak](const AV::Image::Color& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundColor = value;
                        widget->_widgetUpdate();
                    }
                });
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
        
        void ViewControlsWidget::_textUpdateEvent(Event::TextUpdate & event)
        {
            MDIWidget::_textUpdateEvent(event);
            DJV_PRIVATE_PTR();

            setTitle(_getText(DJV_TEXT("View Controls")));
            
            p.actions["GridSettings"]->setText(_getText(DJV_TEXT("Set as Default")));
            p.actions["BackgroundSettings"]->setText(_getText(DJV_TEXT("Set as Default")));

            p.gridSettingsButton->setTooltip(_getText(DJV_TEXT("Set as default settings")));
            p.gridFormLayout->setText(p.gridEnabledButton, _getText(DJV_TEXT("Enabled")) + ":");
            p.gridFormLayout->setText(p.gridSizeSlider, _getText(DJV_TEXT("Size")) + ":");
            p.gridFormLayout->setText(p.gridColorPickerSwatch, _getText(DJV_TEXT("Color")) + ":");
            p.gridFormLayout->setText(p.gridLabelsButton, _getText(DJV_TEXT("Labels")) + ":");
                        
            p.backgroundSettingsButton->setTooltip(_getText(DJV_TEXT("Set as default settings")));
            p.backgroundFormLayout->setText(p.backgroundColorPickerSwatch, _getText(DJV_TEXT("Color")) + ":");
            
            p.tabWidget->setText(p.gridLayout, _getText(DJV_TEXT("Grid")));
            p.tabWidget->setText(p.backgroundLayout, _getText(DJV_TEXT("Background")));
            
            _widgetUpdate();
        }

        void ViewControlsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();

            p.gridEnabledButton->setChecked(p.gridOptions.enabled);
            p.gridSizeSlider->setValue(p.gridOptions.size);
            p.gridColorPickerSwatch->setColor(p.gridOptions.color);
            p.gridLabelsButton->setChecked(p.gridOptions.labels);
            
            p.backgroundColorPickerSwatch->setColor(p.backgroundColor);
        }

    } // namespace ViewApp
} // namespace djv

