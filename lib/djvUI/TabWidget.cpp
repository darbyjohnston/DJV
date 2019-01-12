//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
            std::shared_ptr<Layout::Vertical> layout;
            std::shared_ptr<Layout::Solo> soloLayout;
            std::function<void(int)> callback;
        };

        void TabWidget::_init(Context * context)
        {
            Widget::_init(context);
            
            setClassName("djv::UI::TabWidget");

            DJV_PRIVATE_PTR();
            p.tabBar = TabBar::create(context);

            p.soloLayout = Layout::Solo::create(context);

            p.layout = Layout::Vertical::create(context);
            p.layout->setSpacing(Style::MetricsRole::None);
            p.layout->addWidget(p.tabBar);
            p.layout->addWidget(p.soloLayout, Layout::RowStretch::Expand);
            p.layout->setParent(shared_from_this());

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
        }

        TabWidget::TabWidget() :
            _p(new Private)
        {}

        TabWidget::~TabWidget()
        {}

        std::shared_ptr<TabWidget> TabWidget::create(Context * context)
        {
            auto out = std::shared_ptr<TabWidget>(new TabWidget);
            out->_init(context);
            return out;
        }

        size_t TabWidget::getTabCount() const
        {
            return _p->tabBar->getTabCount();
        }

        size_t TabWidget::addTab(const std::string & text, const std::shared_ptr<Widget>& value)
        {
            DJV_PRIVATE_PTR();
            const size_t out = p.tabBar->addTab(text);
            p.soloLayout->addWidget(value);
            return out;
        }

        void TabWidget::removeTab(const std::shared_ptr<Widget>& value)
        {
            size_t i = 0;
            DJV_PRIVATE_PTR();
            const auto & children = p.soloLayout->getChildrenT<Widget>();
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
            p.soloLayout->removeWidget(value);
        }

        void TabWidget::clearTabs()
        {
            _p->soloLayout->clearWidgets();
        }

        int TabWidget::getCurrentTab() const
        {
            return _p->tabBar->getCurrentTab();
        }

        void TabWidget::setCurrentTab(int value)
        {
            _p->tabBar->setCurrentTab(value);
        }

        void TabWidget::setCurrentTabCallback(const std::function<void(int)>& value)
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

        void TabWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void TabWidget::_layoutEvent(Event::Layout& event)
        {
            if (auto style = _getStyle().lock())
            {
                const BBox2f & g = getGeometry();
                _p->layout->setGeometry(getMargin().bbox(g, style));
            }
        }

    } // namespace UI
} // namespace djv
