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
            std::shared_ptr<TabBar> tabBar;
            std::shared_ptr<VerticalLayout> layout;
            std::shared_ptr<SoloLayout> soloLayout;
            std::function<void(int)> callback;
            std::function<void(size_t)> removedCallback;
        };

        void TabWidget::_init(const std::shared_ptr<Context>& context)
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
            addChild(p.layout);

            auto weak = std::weak_ptr<TabWidget>(std::dynamic_pointer_cast<TabWidget>(shared_from_this()));
            p.tabBar->setCurrentTabCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->soloLayout->setCurrentIndex(value);
                    if (widget->_p->callback)
                    {
                        widget->_p->callback(value);
                    }
                }
            });
            p.tabBar->setTabRemovedCallback(
                [weak](size_t value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->soloLayout->removeChild(widget->_p->soloLayout->getChildWidgets()[value]);
                }
            });
        }

        TabWidget::TabWidget() :
            _p(new Private)
        {}

        TabWidget::~TabWidget()
        {}

        std::shared_ptr<TabWidget> TabWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TabWidget>(new TabWidget);
            out->_init(context);
            return out;
        }

        size_t TabWidget::getTabCount() const
        {
            return _p->tabBar->getTabCount();
        }

        size_t TabWidget::addTab(const std::string & text, const std::shared_ptr<Widget> & value)
        {
            DJV_PRIVATE_PTR();
            const size_t out = p.tabBar->addTab(text);
            p.soloLayout->addChild(value);
            return out;
        }

        void TabWidget::removeTab(const std::shared_ptr<Widget> & value)
        {
            size_t i = 0;
            DJV_PRIVATE_PTR();
            const auto & children = p.soloLayout->getChildWidgets();
            for (const auto & child : children)
            {
                if (child == value)
                {
                    break;
                }
                ++i;
            }
            if (i < children.size())
            {
                p.tabBar->removeTab(i);
            }
            p.soloLayout->removeChild(value);
        }

        void TabWidget::clearTabs()
        {
            _p->soloLayout->clearChildren();
        }

        void TabWidget::setTabRemovedCallback(const std::function<void(size_t)> & callback)
        {
            _p->removedCallback = callback;
        }

        void TabWidget::setText(size_t index, const std::string & value)
        {
            _p->tabBar->setText(index, value);
        }

        int TabWidget::getCurrentTab() const
        {
            return _p->tabBar->getCurrentTab();
        }

        void TabWidget::setCurrentTab(int value)
        {
            DJV_PRIVATE_PTR();
            p.tabBar->setCurrentTab(value);
            p.soloLayout->setCurrentIndex(value);
        }

        void TabWidget::setCurrentTabCallback(const std::function<void(int)> & value)
        {
            _p->callback = value;
        }

        void TabWidget::closeCurrentTab()
        {

        }

        float TabWidget::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void TabWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void TabWidget::_layoutEvent(Event::Layout & event)
        {
            const BBox2f & g = getGeometry();
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(g, style));
        }

    } // namespace UI
} // namespace djv
