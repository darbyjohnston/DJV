// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewPanZoomWidget.h>

#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/ViewSystem.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/FloatEdit.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToolButton.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewPanZoomWidget::Private
        {
            std::shared_ptr<MediaWidget> activeWidget;
            glm::vec2 viewPos = glm::vec2(0.F, 0.F);
            float viewZoom = 1.F;

            std::shared_ptr<UI::Numeric::FloatEdit> viewPosEdit[2];
            std::shared_ptr<UI::ToolButton> viewPosResetButton[2];
            std::shared_ptr<UI::Numeric::FloatEdit> viewZoomEdit;
            std::shared_ptr<UI::ToolButton> viewZoomResetButton;
            std::shared_ptr<UI::HorizontalLayout> viewPosLayout[2];
            std::shared_ptr<UI::HorizontalLayout> viewZoomLayout;
            std::shared_ptr<UI::FormLayout> layout;

            std::shared_ptr<Observer::Value<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<Observer::Value<glm::vec2> > viewPosObserver;
            std::shared_ptr<Observer::Value<float> > viewZoomObserver;
        };

        void ViewPanZoomWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewPanZoomWidget");

            for (size_t i = 0; i < 2; ++i)
            {
                auto edit = UI::Numeric::FloatEdit::create(context);
                edit->setRange(Math::FloatRange(-1000000.F, 1000000.F));
                edit->setSmallIncrement(1.F);
                edit->setLargeIncrement(10.F);
                p.viewPosEdit[i] = edit;

                p.viewPosResetButton[i] = UI::ToolButton::create(context);
                p.viewPosResetButton[i]->setIcon("djvIconClearSmall");
                p.viewPosResetButton[i]->setInsideMargin(UI::MetricsRole::None);
            }
            p.viewZoomEdit = UI::Numeric::FloatEdit::create(context);
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
                p.viewPosLayout[i]->setStretch(p.viewPosEdit[i]);
                p.viewPosLayout[i]->addChild(p.viewPosResetButton[i]);
                p.layout->addChild(p.viewPosLayout[i]);
            }
            p.viewZoomLayout = UI::HorizontalLayout::create(context);
            p.viewZoomLayout->setSpacing(UI::MetricsRole::None);
            p.viewZoomLayout->addChild(p.viewZoomEdit);
            p.viewZoomLayout->setStretch(p.viewZoomEdit);
            p.viewZoomLayout->addChild(p.viewZoomResetButton);
            p.layout->addChild(p.viewZoomLayout);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewPanZoomWidget>(std::dynamic_pointer_cast<ViewPanZoomWidget>(shared_from_this()));
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
                p.activeWidgetObserver = Observer::Value<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->activeWidget = value;
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->viewPosObserver = Observer::Value<glm::vec2>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeImagePos(),
                                    [weak](const glm::vec2& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewPos = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->viewZoomObserver = Observer::Value<float>::create(
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

        ViewPanZoomWidget::ViewPanZoomWidget() :
            _p(new Private)
        {}

        ViewPanZoomWidget::~ViewPanZoomWidget()
        {}

        std::shared_ptr<ViewPanZoomWidget> ViewPanZoomWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewPanZoomWidget>(new ViewPanZoomWidget);
            out->_init(context);
            return out;
        }

        void ViewPanZoomWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewPanZoomWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewPanZoomWidget::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                for (size_t i = 0; i < 2; ++i)
                {
                    p.viewPosResetButton[i]->setTooltip(_getText(DJV_TEXT("reset_the_value")));
                }
                p.viewZoomResetButton->setTooltip(_getText(DJV_TEXT("reset_the_value")));
                p.layout->setText(p.viewPosLayout[0], _getText(DJV_TEXT("view_position_x")) + ":");
                p.layout->setText(p.viewPosLayout[1], _getText(DJV_TEXT("view_position_y")) + ":");
                p.layout->setText(p.viewZoomLayout, _getText(DJV_TEXT("view_zoom")) + ":");
            }
        }

        void ViewPanZoomWidget::_setPos(const glm::vec2& value)
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

        void ViewPanZoomWidget::_setZoom(float value)
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

        void ViewPanZoomWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.viewPosEdit[0]->setValue(p.viewPos.x);
            p.viewPosEdit[1]->setValue(p.viewPos.y);
            p.viewPosResetButton[0]->setEnabled(p.viewPos.x != 0.F);
            p.viewPosResetButton[1]->setEnabled(p.viewPos.y != 0.F);
            p.viewZoomEdit->setValue(p.viewZoom);
            p.viewZoomResetButton->setEnabled(p.viewZoom != 1.F);
        }

    } // namespace ViewApp
} // namespace djv

