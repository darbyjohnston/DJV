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

#include <djvViewLib/HelpSystem.h>

#include <djvViewLib/AboutDialog.h>
#include <djvViewLib/SystemLogDialog.h>

#include <djvUI/Action.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvCore/Context.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct HelpSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<AboutDialog> aboutDialog;
            std::shared_ptr<SystemLogDialog> systemLogDialog;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void HelpSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::HelpSystem", context);

            DJV_PRIVATE_PTR();
            //! \todo Implement me!
            p.actions["Documentation"] = UI::Action::create();
            p.actions["Documentation"]->setEnabled(false);
            p.actions["About"] = UI::Action::create();
            p.actions["SystemLog"] = UI::Action::create();

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Documentation"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["About"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["SystemLog"]);

            p.aboutDialog = AboutDialog::create(context);
            p.systemLogDialog = SystemLogDialog::create(context);

            auto weak = std::weak_ptr<HelpSystem>(std::dynamic_pointer_cast<HelpSystem>(shared_from_this()));
            p.aboutDialog->setCloseCallback(
                [weak, context]
            {
                if (auto system = weak.lock())
                {
                    system->_p->aboutDialog->hide();
                    system->_p->aboutDialog->setParent(nullptr);
                }
            });

            p.systemLogDialog->setCloseCallback(
                [weak, context]
            {
                if (auto system = weak.lock())
                {
                    system->_p->systemLogDialog->hide();
                    system->_p->systemLogDialog->setParent(nullptr);
                }
            });

            p.clickedObservers["About"] = ValueObserver<bool>::create(
                p.actions["About"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
                        {
                            if (auto window = windowSystem->observeCurrentWindow()->get())
                            {
                                window->addWidget(system->_p->aboutDialog);
                                system->_p->aboutDialog->show();
                            }
                        }
                    }
                }
            });

            p.clickedObservers["SystemLog"] = ValueObserver<bool>::create(
                p.actions["SystemLog"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
                        {
                            if (auto window = windowSystem->observeCurrentWindow()->get())
                            {
                                if (value)
                                {
                                    system->_p->systemLogDialog->reloadLog();
                                }
                                else
                                {
                                    system->_p->systemLogDialog->clearLog();
                                }
                                window->addWidget(system->_p->systemLogDialog);
                                system->_p->systemLogDialog->show();
                            }
                        }
                    }
                }
            });
        }

        HelpSystem::HelpSystem() :
            _p(new Private)
        {}

        HelpSystem::~HelpSystem()
        {}

        std::shared_ptr<HelpSystem> HelpSystem::create(Context * context)
        {
            auto out = std::shared_ptr<HelpSystem>(new HelpSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > HelpSystem::getActions()
        {
            return _p->actions;
        }

        MenuData HelpSystem::getMenu()
        {
            return
            {
                _p->menu,
                "F"
            };
        }

        void HelpSystem::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.actions["Documentation"]->setTitle(_getText(DJV_TEXT("Documentation")));
            p.actions["Documentation"]->setTooltip(_getText(DJV_TEXT("Documentation tooltip")));
            p.actions["About"]->setTitle(_getText(DJV_TEXT("About")));
            p.actions["About"]->setTooltip(_getText(DJV_TEXT("About tooltip")));
            p.actions["SystemLog"]->setTitle(_getText(DJV_TEXT("System Log")));
            p.actions["SystemLog"]->setTooltip(_getText(DJV_TEXT("System log tooltip")));

            p.menu->setText(_getText(DJV_TEXT("Help")));
        }

    } // namespace ViewLib
} // namespace djv

