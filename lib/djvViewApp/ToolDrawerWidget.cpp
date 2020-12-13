// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ToolDrawerWidget.h>

#include <djvViewApp/ToolSystem.h>

#include <djvUI/FlowLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ToolDrawerWidget::Private
        {
            CurrentTool currentTool;

            std::shared_ptr<UI::FlowLayout> buttonLayout;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;
            std::shared_ptr<UI::ToolBar> footerToolBar;
            std::shared_ptr<UI::VerticalLayout> footerLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Value<CurrentTool> > currentToolObserver;
        };

        void ToolDrawerWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setBackgroundRole(UI::ColorRole::Background);

            std::vector<std::shared_ptr<UI::ToolButton> > buttons;
            auto toolSystem = context->getSystemT<ToolSystem>();
            for (const auto& i : toolSystem->getToolActions())
            {
                auto button = UI::ToolButton::create(context);
                button->addAction(i);
                buttons.push_back(button);
            }

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setShadowOverlay({ UI::Side::Top });
            p.buttonLayout = UI::FlowLayout::create(context);
            p.buttonLayout->setSpacing(UI::MetricsRole::None);
            p.buttonLayout->setBackgroundRole(UI::ColorRole::BackgroundToolBar);
            for (const auto& i : buttons)
            {
                p.buttonLayout->addChild(i);
            }
            p.layout->addChild(p.buttonLayout);
            p.layout->addSeparator();
            p.scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setMinimumSizeRole(UI::MetricsRole::Icon);
            p.scrollWidget->setShadowOverlay({ UI::Side::Top });
            p.layout->addChild(p.scrollWidget);
            p.layout->setStretch(p.scrollWidget, UI::RowStretch::Expand);
            p.footerToolBar = UI::ToolBar::create(context);
            p.footerToolBar->addExpander();
            p.footerLayout = UI::VerticalLayout::create(context);
            p.footerLayout->setSpacing(UI::MetricsRole::None);
            p.footerLayout->addSeparator();
            p.footerLayout->addChild(p.footerToolBar);
            p.footerLayout->hide();
            p.layout->addChild(p.footerLayout);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ToolDrawerWidget>(std::dynamic_pointer_cast<ToolDrawerWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.currentToolObserver = Observer::Value<CurrentTool>::create(
                toolSystem->observeCurrentTool(),
                [weak, contextWeak](const CurrentTool& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->currentTool.system)
                            {
                                widget->_p->currentTool.system->deleteToolWidget(widget->_p->currentTool.action);
                                widget->_p->scrollWidget->clearChildren();
                                widget->_p->footerLayout->hide();
                                widget->_p->footerToolBar->clearChildren();
                            }
                            widget->_p->currentTool = value;
                            if (widget->_p->currentTool.system)
                            {
                                auto toolWidgetData = widget->_p->currentTool.system->createToolWidget(widget->_p->currentTool.action);
                                if (toolWidgetData.widget)
                                {
                                    widget->_p->scrollWidget->addChild(toolWidgetData.widget);
                                }
                                if (toolWidgetData.footer)
                                {
                                    widget->_p->footerToolBar->addExpander();
                                    widget->_p->footerToolBar->addChild(toolWidgetData.footer);
                                    widget->_p->footerLayout->show();
                                }
                            }
                        }
                    }
                });
        }

        ToolDrawerWidget::ToolDrawerWidget() :
            _p(new Private)
        {}

        ToolDrawerWidget::~ToolDrawerWidget()
        {}

        std::shared_ptr<ToolDrawerWidget> ToolDrawerWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ToolDrawerWidget>(new ToolDrawerWidget);
            out->_init(context);
            return out;
        }

        void ToolDrawerWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ToolDrawerWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ToolDrawerWidget::_initEvent(System::Event::Init& event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
            }
        }

        void ToolDrawerWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
        }

    } // namespace ViewApp
} // namespace djv

