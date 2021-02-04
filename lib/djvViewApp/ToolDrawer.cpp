// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ToolDrawer.h>

#include <djvViewApp/ToolSystem.h>

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
        struct ToolDrawer::Private
        {
            CurrentTool currentTool;

            std::shared_ptr<UI::VerticalLayout> titleBarLayout;
            std::shared_ptr<UI::VerticalLayout> toolBarLayout;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;
            std::shared_ptr<UI::VerticalLayout> footerLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Value<CurrentTool> > currentToolObserver;
        };

        void ToolDrawer::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setBackgroundRole(UI::ColorRole::Background);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setShadowOverlay({ UI::Side::Top });
            p.titleBarLayout = UI::VerticalLayout::create(context);
            p.titleBarLayout->setSpacing(UI::MetricsRole::None);
            p.titleBarLayout->hide();
            p.layout->addChild(p.titleBarLayout);
            p.toolBarLayout = UI::VerticalLayout::create(context);
            p.toolBarLayout->setSpacing(UI::MetricsRole::None);
            p.toolBarLayout->hide();
            p.layout->addChild(p.toolBarLayout);
            p.scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setShadowOverlay({ UI::Side::Top });
            p.layout->addChild(p.scrollWidget);
            p.layout->setStretch(p.scrollWidget);
            p.footerLayout = UI::VerticalLayout::create(context);
            p.footerLayout->setSpacing(UI::MetricsRole::None);
            p.footerLayout->hide();
            p.layout->addChild(p.footerLayout);
            addChild(p.layout);

            auto toolSystem = context->getSystemT<ToolSystem>();
            auto weak = std::weak_ptr<ToolDrawer>(std::dynamic_pointer_cast<ToolDrawer>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.currentToolObserver = Observer::Value<CurrentTool>::create(
                toolSystem->observeCurrentTool(),
                [weak, contextWeak](const CurrentTool& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->currentTool.action && widget->_p->currentTool.system)
                            {
                                widget->_p->currentTool.system->deleteToolWidget(widget->_p->currentTool.action);
                                if (value.action && value.system)
                                {
                                    widget->_p->titleBarLayout->hide();
                                    widget->_p->titleBarLayout->clearChildren();
                                    widget->_p->toolBarLayout->hide();
                                    widget->_p->toolBarLayout->clearChildren();
                                    widget->_p->scrollWidget->clearChildren();
                                    widget->_p->footerLayout->hide();
                                    widget->_p->footerLayout->clearChildren();
                                }
                            }
                            widget->_p->currentTool = value;
                            if (widget->_p->currentTool.action && widget->_p->currentTool.system)
                            {
                                auto toolWidgetData = widget->_p->currentTool.system->createToolWidget(widget->_p->currentTool.action);
                                if (toolWidgetData.titleBar)
                                {
                                    widget->_p->titleBarLayout->addChild(toolWidgetData.titleBar);
                                    widget->_p->titleBarLayout->addSeparator();
                                    widget->_p->titleBarLayout->show();
                                }
                                if (toolWidgetData.toolBar)
                                {
                                    widget->_p->toolBarLayout->addChild(toolWidgetData.toolBar);
                                    widget->_p->toolBarLayout->addSeparator();
                                    widget->_p->toolBarLayout->show();
                                }
                                if (toolWidgetData.widget)
                                {
                                    widget->_p->scrollWidget->addChild(toolWidgetData.widget);
                                }
                                if (toolWidgetData.footer)
                                {
                                    widget->_p->footerLayout->addSeparator();
                                    widget->_p->footerLayout->addChild(toolWidgetData.footer);
                                    widget->_p->footerLayout->show();
                                }
                            }
                        }
                    }
                });
        }

        ToolDrawer::ToolDrawer() :
            _p(new Private)
        {}

        ToolDrawer::~ToolDrawer()
        {}

        std::shared_ptr<ToolDrawer> ToolDrawer::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ToolDrawer>(new ToolDrawer);
            out->_init(context);
            return out;
        }

        void ToolDrawer::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ToolDrawer::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

    } // namespace ViewApp
} // namespace djv

