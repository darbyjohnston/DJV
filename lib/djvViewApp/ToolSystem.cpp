// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ToolSystem.h>

#include <djvViewApp/DebugWidget.h>
#include <djvViewApp/IToolSystem.h>
#include <djvViewApp/InfoWidget.h>
#include <djvViewApp/MessagesWidget.h>
#include <djvViewApp/SettingsSystem.h>
#include <djvViewApp/SystemLogWidget.h>
#include <djvViewApp/ToolSettings.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>

#include <djvCore/Context.h>
#include <djvCore/IEventSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/String.h>
#include <djvCore/TextSystem.h>

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
        
        struct ToolSystem::Private
        {
            std::shared_ptr<ToolSettings> settings;
            int currentToolSystem = -1;
            std::list<std::string> messages;
            bool messagesPopup = false;
            std::map<std::string, bool> debugBellowsState;
            std::vector<std::shared_ptr<IToolSystem> > toolSystems;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> toolActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::weak_ptr<MessagesWidget> messagesWidget;
            std::weak_ptr<DebugWidget> debugWidget;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ListObserver<std::string> > warningsObserver;
            std::shared_ptr<ListObserver<std::string> > errorsObserver;
            std::shared_ptr<ValueObserver<bool> > messagesPopupObserver;
        };

        void ToolSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::ToolSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = ToolSettings::create(context);
            p.debugBellowsState = p.settings->getDebugBellowsState();
            _setWidgetGeom(p.settings->getWidgetGeom());

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
            p.actions["Info"] = UI::Action::create();
            p.actions["Info"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Info"]->setShortcut(GLFW_KEY_I, UI::Shortcut::getSystemModifier());
            p.actions["Messages"] = UI::Action::create();
            p.actions["Messages"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["SystemLog"] = UI::Action::create();
            p.actions["SystemLog"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Debug"] = UI::Action::create();
            p.actions["Debug"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Debug"]->setShortcut(GLFW_KEY_D, UI::Shortcut::getSystemModifier());
            p.actions["Settings"] = UI::Action::create();
            p.actions["Settings"]->setIcon("djvIconSettings");
            p.actions["Settings"]->setButtonType(UI::ButtonType::Toggle);

            p.menu = UI::Menu::create(context);
            for (const auto& i : toolActions)
            {
                p.menu->addAction(i.second);
            }
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Info"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Messages"]);
            p.menu->addAction(p.actions["SystemLog"]);
            p.menu->addAction(p.actions["Debug"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Settings"]);

            _textUpdate();

            auto weak = std::weak_ptr<ToolSystem>(std::dynamic_pointer_cast<ToolSystem>(shared_from_this()));
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

            auto contextWeak = std::weak_ptr<Context>(context);
            p.actionObservers["Info"] = ValueObserver<bool>::create(
                p.actions["Info"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                system->_openWidget("Info", InfoWidget::create(context));
                            }
                            else
                            {
                                system->_closeWidget("Info");
                            }
                        }
                    }
                });

            p.actionObservers["Messages"] = ValueObserver<bool>::create(
                p.actions["Messages"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                system->_messagesPopup();
                            }
                            else
                            {
                                system->_closeWidget("Messages");
                            }
                        }
                    }
                });

            p.actionObservers["SystemLog"] = ValueObserver<bool>::create(
                p.actions["SystemLog"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                auto widget = SystemLogWidget::create(context);
                                widget->reloadLog();
                                system->_openWidget("SystemLog", widget);
                            }
                            else
                            {
                                system->_closeWidget("SystemLog");
                            }
                        }
                    }
                });

            p.actionObservers["Debug"] = ValueObserver<bool>::create(
                p.actions["Debug"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                auto widget = DebugWidget::create(context);
                                widget->setBellowsState(system->_p->debugBellowsState);
                                system->_p->debugWidget = widget;
                                system->_openWidget("Debug", widget);
                            }
                            else
                            {
                                system->_closeWidget("Debug");
                            }
                        }
                    }
                });

            auto logSystem = context->getSystemT<LogSystem>();
            p.warningsObserver = ListObserver<std::string>::create(
                logSystem->observeWarnings(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto system = weak.lock())
                    {
                        for (const auto& i : value)
                        {
                            std::stringstream ss;
                            ss << system->_getText(DJV_TEXT("recent_files_label_warning")) << ": ";
                            ss << i;
                            system->_p->messages.push_back(ss.str());
                            while (system->_p->messages.size() > messagesMax)
                            {
                                system->_p->messages.pop_front();
                            }
                        }
                        if (auto messagesWidget = system->_p->messagesWidget.lock())
                        {
                            messagesWidget->setText(system->_getMessagesString());
                        }
                        if (system->_p->messagesPopup)
                        {
                            system->_messagesPopup();
                        }
                    }
                });

            p.errorsObserver = ListObserver<std::string>::create(
                logSystem->observeErrors(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto system = weak.lock())
                    {
                        for (const auto& i : value)
                        {
                            std::stringstream ss;
                            ss << system->_getText(DJV_TEXT("recent_files_label_error")) << ": ";
                            ss << i;
                            system->_p->messages.push_back(ss.str());
                            while (system->_p->messages.size() > messagesMax)
                            {
                                system->_p->messages.pop_front();
                            }
                        }
                        if (auto messagesWidget = system->_p->messagesWidget.lock())
                        {
                            messagesWidget->setText(system->_getMessagesString());
                        }
                        if (system->_p->messagesPopup)
                        {
                            system->_messagesPopup();
                        }
                    }
                });

            p.messagesPopupObserver = ValueObserver<bool>::create(
                p.settings->observeMessagesPopup(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->messagesPopup = value;
                        if (auto messagesWidget = system->_p->messagesWidget.lock())
                        {
                            messagesWidget->setPopup(value);
                        }
                    }
                });
        }

        ToolSystem::ToolSystem() :
            _p(new Private)
        {}

        ToolSystem::~ToolSystem()
        {
            DJV_PRIVATE_PTR();
            _closeWidget("Info");
            _closeWidget("Messages");
            _closeWidget("SystemLog");
            _closeWidget("Debug");
            p.settings->setDebugBellowsState(p.debugBellowsState);
            p.settings->setWidgetGeom(_getWidgetGeom());
        }

        std::shared_ptr<ToolSystem> ToolSystem::create(const std::shared_ptr<Core::Context>& context)
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

        void ToolSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if ("Messages" == value)
            {
                p.messagesWidget.reset();
            }
            else if ("Debug" == value)
            {
                if (auto debugWidget = p.debugWidget.lock())
                {
                    p.debugBellowsState = debugWidget->getBellowsState();
                }
                p.debugWidget.reset();
            }
            const auto i = p.actions.find(value);
            if (i != p.actions.end())
            {
                i->second->setChecked(false);
            }
            IViewSystem::_closeWidget(value);
        }

        void ToolSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Info"]->setText(_getText(DJV_TEXT("menu_tools_information")));
                p.actions["Info"]->setTooltip(_getText(DJV_TEXT("menu_tools_information_widget_tooltip")));
                p.actions["Messages"]->setText(_getText(DJV_TEXT("menu_tools_messages")));
                p.actions["Messages"]->setTooltip(_getText(DJV_TEXT("menu_tools_messages_widget_tooltip")));
                p.actions["SystemLog"]->setText(_getText(DJV_TEXT("menu_tools_system_log")));
                p.actions["SystemLog"]->setTooltip(_getText(DJV_TEXT("menu_tools_system_log_widget_tooltip")));
                p.actions["Debug"]->setText(_getText(DJV_TEXT("menu_tools_debugging")));
                p.actions["Debug"]->setTooltip(_getText(DJV_TEXT("menu_tools_debugging_widget_tooltip")));
                p.actions["Settings"]->setText(_getText(DJV_TEXT("menu_tools_settings")));
                p.actions["Settings"]->setTooltip(_getText(DJV_TEXT("menu_tools_settings_tooltip")));

                p.menu->setText(_getText(DJV_TEXT("menu_tools")));
            }
        }

        std::string ToolSystem::_getMessagesString() const
        {
            return String::joinList(_p->messages, '\n');
        }

        void ToolSystem::_messagesPopup()
        {
            DJV_PRIVATE_PTR();
            auto contextWeak = getContext();
            if (auto context = contextWeak.lock())
            {
                if (!p.messagesWidget.lock())
                {
                    auto widget = MessagesWidget::create(context);
                    widget->setPopup(p.messagesPopup);
                    widget->setText(_getMessagesString());
                    auto weak = std::weak_ptr<ToolSystem>(std::dynamic_pointer_cast<ToolSystem>(shared_from_this()));
                    widget->setPopupCallback(
                        [weak](bool value)
                        {
                            if (auto system = weak.lock())
                            {
                                system->_p->settings->setMessagesPopup(value);
                            }
                        });
                    widget->setCopyCallback(
                        [weak, contextWeak]
                        {
                            if (auto context = contextWeak.lock())
                            {
                                if (auto system = weak.lock())
                                {
                                    auto eventSystem = context->getSystemT<Event::IEventSystem>();
                                    eventSystem->setClipboard(system->_getMessagesString().c_str());
                                }
                            }
                        });
                    widget->setClearCallback(
                        [weak]
                        {
                            if (auto system = weak.lock())
                            {
                                system->_p->messages.clear();
                                if (auto messagesWidget = system->_p->messagesWidget.lock())
                                {
                                    messagesWidget->setText(std::string());
                                }
                            }
                        });
                    p.messagesWidget = widget;
                    p.actions["Messages"]->setChecked(true);
                    _openWidget("Messages", widget);
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

