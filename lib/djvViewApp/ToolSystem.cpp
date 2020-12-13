// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ToolSystem.h>

#include <djvViewApp/ToolDrawer.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>

#include <djvSystem/Context.h>

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
            std::shared_ptr<UI::ActionGroup> actionGroup;
            std::shared_ptr<UI::Menu> menu;
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

            p.actionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.actionGroup->setActions(p.toolActions);

            p.menu = UI::Menu::create(context);
            for (const auto& i : toolActions)
            {
                p.menu->addAction(i.second);
            }

            _textUpdate();

            auto weak = std::weak_ptr<ToolSystem>(std::dynamic_pointer_cast<ToolSystem>(shared_from_this()));
            p.actionGroup->setExclusiveCallback(
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setCurrentTool(value);
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

        void ToolSystem::setCurrentTool(int value)
        {
            DJV_PRIVATE_PTR();
            auto currentTool = p.currentTool->get();
            if (currentTool.system)
            {
                currentTool.system.reset();
                currentTool.action.reset();
            }
            auto& systems = p.toolSystems;
            auto& actions = p.toolActions;
            if (value >= 0 && value < systems.size() && value < actions.size())
            {
                currentTool.system = systems[value];
                currentTool.action = actions[value];
            }
            p.currentTool->setIfChanged(currentTool);
            p.actionGroup->setChecked(value);
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
                p.menu->setText(_getText(DJV_TEXT("menu_tools")));
            }
        }

    } // namespace ViewApp
} // namespace djv

