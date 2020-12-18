// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ToolSystem.h>

#include <djvViewApp/ToolDrawer.h>
#include <djvViewApp/WindowSettings.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ShortcutDataFunc.h>

#include <djvSystem/Context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t messagesMax = 100;
            
        } // namespace

        bool CurrentTool::operator == (const CurrentTool& other) const
        {
            return system == other.system &&
                action == other.action;
        }
        
        struct ToolSystem::Private
        {
            std::vector<std::shared_ptr<IViewAppSystem> > toolSystems;
            std::vector<std::shared_ptr<UI::Action> > toolActions;
            std::shared_ptr<Observer::ValueSubject<CurrentTool> > currentTool;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> actionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<Observer::Value<bool> > showToolsObserver;
        };

        void ToolSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::ToolSystem", context);
            DJV_PRIVATE_PTR();

            p.currentTool = Observer::ValueSubject<CurrentTool>::create();

            std::map<std::string, std::shared_ptr<IViewAppSystem> > toolSystems;
            std::map<std::string, std::shared_ptr<UI::Action> > toolActions;
            auto systems = context->getSystemsT<IViewAppSystem>();
            for (const auto& i : systems)
            {
                for (const auto& j : i->getToolActionData())
                {
                    toolSystems[j.sortKey] = i;
                    toolActions[j.sortKey] = j.action;
                }
            }
            for (const auto& i : toolSystems)
            {
                p.toolSystems.push_back(i.second);
            }
            for (const auto& i : toolActions)
            {
                p.toolActions.push_back(i.second);
            }

            p.actions["Show"] = UI::Action::create();
            p.actions["Show"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Show"]->setIcon("djvIconDrawerRight");

            p.actionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.actionGroup->setActions(p.toolActions);

            _addShortcut(DJV_TEXT("shortcut_tools"), GLFW_KEY_T);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Show"]);
            p.menu->addSeparator();
            for (const auto& i : toolActions)
            {
                p.menu->addAction(i.second);
            }

            _textUpdate();
            _shortcutsUpdate();

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.actions["Show"]->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto windowSettings = settingsSystem->getSettingsT<WindowSettings>();
                        windowSettings->setShowTools(value);
                    }
                });

            auto weak = std::weak_ptr<ToolSystem>(std::dynamic_pointer_cast<ToolSystem>(shared_from_this()));
            p.actionGroup->setExclusiveCallback(
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_setCurrentTool(value, true);
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto windowSettings = settingsSystem->getSettingsT<WindowSettings>();
            p.showToolsObserver = Observer::Value<bool>::create(
                windowSettings->observeShowTools(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->actions["Show"]->setChecked(value);
                    }
                });
        }

        ToolSystem::ToolSystem() :
            _p(new Private)
        {}

        ToolSystem::~ToolSystem()
        {}

        std::shared_ptr<ToolSystem> ToolSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<ToolSystem>();
            if (!out)
            {
                out = std::shared_ptr<ToolSystem>(new ToolSystem);
                out->_init(context);
            }
            return out;
        }

        const std::vector<std::shared_ptr<IViewAppSystem> >& ToolSystem::getToolSystems() const
        {
            return _p->toolSystems;
        }

        const std::vector<std::shared_ptr<UI::Action> >& ToolSystem::getToolActions() const
        {
            return _p->toolActions;
        }

        std::shared_ptr<Core::Observer::IValueSubject<CurrentTool> > ToolSystem::observeCurrentTool() const
        {
            return _p->currentTool;
        }

        void ToolSystem::setCurrentTool(const CurrentTool& value)
        {
            DJV_PRIVATE_PTR();
            int index = -1;
            const size_t systemsSize = p.toolSystems.size();
            const size_t actionsSize = p.toolActions.size();
            for (size_t i = 0; i < systemsSize && i < actionsSize; ++i)
            {
                if (CurrentTool({ p.toolSystems[i], p.toolActions[i] }) == value)
                {
                    index = static_cast<int>(i);
                    break;
                }
            }
            _setCurrentTool(index, false);
        }

        std::shared_ptr<UI::Widget> ToolSystem::createToolDrawer()
        {
            std::shared_ptr<UI::Widget> out;
            if (auto context = getContext().lock())
            {
                out = ToolDrawer::create(context);
            }
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ToolSystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<MenuData> ToolSystem::getMenuData() const
        {
            return
            {
                { _p->menu, "H" }
            };
        }

        void ToolSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.toolActions.size())
            {
                p.actions["Show"]->setText(_getText(DJV_TEXT("menu_tools_show")));
                p.actions["Show"]->setTooltip(_getText(DJV_TEXT("menu_tools_show_tooltip")));

                p.menu->setText(_getText(DJV_TEXT("menu_tools")));
            }
        }

        void ToolSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Show"]->setShortcuts(_getShortcuts("shortcut_tools"));
            }
        }
        
        void ToolSystem::_setCurrentTool(int value, bool autoHide)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                CurrentTool currentTool;
                auto& systems = p.toolSystems;
                auto& actions = p.toolActions;
                if (value >= 0 && value < systems.size() && value < actions.size())
                {
                    currentTool.system = systems[value];
                    currentTool.action = actions[value];
                }
                p.currentTool->setIfChanged(currentTool);
                p.actionGroup->setChecked(value);
                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                auto windowSettings = settingsSystem->getSettingsT<WindowSettings>();
                windowSettings->setShowTools(autoHide ? value != -1 : true);
            }
        }

    } // namespace ViewApp
} // namespace djv

