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

#include <djvUI/ListWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ListWidget::Private
        {
            std::vector<std::string> items;
            int currentItem = -1;
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<VerticalLayout> layout;
            std::shared_ptr<ScrollWidget> scrollWidget;
            std::function<void(int)> callback;
        };

        void ListWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ListWidget");

            p.buttonGroup = ButtonGroup::create(ButtonType::Radio);

            p.layout = VerticalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);

            p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
            p.scrollWidget->addChild(p.layout);
            addChild(p.scrollWidget);

            _updateCurrentItem(Callback::Suppress);

            auto weak = std::weak_ptr<ListWidget>(std::dynamic_pointer_cast<ListWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    widget->setCurrentItem(value);
                    if (widget->_p->callback)
                    {
                        widget->_p->callback(value);
                    }
                }
            });
        }

        ListWidget::ListWidget() :
            _p(new Private)
        {}

        ListWidget::~ListWidget()
        {}

        std::shared_ptr<ListWidget> ListWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ListWidget>(new ListWidget);
            out->_init(context);
            return out;
        }

        const std::vector<std::string> & ListWidget::getItems() const
        {
            return _p->items;
        }

        void ListWidget::setItems(const std::vector<std::string> & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.items)
                return;
            p.items = value;
            _updateItems();
        }

        void ListWidget::addItem(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.items.push_back(value);
                auto button = ListButton::create(context);
                button->setText(value);
                p.buttonGroup->addButton(button);
                p.layout->addChild(button);
            }
        }

        void ListWidget::clearItems(Callback callback)
        {
            DJV_PRIVATE_PTR();
            if (p.items.size())
            {
                p.items.clear();
                _updateItems();
                p.currentItem = -1;
                _updateCurrentItem(callback);
            }
        }

        int ListWidget::getCurrentItem() const
        {
            return _p->currentItem;
        }

        void ListWidget::setCurrentItem(int value, Callback callback)
        {
            DJV_PRIVATE_PTR();
            const int tmp = Math::clamp(value, 0, static_cast<int>(p.items.size()) - 1);
            if (tmp == p.currentItem)
                return;
            p.currentItem = tmp;
            _updateCurrentItem(callback);
        }

        void ListWidget::firstItem(Callback callback)
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size)
            {
                setCurrentItem(0, callback);
            }
        }

        void ListWidget::lastItem(Callback callback)
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size)
            {
                setCurrentItem(size - 1, callback);
            }
        }

        void ListWidget::prevItem(Callback callback)
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size && p.currentItem > 0)
            {
                setCurrentItem(p.currentItem - 1, callback);
            }
        }

        void ListWidget::nextItem(Callback callback)
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size && p.currentItem >= 0 && p.currentItem < size - 1)
            {
                setCurrentItem(p.currentItem + 1, callback);
            }
        }

        void ListWidget::setCallback(const std::function<void(int)> & value)
        {
            _p->callback = value;
        }

        void ListWidget::setBorder(bool value)
        {
            _p->scrollWidget->setBorder(value);
        }

        void ListWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            const auto& style = _getStyle();
            const float sa = style->getMetric(MetricsRole::ScrollArea);
            glm::vec2 size = _p->scrollWidget->getMinimumSize();
            size.x = std::max(size.x, sa);
            size.y = std::max(size.y, sa);
            _setMinimumSize(size);
        }

        void ListWidget::_layoutEvent(Event::Layout & event)
        {
            _p->scrollWidget->setGeometry(getGeometry());
        }

        void ListWidget::_keyPressEvent(Event::KeyPress& event)
        {
            DJV_PRIVATE_PTR();
            const int currentItem = p.currentItem;
            switch (event.getKey())
            {
            case GLFW_KEY_HOME:
                event.accept();
                firstItem(Callback::Trigger);
                break;
            case GLFW_KEY_END:
                event.accept();
                lastItem(Callback::Trigger);
                break;
            case GLFW_KEY_UP:
                event.accept();
                prevItem(Callback::Trigger);
                break;
            case GLFW_KEY_DOWN:
                event.accept();
                nextItem(Callback::Trigger);
                break;
            }
            if (currentItem != p.currentItem && p.callback)
            {
                p.callback(p.currentItem);
            }
        }

        void ListWidget::_updateItems()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.buttonGroup->clearButtons();
                p.layout->clearChildren();
                for (const auto& i : p.items)
                {
                    auto button = ListButton::create(context);
                    button->setText(i);
                    p.buttonGroup->addButton(button);
                    p.layout->addChild(button);
                }
            }
        }

        void ListWidget::_updateCurrentItem(Callback callback)
        {
            DJV_PRIVATE_PTR();
            if (p.currentItem >= 0 && p.currentItem < p.items.size())
            {
                p.buttonGroup->setChecked(p.currentItem, true, callback);
            }
            else
            {
                p.buttonGroup->setChecked(-1, true, callback);
            }
        }

    } // namespace UI
} // namespace djv
