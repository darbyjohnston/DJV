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
            std::map<std::shared_ptr<Widget>, int> widgetToIndex;
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
            p.layout->addSeparator();
            p.layout->addChild(p.soloLayout);
            p.layout->setStretch(p.soloLayout, RowStretch::Expand);
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

        void TabWidget::setText(const std::shared_ptr<Widget>& widget, const std::string& text)
        {
            DJV_PRIVATE_PTR();
            p.widgetToText[widget] = text;
            const auto i = p.widgetToIndex.find(widget);
            if (i != p.widgetToIndex.end())
            {
                _p->tabBar->setText(i->second, text);
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

        void TabWidget::closeCurrentTab()
        {
            //! \todo Implement me!
        }

        void TabWidget::addChild(const std::shared_ptr<IObject>& value)
        {
            DJV_PRIVATE_PTR();
            if (auto widget = std::dynamic_pointer_cast<Widget>(value))
            {
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
                    p.widgets.erase(i);
                }
                const auto j = p.widgetToIndex.find(widget);
                if (j != p.widgetToIndex.end())
                {
                    p.widgetToIndex.erase(j);
                }
                const auto k = p.widgetToText.find(widget);
                if (k != p.widgetToText.end())
                {
                    p.widgetToText.erase(k);
                }
                p.soloLayout->removeChild(value);
                _widgetUpdate();
            }
        }

        void TabWidget::clearChildren()
        {
            DJV_PRIVATE_PTR();
            p.widgetToIndex.clear();
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
                const auto j = p.widgetToIndex.find(i);
                const auto k = p.widgetToText.find(i);
                if (j != p.widgetToIndex.end() && k != p.widgetToText.end())
                {
                    p.tabBar->setText(j->second, k->second);
                }
            }
            p.tabBar->setTabs(tabs);
        }

    } // namespace UI
} // namespace djv
