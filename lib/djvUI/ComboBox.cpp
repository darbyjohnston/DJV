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

#include <djvUI/ComboBox.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Border.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Window.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ComboBox::Private
        {
            std::vector<std::string> items;
            int currentItem = -1;
            std::shared_ptr<Action> closeAction;
            std::shared_ptr<ActionGroup> actionGroup;
            std::shared_ptr<Menu> menu;
            std::shared_ptr<Button::Menu> button;
            std::function<void(int)> callback;
            std::shared_ptr<ValueObserver<bool> > closeObserver;
        };

        void ComboBox::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::ComboBox");

            DJV_PRIVATE_PTR();
            p.closeAction = Action::create();
            p.closeAction->setShortcut(GLFW_KEY_ESCAPE);
            addAction(p.closeAction);

            p.actionGroup = ActionGroup::create(ButtonType::Radio);

            p.menu = Menu::create(context);
            addChild(p.menu);

            p.button = Button::Menu::create(context);
            p.button->setBorder(true);
            p.button->setBackgroundRole(ColorRole::Button);
            addChild(p.button);

            _updateCurrentItem();

            auto weak = std::weak_ptr<ComboBox>(std::dynamic_pointer_cast<ComboBox>(shared_from_this()));
            p.actionGroup->setRadioCallback(
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

            p.menu->setCloseCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->closeAction->setEnabled(false);
                    widget->_p->button->setChecked(false);
                }
            });

            p.button->setCheckedCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->menu->hide();
                    if (value && widget->_p->currentItem >= 0 && widget->_p->currentItem < widget->_p->items.size())
                    {
                        if (auto window = widget->getWindow())
                        {
                            widget->_p->closeAction->setEnabled(true);
                            widget->_p->menu->popup(weak);
                        }
                    }
                }
            });

            p.closeObserver = ValueObserver<bool>::create(
                p.closeAction->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->closeAction->setEnabled(false);
                        widget->_p->button->setChecked(false);
                        widget->_p->menu->hide();
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
            DJV_PRIVATE_PTR();
            if (value == p.items)
                return;
            p.items = value;
            _updateItems();
            setCurrentItem(p.currentItem);
        }

        void ComboBox::addItem(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            p.items.push_back(value);
            auto action = Action::create();
            action->setTitle(value);
            p.actionGroup->addAction(action);
            p.menu->addAction(action);
            if (-1 == p.currentItem)
            {
                p.currentItem = 0;
                _updateCurrentItem();
            }
        }

        void ComboBox::clearItems()
        {
            DJV_PRIVATE_PTR();
            if (p.items.size())
            {
                p.items.clear();
                _updateItems();
                p.currentItem = -1;
                _updateCurrentItem();
            }
        }

        int ComboBox::getCurrentItem() const
        {
            return _p->currentItem;
        }

        void ComboBox::setCurrentItem(int value)
        {
            DJV_PRIVATE_PTR();
            const int tmp = Math::clamp(value, 0, static_cast<int>(p.items.size()) - 1);
            if (tmp == p.currentItem)
                return;
            p.currentItem = tmp;
            _updateCurrentItem();
        }

        void ComboBox::setCallback(const std::function<void(int)> & value)
        {
            _p->callback = value;
        }

        void ComboBox::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->button->getMinimumSize());
        }

        void ComboBox::_layoutEvent(Event::Layout & event)
        {
            _p->button->setGeometry(getGeometry());
        }

        void ComboBox::_updateItems()
        {
            DJV_PRIVATE_PTR();
            p.actionGroup->clearActions();
            p.menu->clearActions();
            for (const auto & i : p.items)
            {
                auto action = Action::create();
                action->setTitle(i);
                p.actionGroup->addAction(action);
                p.menu->addAction(action);
            }
        }

        void ComboBox::_updateCurrentItem()
        {
            DJV_PRIVATE_PTR();
            if (p.currentItem >= 0 && p.currentItem < p.items.size())
            {
                p.actionGroup->setChecked(p.currentItem);
                p.button->setText(p.items[p.currentItem]);
            }
        }

    } // namespace UI
} // namespace djv
