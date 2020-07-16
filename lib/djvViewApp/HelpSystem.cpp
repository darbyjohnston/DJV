// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/HelpSystem.h>

#include <djvViewApp/AboutDialog.h>

#include <djvDesktopApp/GLFWSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

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

            _textUpdate();
            
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
                            if (!system->_p->aboutDialog)
                            {
                                system->_p->aboutDialog = AboutDialog::create(context);
                                system->_p->aboutDialog->setCloseCallback(
                                    [weak]
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            if (system->_p->aboutDialog)
                                            {
                                                system->_p->aboutDialog->close();
                                                system->_p->aboutDialog.reset();
                                            }
                                        }
                                    });
                                system->_p->aboutDialog->show();
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
                "Z"
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
                p.actions["Documentation"]->setText(_getText(DJV_TEXT("menu_help_documentation")));
                p.actions["About"]->setText(_getText(DJV_TEXT("menu_help_about")));

                p.menu->setText(_getText(DJV_TEXT("menu_help")));
            }
        }

    } // namespace ViewApp
} // namespace djv

