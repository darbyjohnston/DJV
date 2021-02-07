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
            std::vector<std::string> text;
            std::vector<std::string> tooltips;
            std::vector<std::shared_ptr<Widget> > widgets;
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
                const auto j = i - p.widgets.begin();
                p.text.resize(j + 1);
                p.text[j] = text;
                p.tabBar->setText(j, text);
            }
        }

        void TabWidget::setTooltip(const std::shared_ptr<Widget>& widget, const std::string& tooltip)
        {
            DJV_PRIVATE_PTR();
            const auto i = std::find(p.widgets.begin(), p.widgets.end(), widget);
            if (i != p.widgets.end())
            {
                const auto j = i - p.widgets.begin();
                p.tooltips.resize(j + 1);
                p.tooltips[j] = tooltip;
                p.tabBar->setTooltip(j, tooltip);
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
                p.text.push_back(std::string());
                p.tooltips.push_back(std::string());
                p.widgets.push_back(widget);
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
                    const auto j = i - p.widgets.begin();
                    if (j < p.text.size())
                    {
                        p.text.erase(p.text.begin() + j);
                    }
                    if (j < p.tooltips.size())
                    {
                        p.tooltips.erase(p.tooltips.begin() + j);
                    }
                    p.widgets.erase(i);
                }
                p.soloLayout->removeChild(widget);
                _widgetUpdate();
            }
        }

        void TabWidget::clearChildren()
        {
            DJV_PRIVATE_PTR();
            p.text.clear();
            p.tooltips.clear();
            p.widgets.clear();
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
            p.tabBar->setTabs(p.text);
            for (size_t i = 0; i < p.tooltips.size(); ++i)
            {
                p.tabBar->setTooltip(static_cast<int>(i), p.tooltips[i]);
            }
            const auto i = std::find(p.widgets.begin(), p.widgets.end(), p.soloLayout->getCurrentWidget());
            if (i != p.widgets.end())
            {
                const auto j = i - p.widgets.begin();
                p.tabBar->setCurrentTab(static_cast<int>(j));
            }
        }

    } // namespace UI
} // namespace djv
