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

#include <djvViewApp/ViewSettings.h>

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/Action.h>
#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/TabWidget.h>
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

            std::shared_ptr<UI::ToggleButton> gridEnabledButton;
            std::shared_ptr<UI::IntSlider> gridSizeSlider;
            std::shared_ptr<UI::ColorPickerSwatch> gridColorPickerSwatch;
            std::shared_ptr<UI::ToggleButton> gridLabelsButton;
            std::shared_ptr<UI::FormLayout> gridLayout;
                        
            std::shared_ptr<UI::TabWidget> tabWidget;
            std::map<std::string, size_t> tabIDs;
            std::function<void(const GridOptions&)> gridOptionsCallback;
        };

        void ViewControlsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewControlsWidget");

            p.gridEnabledButton = UI::ToggleButton::create(context);

            p.gridSizeSlider = UI::IntSlider::create(context);
            p.gridSizeSlider->setRange(IntRange(1, 500));

            p.gridColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.gridColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);

            p.gridLabelsButton = UI::ToggleButton::create(context);

            p.gridLayout = UI::FormLayout::create(context);
            p.gridLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.gridLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.gridLayout->setShadowOverlay({ UI::Side::Top });
            p.gridLayout->addChild(p.gridEnabledButton);
            p.gridLayout->addChild(p.gridSizeSlider);
            p.gridLayout->addChild(p.gridColorPickerSwatch);
            p.gridLayout->addChild(p.gridLabelsButton);

            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setBackgroundRole(UI::ColorRole::Background);
            p.tabWidget->setShadowOverlay({ UI::Side::Top });
            p.tabIDs["Grid"] = p.tabWidget->addTab(std::string(), p.gridLayout);
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
                        if (widget->_p->gridOptionsCallback)
                        {
                            widget->_p->gridOptionsCallback(widget->_p->gridOptions);
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
                        if (widget->_p->gridOptionsCallback)
                        {
                            widget->_p->gridOptionsCallback(widget->_p->gridOptions);
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
                        if (widget->_p->gridOptionsCallback)
                        {
                            widget->_p->gridOptionsCallback(widget->_p->gridOptions);
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
                        if (widget->_p->gridOptionsCallback)
                        {
                            widget->_p->gridOptionsCallback(widget->_p->gridOptions);
                        }
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

        void ViewControlsWidget::setGridOptions(const GridOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.gridOptions)
                return;
            p.gridOptions = value;
            _widgetUpdate();
        }
        
        int ViewControlsWidget::getCurrentTab() const
        {
            return _p->tabWidget->getCurrentTab();
        }

        void ViewControlsWidget::setCurrentTab(int value)
        {
            _p->tabWidget->setCurrentTab(value);
        }
        
        void ViewControlsWidget::setGridOptionsCallback(const std::function<void(const GridOptions&)>& callback)
        {
            _p->gridOptionsCallback = callback;
        }
        
        void ViewControlsWidget::_textUpdateEvent(Event::TextUpdate & event)
        {
            MDIWidget::_textUpdateEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("View Controls")));
            p.gridLayout->setText(p.gridEnabledButton, _getText(DJV_TEXT("Enabled")) + ":");
            p.gridLayout->setText(p.gridSizeSlider, _getText(DJV_TEXT("Size")) + ":");
            p.gridLayout->setText(p.gridColorPickerSwatch, _getText(DJV_TEXT("Color")) + ":");
            p.gridLayout->setText(p.gridLabelsButton, _getText(DJV_TEXT("Labels")) + ":");
            p.tabWidget->setText(p.tabIDs["Grid"], _getText(DJV_TEXT("Grid")));
            _widgetUpdate();
        }

        void ViewControlsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.gridEnabledButton->setChecked(p.gridOptions.enabled);
            p.gridSizeSlider->setValue(p.gridOptions.size);
            p.gridColorPickerSwatch->setColor(p.gridOptions.color);
            p.gridLabelsButton->setChecked(p.gridOptions.labels);
        }

    } // namespace ViewApp
} // namespace djv

