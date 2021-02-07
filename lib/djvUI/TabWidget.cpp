// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/TabWidget.h>

#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/TabBar.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct TabWidget::Private
        {
            std::vector<std::shared_ptr<Widget> > widgets;
            std::map<std::shared_ptr<Widget>, std::string> widgetToText;
            std::shared_ptr<TabBar> tabBar;
            std::shared_ptr<VerticalLayout> layout;
            std::shared_ptr<SoloLayout> soloLayout;
            std::function<void(int)> callback;
        };

        void TabWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);            
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::TabWidget");

            p.tabBar = TabBar::create(context);

            p.soloLayout = SoloLayout::create(context);

            p.layout = VerticalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);
            p.layout->addChild(p.tabBar);
            p.layout->addChild(p.soloLayout);
            p.layout->setStretch(p.soloLayout);
            Widget::addChild(p.layout);

            auto weak = std::weak_ptr<TabWidget>(std::dynamic_pointer_cast<TabWidget>(shared_from_this()));
            p.tabBar->setCurrentTabCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value >= 0 && value < static_cast<int>(widget->_p->widgets.size()))
                        {
                            widget->_p->soloLayout->setCurrentWidget(widget->_p->widgets[value]);
                        }
                        if (widget->_p->callback)
                        {
                            widget->_p->callback(value);
                        }
                    }
                });
        }

        TabWidget::TabWidget() :
            _p(new Private)
        {}

        TabWidget::~TabWidget()
        {}

        std::shared_ptr<TabWidget> TabWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<TabWidget>(new TabWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<TabBar>& TabWidget::getTabBar() const
        {
            return _p->tabBar;
        }

        void TabWidget::setText(const std::shared_ptr<Widget>& widget, const std::string& text)
        {
            DJV_PRIVATE_PTR();
            const auto i = std::find(p.widgets.begin(), p.widgets.end(), widget);
            if (i != p.widgets.end())
            {
                p.widgetToText[widget] = text;
                p.tabBar->setText(i - p.widgets.begin(), text);
            }
        }

        int TabWidget::getCurrentTab() const
        {
            return _p->tabBar->getCurrentTab();
        }

        void TabWidget::setCurrentTab(int value)
        {
            DJV_PRIVATE_PTR();
            p.tabBar->setCurrentTab(value);
            if (value >= 0 && value < static_cast<int>(p.widgets.size()))
            {
                p.soloLayout->setCurrentWidget(p.widgets[value]);
            }
        }

        void TabWidget::setCurrentTabCallback(const std::function<void(int)>& value)
        {
            _p->callback = value;
        }

        bool TabWidget::areTabsClosable() const
        {
            return _p->tabBar->areTabsClosable();
        }

        void TabWidget::setTabsClosable(bool value)
        {
            _p->tabBar->setTabsClosable(value);
        }

        void TabWidget::setTabCloseCallback(const std::function<void(int)>& value)
        {
            _p->tabBar->setTabCloseCallback(value);
        }

        void TabWidget::addChild(const std::shared_ptr<IObject>& value)
        {
            DJV_PRIVATE_PTR();
            if (auto widget = std::dynamic_pointer_cast<Widget>(value))
            {
                p.widgets.push_back(widget);
                p.widgetToText[widget] = std::string();
                p.soloLayout->addChild(widget);
                _widgetUpdate();
            }
        }

        void TabWidget::removeChild(const std::shared_ptr<IObject>& value)
        {
            DJV_PRIVATE_PTR();
            if (auto widget = std::dynamic_pointer_cast<Widget>(value))
            {
                const auto i = std::find(p.widgets.begin(), p.widgets.end(), widget);
                if (i != p.widgets.end())
                {
                    p.widgets.erase(i);
                }
                const auto j = p.widgetToText.find(widget);
                if (j != p.widgetToText.end())
                {
                    p.widgetToText.erase(j);
                }
                p.soloLayout->removeChild(widget);
                _widgetUpdate();
            }
        }

        void TabWidget::clearChildren()
        {
            DJV_PRIVATE_PTR();
            p.widgets.clear();
            p.widgetToText.clear();
            p.soloLayout->clearChildren();
            _widgetUpdate();
        }

        float TabWidget::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void TabWidget::_preLayoutEvent(System::Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void TabWidget::_layoutEvent(System::Event::Layout& event)
        {
            const Math::BBox2f& g = getGeometry();
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(g, style));
        }

        void TabWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            std::vector<std::string> tabs;
            for (const auto& i : p.widgets)
            {
                std::string text;
                const auto j = p.widgetToText.find(i);
                if (j != p.widgetToText.end())
                {
                    text = j->second;
                }
                tabs.push_back(text);
            }
            p.tabBar->setTabs(tabs);
            const auto i = std::find(p.widgets.begin(), p.widgets.end(), p.soloLayout->getCurrentWidget());
            if (i != p.widgets.end())
            {
                p.tabBar->setCurrentTab(i - p.widgets.begin());
            }
        }

    } // namespace UI
} // namespace djv
