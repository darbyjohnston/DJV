// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ToolSystem.h>

#include <djvViewApp/ToolDrawer.h>
#include <djvViewApp/ToolSettings.h>

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
            return action == other.action &&
                system == other.system;
        }
        
        struct ToolSystem::Private
        {
            std::shared_ptr<ToolSettings> settings;
            std::vector<std::shared_ptr<UI::Action> > toolActions;
            std::map<std::shared_ptr<UI::Action>, std::shared_ptr<IViewAppSystem> > toolSystems;
            std::shared_ptr<Observer::ValueSubject<CurrentTool> > currentTool;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> actionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<Observer::Value<int> > currentToolObserver;
        };

        void ToolSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::ToolSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = ToolSettings::create(context);

            p.currentTool = Observer::ValueSubject<CurrentTool>::create();

            std::map<int, std::shared_ptr<IViewAppSystem> > toolSystems;
            std::map<int, std::pair<std::shared_ptr<IViewAppSystem>, std::vector<std::shared_ptr<UI::Action> > > > toolActions;
            auto systems = context->getSystemsT<IViewAppSystem>();
            for (const auto& i : systems)
            {
                const auto actionData = i->getToolActionData();
                if (!actionData.actions.empty())
                {
                    toolActions[actionData.sortKey] = std::make_pair(i, actionData.actions);
                }
            }
            for (const auto& i : toolActions)
            {
                for (const auto& j : i.second.second)
                {
                    p.toolActions.push_back(j);
                    p.toolSystems[j] = i.second.first;
                }
            }

            p.actionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.actionGroup->setActions(p.toolActions);

            _addShortcut(DJV_TEXT("shortcut_tools"), GLFW_KEY_T);

            p.menu = UI::Menu::create(context);
            for (const auto& i : toolActions)
            {
                for (const auto& j : i.second.second)
                {
                    p.menu->addAction(j);
                }
            }

            _textUpdate();
            _shortcutsUpdate();

            auto weak = std::weak_ptr<ToolSystem>(std::dynamic_pointer_cast<ToolSystem>(shared_from_this()));
            p.actionGroup->setExclusiveCallback(
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->settings->setCurrentTool(value);
                    }
                });

            p.currentToolObserver = Observer::Value<int>::create(
                p.settings->observeCurrentTool(),
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_setCurrentTool(value);
                    }
                });

            _logInitTime();
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

        std::shared_ptr<Core::Observer::IValueSubject<CurrentTool> > ToolSystem::observeCurrentTool() const
        {
            return _p->currentTool;
        }
        
        int ToolSystem::getToolIndex(const std::shared_ptr<UI::Action>& value) const
        {
            DJV_PRIVATE_PTR();
            int out = -1;
            const auto i = std::find(p.toolActions.begin(), p.toolActions.end(), value);
            if (i != p.toolActions.end())
            {
                out = i - p.toolActions.begin();
            }
            return out;
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

        MenuData ToolSystem::getMenuData() const
        {
            return
            {
                { _p->menu },
                18
            };
        }

        ActionData ToolSystem::getToolBarActionData() const
        {
            return
            {
                _p->toolActions,
                18
            };
        }

        void ToolSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.toolActions.size())
            {
                p.menu->setText(_getText(DJV_TEXT("menu_tools")));
            }
        }
        
        void ToolSystem::_setCurrentTool(int value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                CurrentTool currentTool;
                if (value >= 0 && value < p.toolActions.size())
                {
                    const auto& action = p.toolActions[value];
                    const auto i = p.toolSystems.find(action);
                    if (i != p.toolSystems.end())
                    {
                        currentTool.action = action;
                        currentTool.system = i->second;
                    }
                }
                p.currentTool->setIfChanged(currentTool);
                p.actionGroup->setChecked(value);
            }
        }

    } // namespace ViewApp
} // namespace djv

