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

        void ComboBox::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ComboBox");
            setHAlign(HAlign::Left);
            setVAlign(VAlign::Center);

            p.closeAction = Action::create();
            p.closeAction->setShortcut(GLFW_KEY_ESCAPE);
            addAction(p.closeAction);

            p.actionGroup = ActionGroup::create(ButtonType::Radio);

            p.menu = Menu::create(context);
            p.menu->setMinimumSizeRole(MetricsRole::Menu);
            addChild(p.menu);

            p.button = Button::Menu::create(Button::MenuStyle::ComboBox, context);
            p.button->setPopupIcon("djvIconPopupMenu");
            p.button->setBackgroundRole(ColorRole::Button);
            p.button->setBorderColorRole(ColorRole::BorderButton);
            addChild(p.button);

            _updateCurrentItem(Callback::Suppress);

            auto weak = std::weak_ptr<ComboBox>(std::dynamic_pointer_cast<ComboBox>(shared_from_this()));
            p.actionGroup->setRadioCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    widget->setCurrentItem(value);
                    widget->_p->menu->close();
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

            auto contextWeak = std::weak_ptr<Context>(context);
            p.button->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (!value)
                            {
                                widget->close();
                            }
                            if (value && widget->_p->currentItem >= 0 && widget->_p->currentItem < widget->_p->items.size())
                            {
                                widget->open();
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
                        widget->_p->menu->close();
                    }
                }
            });
        }

        ComboBox::ComboBox() :
            _p(new Private)
        {}

        ComboBox::~ComboBox()
        {}

        std::shared_ptr<ComboBox> ComboBox::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ComboBox>(new ComboBox);
            out->_init(context);
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
        }

        void ComboBox::addItem(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            p.items.push_back(value);
            auto action = Action::create();
            action->setText(value);
            p.actionGroup->addAction(action);
            p.menu->addAction(action);
        }

        void ComboBox::clearItems(Callback callback)
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

        int ComboBox::getCurrentItem() const
        {
            return _p->currentItem;
        }

        void ComboBox::setCurrentItem(int value, Callback callback)
        {
            DJV_PRIVATE_PTR();
            const int tmp = Math::clamp(value, 0, static_cast<int>(p.items.size()) - 1);
            if (tmp == p.currentItem)
                return;
            p.currentItem = tmp;
            _updateCurrentItem(callback);
        }

        void ComboBox::firstItem(Callback callback)
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size)
            {
                setCurrentItem(0, callback);
            }
        }

        void ComboBox::lastItem(Callback callback)
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size)
            {
                setCurrentItem(size - 1, callback);
            }
        }

        void ComboBox::prevItem(Callback callback)
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size && p.currentItem > 0)
            {
                setCurrentItem(p.currentItem - 1, callback);
            }
        }

        void ComboBox::nextItem(Callback callback)
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size && p.currentItem >= 0 && p.currentItem < size - 1)
            {
                setCurrentItem(p.currentItem + 1, callback);
            }
        }

        bool ComboBox::isOpen() const
        {
            return _p->menu->isOpen();
        }

        void ComboBox::open()
        {
            DJV_PRIVATE_PTR();
            if (p.currentItem >= 0 && p.currentItem < p.items.size())
            {
                if (auto window = getWindow())
                {
                    takeTextFocus();
                    p.closeAction->setEnabled(true);
                    p.menu->popup(p.button);
                }
            }
        }

        void ComboBox::close()
        {
            _p->menu->close();
        }

        void ComboBox::setFont(int index, const std::string & font)
        {
            DJV_PRIVATE_PTR();
            const auto & actions = p.actionGroup->getActions();
            if (index >= 0 && index < actions.size())
            {
                actions[index]->setFont(font);
            }
        }

        void ComboBox::setFontSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            p.button->setFontSizeRole(value);
        }

        void ComboBox::setCallback(const std::function<void(int)> & value)
        {
            _p->callback = value;
        }

        void ComboBox::_preLayoutEvent(Event::PreLayout & event)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->button->getMinimumSize());
        }

        void ComboBox::_layoutEvent(Event::Layout & event)
        {
            _p->button->setGeometry(getGeometry());
        }

        void ComboBox::_keyPressEvent(Event::KeyPress& event)
        {
            DJV_PRIVATE_PTR();
            const int currentItem = p.currentItem;
            switch (event.getKey())
            {
            case GLFW_KEY_ENTER:
            case GLFW_KEY_SPACE:
                event.accept();
                if (isOpen())
                {
                    close();
                }
                else
                {
                    open();
                }
                break;
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
            case GLFW_KEY_ESCAPE:
                if (p.menu->isOpen())
                {
                    event.accept();
                    p.menu->close();
                }
                else if (hasTextFocus())
                {
                    event.accept();
                    releaseTextFocus();
                }
                break;
            }
            if (currentItem != p.currentItem && p.callback)
            {
                p.callback(p.currentItem);
            }
        }

        void ComboBox::_textFocusEvent(Event::TextFocus&)
        {
            _p->button->setBorderColorRole(ColorRole::TextFocus);
        }

        void ComboBox::_textFocusLostEvent(Event::TextFocusLost&)
        {
            _p->button->setBorderColorRole(ColorRole::BorderButton);
        }

        void ComboBox::_updateItems()
        {
            DJV_PRIVATE_PTR();
            p.actionGroup->clearActions();
            p.menu->clearActions();
            for (const auto & i : p.items)
            {
                auto action = Action::create();
                action->setText(i);
                p.actionGroup->addAction(action);
                p.menu->addAction(action);
            }
        }

        void ComboBox::_updateCurrentItem(Callback callback)
        {
            DJV_PRIVATE_PTR();
            if (p.currentItem >= 0 && p.currentItem < p.items.size())
            {
                p.actionGroup->setChecked(p.currentItem, true, callback);
                p.button->setText(p.items[p.currentItem]);
            }
            else
            {
                p.actionGroup->setChecked(-1, true, callback);
            }
        }

    } // namespace UI
} // namespace djv
