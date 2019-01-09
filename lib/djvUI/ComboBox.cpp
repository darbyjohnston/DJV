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

#include <djvUI/ComboBox.h>

#include <djvUI/Action.h>
#include <djvUI/Border.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Window.h>

#include <djvAV/Render2DSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ComboBox::Private
        {
            std::vector<std::string> items;
            int currentItem = -1;
            std::shared_ptr<ActionGroup> actionGroup;
            std::shared_ptr<Menu> menu;
            std::shared_ptr<Button::Menu> button;
            std::shared_ptr<Layout::Overlay> overlay;
            std::function<void(int)> callback;
        };

        void ComboBox::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::ComboBox");

            _p->actionGroup = ActionGroup::create(ButtonType::Radio);

            _p->menu = Menu::create(context);

            _p->button = Button::Menu::create(context);
            _p->button->setBorder(true);
            _p->button->setBackgroundRole(Style::ColorRole::Button);
            _p->button->setParent(shared_from_this());

            _updateCurrentItem();

            auto weak = std::weak_ptr<ComboBox>(std::dynamic_pointer_cast<ComboBox>(shared_from_this()));
            _p->actionGroup->setRadioCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    widget->setCurrentItem(value);
                    widget->_p->menu->hide();
                    if (widget->_p->callback)
                    {
                        widget->_p->callback(value);
                    }
                }
            });

            _p->menu->setCloseCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->button->setChecked(false);
                }
            });

            _p->button->setCheckedCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->menu->hide();
                    if (value && widget->_p->currentItem >= 0 && widget->_p->currentItem < widget->_p->items.size())
                    {
                        if (auto window = widget->getWindow().lock())
                        {
                            const auto & g = widget->getGeometry();
                            widget->_p->menu->popup(window, weak, MenuType::ComboBox);
                        }
                    }
                }
            });
        }

        ComboBox::ComboBox() :
            _p(new Private)
        {}

        ComboBox::~ComboBox()
        {}

        std::shared_ptr<ComboBox> ComboBox::create(Context * context)
        {
            auto out = std::shared_ptr<ComboBox>(new ComboBox);
            out->_init(context);
            return out;
        }

        std::shared_ptr<ComboBox> ComboBox::create(const std::vector<std::string> & items, Context * context)
        {
            auto out = ComboBox::create(context);
            out->setItems(items);
            return out;
        }

        const std::vector<std::string> & ComboBox::getItems() const
        {
            return _p->items;
        }

        void ComboBox::setItems(const std::vector<std::string> & value)
        {
            if (value == _p->items)
                return;
            _p->items = value;
            _updateItems();
            setCurrentItem(Math::clamp(_p->currentItem, 0, static_cast<int>(_p->items.size()) - 1));
        }

        void ComboBox::addItem(const std::string & value)
        {
            _p->items.push_back(value);
            auto action = Action::create();
            action->setText(value);
            _p->actionGroup->addAction(action);
            _p->menu->addAction(action);
            if (-1 == _p->currentItem)
            {
                setCurrentItem(0);
            }
        }

        void ComboBox::clearItems()
        {
            if (_p->items.size())
            {
                _p->items.clear();
                _updateItems();
                setCurrentItem(-1);
            }
        }

        int ComboBox::getCurrentItem() const
        {
            return _p->currentItem;
        }

        void ComboBox::setCurrentItem(int value)
        {
            if (value == _p->currentItem)
                return;
            _p->currentItem = value;
            _updateCurrentItem();
        }

        void ComboBox::setCallback(const std::function<void(int)> & value)
        {
            _p->callback = value;
        }

        void ComboBox::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->button->getMinimumSize());
        }

        void ComboBox::_layoutEvent(Event::Layout& event)
        {
            _p->button->setGeometry(getGeometry());
        }

        void ComboBox::_updateItems()
        {
            _p->actionGroup->clearActions();
            _p->menu->clearActions();
            for (const auto & i : _p->items)
            {
                auto action = Action::create();
                action->setText(i);
                _p->actionGroup->addAction(action);
                _p->menu->addAction(action);
            }
        }

        void ComboBox::_updateCurrentItem()
        {
            if (_p->currentItem >= 0 && _p->currentItem < _p->items.size())
            {
                _p->actionGroup->setChecked(_p->currentItem);
                _p->button->setText(_p->items[_p->currentItem]);
            }
        }

    } // namespace UI
} // namespace djv
