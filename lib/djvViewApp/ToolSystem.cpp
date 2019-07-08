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

#include <djvViewApp/ToolSystem.h>

#include <djvViewApp/IToolSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ToolSystem::Private
        {
            int currentToolSystem = -1;
            std::vector<std::shared_ptr<IToolSystem> > toolSystems;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> toolActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void ToolSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewApp::ToolSystem", context);

            DJV_PRIVATE_PTR();

            std::map<std::string, std::shared_ptr<IToolSystem> > toolSystems;
            std::map<std::string, std::shared_ptr<UI::Action> > toolActions;
            auto systems = context->getSystemsT<IToolSystem>();
            for (const auto& i : systems)
            {
                auto data = i->getToolAction();
                toolSystems[data.sortKey] = i;
                toolActions[data.sortKey] = data.action;
            }
            for (const auto& i : toolSystems)
            {
                p.toolSystems.push_back(i.second);
            }

            p.toolActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            for (const auto& i : toolActions)
            {
                p.toolActionGroup->addAction(i.second);
            }

            p.menu = UI::Menu::create(context);
            for (const auto& i : toolActions)
            {
                p.menu->addAction(i.second);
            }

            auto weak = std::weak_ptr<ToolSystem>(std::dynamic_pointer_cast<ToolSystem>(shared_from_this()));
            _setCloseWidgetCallback(
                [weak](const std::string& name)
            {
                if (auto system = weak.lock())
                {
                    const auto i = system->_p->actions.find(name);
                    if (i != system->_p->actions.end())
                    {
                        i->second->setChecked(false);
                    }
                }
            });

            p.toolActionGroup->setExclusiveCallback(
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        if (system->_p->currentToolSystem >= 0 && system->_p->currentToolSystem < system->_p->toolSystems.size())
                        {
                            system->_p->toolSystems[system->_p->currentToolSystem]->setCurrentTool(false);
                        }
                        system->_p->currentToolSystem = value;
                        if (system->_p->currentToolSystem >= 0 && system->_p->currentToolSystem < system->_p->toolSystems.size())
                        {
                            system->_p->toolSystems[system->_p->currentToolSystem]->setCurrentTool(true);
                        }
                    }
                });

            p.localeObserver = ValueObserver<std::string>::create(
                context->getSystemT<TextSystem>()->observeCurrentLocale(),
                [weak](const std::string & value)
            {
                if (auto system = weak.lock())
                {
                    system->_textUpdate();
                }
            });
        }

        ToolSystem::ToolSystem() :
            _p(new Private)
        {}

        ToolSystem::~ToolSystem()
        {}

        std::shared_ptr<ToolSystem> ToolSystem::create(Context * context)
        {
            auto out = std::shared_ptr<ToolSystem>(new ToolSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ToolSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData ToolSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "H"
            };
        }

        void ToolSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.menu->setText(_getText(DJV_TEXT("Tools")));
        }

    } // namespace ViewApp
} // namespace djv

