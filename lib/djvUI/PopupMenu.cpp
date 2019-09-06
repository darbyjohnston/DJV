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

#include <djvUI/PopupMenu.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct PopupMenu::Private
        {
            std::shared_ptr<Action> closeAction;
            std::shared_ptr<Menu> menu;
            std::shared_ptr<Button::Menu> button;
            std::shared_ptr<ValueObserver<std::string> > iconObserver;
            std::shared_ptr<ValueObserver<std::string> > textObserver;
            std::shared_ptr<ValueObserver<bool> > closeObserver;
        };

        void PopupMenu::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::PopupMenu");

            p.closeAction = Action::create();
            p.closeAction->setShortcut(GLFW_KEY_ESCAPE);
            addAction(p.closeAction);

            p.button = Button::Menu::create(Button::MenuStyle::Flat, context);
            addChild(p.button);

            auto weak = std::weak_ptr<PopupMenu>(std::dynamic_pointer_cast<PopupMenu>(shared_from_this()));
            p.button->setCheckedCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    if (value)
                    {
                        widget->open();
                    }
                    else
                    {
                        widget->close();
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
                        widget->close();
                    }
                }
            });
        }

        PopupMenu::PopupMenu() :
            _p(new Private)
        {}

        PopupMenu::~PopupMenu()
        {}

        std::shared_ptr<PopupMenu> PopupMenu::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PopupMenu>(new PopupMenu);
            out->_init(context);
            return out;
        }

        void PopupMenu::setMenu(const std::shared_ptr<Menu> & menu)
        {
            DJV_PRIVATE_PTR();
            if (p.menu)
            {
                if (auto parent = p.menu->getParent().lock())
                {
                    parent->removeChild(p.menu);
                }
                p.menu->setCloseCallback(nullptr);
                p.iconObserver.reset();
                p.textObserver.reset();
            }

            p.menu = menu;

            if (p.menu)
            {
                addChild(p.menu);

                auto weak = std::weak_ptr<PopupMenu>(std::dynamic_pointer_cast<PopupMenu>(shared_from_this()));
                menu->setCloseCallback(
                    [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->close();
                    }
                });

                p.iconObserver = ValueObserver<std::string>::create(
                    p.menu->observeIcon(),
                    [weak](const std::string & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->button->setIcon(value);
                    }
                });

                p.textObserver = ValueObserver<std::string>::create(
                    p.menu->observeText(),
                    [weak](const std::string & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->button->setText(value);
                    }
                });
            }
        }

        void PopupMenu::open()
        {
            DJV_PRIVATE_PTR();
            if (p.menu)
            {
                p.closeAction->setEnabled(true);
                p.menu->popup(p.button);
                p.button->setChecked(true);
            }
        }

        void PopupMenu::close()
        {
            DJV_PRIVATE_PTR();
            if (p.menu)
            {
                p.closeAction->setEnabled(false);
                p.menu->close();
                p.button->setChecked(false);
            }
        }

        void PopupMenu::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->button->getMinimumSize());
        }

        void PopupMenu::_layoutEvent(Event::Layout & event)
        {
            _p->button->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv
