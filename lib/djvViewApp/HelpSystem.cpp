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

#include <djvViewApp/HelpSystem.h>

#include <djvViewApp/AboutDialog.h>

#include <djvDesktopApp/GLFWSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>

#include <djvCore/Context.h>
#include <djvCore/OS.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct HelpSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<AboutDialog> aboutDialog;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
        };

        void HelpSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::HelpSystem", context);
            DJV_PRIVATE_PTR();

            p.actions["Documentation"] = UI::Action::create();
            p.actions["About"] = UI::Action::create();

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Documentation"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["About"]);

            auto weak = std::weak_ptr<HelpSystem>(std::dynamic_pointer_cast<HelpSystem>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.actionObservers["Documentation"] = ValueObserver<bool>::create(
                p.actions["Documentation"]->observeClicked(),
                [weak, contextWeak](bool value)
                {
                    if (value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto system = weak.lock())
                            {
                                auto resourceSystem = context->getSystemT<ResourceSystem>();
                                auto path = resourceSystem->getPath(Core::FileSystem::ResourcePath::Documentation);
                                try
                                {
                                    OS::openURL(path.get());
                                }
                                catch (const std::exception& e)
                                {
                                    system->_log(e.what(), LogLevel::Error);
                                }
                            }
                        }
                    }
                });
                
            p.actionObservers["About"] = ValueObserver<bool>::create(
                p.actions["About"]->observeClicked(),
                [weak, contextWeak](bool value)
            {
                if (value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (system->_p->aboutDialog)
                            {
                                system->_p->aboutDialog->close();
                                system->_p->aboutDialog.reset();
                            }
                            system->_p->aboutDialog = AboutDialog::create(context);
                            system->_p->aboutDialog->setCloseCallback(
                                [weak]
                                {
                                    if (auto system = weak.lock())
                                    {
                                        system->_p->aboutDialog->close();
                                        system->_p->aboutDialog.reset();
                                    }
                                });
                            system->_p->aboutDialog->show();
                        }
                    }
                }
            });
        }

        HelpSystem::HelpSystem() :
            _p(new Private)
        {}

        HelpSystem::~HelpSystem()
        {
            DJV_PRIVATE_PTR();
            if (p.aboutDialog)
            {
                p.aboutDialog->close();
            }
        }

        std::shared_ptr<HelpSystem> HelpSystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<HelpSystem>(new HelpSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > HelpSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData HelpSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "I"
            };
        }

        void HelpSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            const auto i = p.actions.find(value);
            if (i != p.actions.end())
            {
                i->second->setChecked(false);
            }
            IViewSystem::_closeWidget(value);
        }

        void HelpSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Documentation"]->setText(_getText(DJV_TEXT("Documentation")));
                p.actions["About"]->setText(_getText(DJV_TEXT("About")));

                p.menu->setText(_getText(DJV_TEXT("Help")));
            }
        }

    } // namespace ViewApp
} // namespace djv

