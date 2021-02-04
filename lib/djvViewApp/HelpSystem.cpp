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

#include <djvSystem/Context.h>
#include <djvSystem/ResourceSystem.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/OSFunc.h>

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
        };

        void HelpSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::HelpSystem", context);
            DJV_PRIVATE_PTR();

            p.actions["Documentation"] = UI::Action::create();
            p.actions["About"] = UI::Action::create();

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Documentation"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["About"]);

            _textUpdate();
            
            auto weak = std::weak_ptr<HelpSystem>(std::dynamic_pointer_cast<HelpSystem>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.actions["Documentation"]->setClickedCallback(
                [weak, contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            auto resourceSystem = context->getSystemT<System::ResourceSystem>();
                            auto path = resourceSystem->getPath(System::File::ResourcePath::Documentation);
                            try
                            {
                                OS::openURL(path.get());
                            }
                            catch (const std::exception& e)
                            {
                                system->_log(e.what(), System::LogLevel::Error);
                            }
                        }
                    }
                });
                
            p.actions["About"]->setClickedCallback(
                [weak, contextWeak]
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
            });

            _logInitTime();
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

        std::shared_ptr<HelpSystem> HelpSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<HelpSystem>();
            if (!out)
            {
                out = std::shared_ptr<HelpSystem>(new HelpSystem);
                out->_init(context);
            }
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > HelpSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData HelpSystem::getMenuData() const
        {
            return
            {
                { _p->menu },
                15
            };
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

