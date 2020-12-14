// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MiscToolsSystem.h>

#include <djvViewApp/DebugWidget.h>
#include <djvViewApp/InfoWidget.h>
#include <djvViewApp/MiscToolsSettings.h>
#include <djvViewApp/MessagesWidget.h>
#include <djvViewApp/SettingsSystem.h>
#include <djvViewApp/SystemLogWidget.h>
#include <djvViewApp/ToolTitleBar.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ShortcutDataFunc.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>

#include <djvSystem/Context.h>
#include <djvSystem/IEventSystem.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/StringFunc.h>

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

        struct MiscToolsSystem::Private
        {
            std::shared_ptr<MiscToolsSettings> settings;
            std::list<std::string> messages;
            bool messagesPopup = false;
            std::map<std::string, bool> debugBellowsState;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::weak_ptr<MessagesToolBar> messagesToolBar;
            std::weak_ptr<MessagesWidget> messagesWidget;
            std::weak_ptr<DebugWidget> debugWidget;
            std::shared_ptr<Observer::List<std::string> > warningsObserver;
            std::shared_ptr<Observer::List<std::string> > errorsObserver;
            std::shared_ptr<Observer::Value<bool> > messagesPopupObserver;
        };

        void MiscToolsSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::MiscToolsSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = MiscToolsSettings::create(context);
            p.debugBellowsState = p.settings->getDebugBellowsState();

            p.actions["Info"] = UI::Action::create();
            p.actions["Info"]->setIcon("djvIconInfo");
            p.actions["Messages"] = UI::Action::create();
            p.actions["Messages"]->setIcon("djvIconMessages");
            p.actions["SystemLog"] = UI::Action::create();
            p.actions["SystemLog"]->setIcon("djvIconFile");
            p.actions["Debug"] = UI::Action::create();
            p.actions["Debug"]->setIcon("djvIconDebug");

            _addShortcut("shortcut_tool_info", GLFW_KEY_I, UI::getSystemModifier());
            _addShortcut("shortcut_tool_messages", GLFW_KEY_S, UI::getSystemModifier());
            _addShortcut("shortcut_tool_system_log", 0);
            _addShortcut("shortcut_tool_debug", GLFW_KEY_D, UI::getSystemModifier());

            _textUpdate();
            _shortcutsUpdate();

            auto weak = std::weak_ptr<MiscToolsSystem>(std::dynamic_pointer_cast<MiscToolsSystem>(shared_from_this()));
            auto logSystem = context->getSystemT<System::LogSystem>();
            p.warningsObserver = Observer::List<std::string>::create(
                logSystem->observeWarnings(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto system = weak.lock())
                    {
                        for (const auto& i : value)
                        {
                            std::stringstream ss;
                            ss << system->_getText(DJV_TEXT("warning")) << ": ";
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

            p.errorsObserver = Observer::List<std::string>::create(
                logSystem->observeErrors(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto system = weak.lock())
                    {
                        for (const auto& i : value)
                        {
                            std::stringstream ss;
                            ss << system->_getText(DJV_TEXT("error")) << ": ";
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

            p.messagesPopupObserver = Observer::Value<bool>::create(
                p.settings->observeMessagesPopup(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->messagesPopup = value;
                        if (auto messagesToolBar = system->_p->messagesToolBar.lock())
                        {
                            messagesToolBar->setPopup(value);
                        }
                    }
                });
        }

        MiscToolsSystem::MiscToolsSystem() :
            _p(new Private)
        {}

        MiscToolsSystem::~MiscToolsSystem()
        {
            DJV_PRIVATE_PTR();
            p.settings->setDebugBellowsState(p.debugBellowsState);
        }

        std::shared_ptr<MiscToolsSystem> MiscToolsSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<MiscToolsSystem>();
            if (!out)
            {
                out = std::shared_ptr<MiscToolsSystem>(new MiscToolsSystem);
                out->_init(context);
            }
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > MiscToolsSystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<ActionData> MiscToolsSystem::getToolActionData() const
        {
            return
            {
                { _p->actions["Info"], "F0" },
                { _p->actions["Messages"], "F1" },
                { _p->actions["SystemLog"], "F2" },
                { _p->actions["Debug"], "F3" }
            };
        }

        ToolWidgetData MiscToolsSystem::createToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            ToolWidgetData out;
            auto contextWeak = getContext();
            if (auto context = contextWeak.lock())
            {
                if (value == _p->actions["Info"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("widget_info_title"), context);

                    auto searchBox = UIComponents::SearchBox::create(context);
                    auto toolBar = UI::ToolBar::create(context);
                    toolBar->addChild(searchBox);
                    toolBar->setStretch(searchBox, UI::RowStretch::Expand);

                    auto widget = InfoWidget::create(context);

                    searchBox->setFilterCallback(
                        [widget](const std::string& value)
                        {
                            widget->setFilter(value);
                        });

                    out.titleBar = titleBar;
                    out.toolBar = toolBar;
                    out.widget = widget;
                }
                else if (value == _p->actions["Messages"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("widget_messages"), context);

                    auto toolBar = MessagesToolBar::create(context);
                    toolBar->setPopup(p.messagesPopup);
                    p.messagesToolBar = toolBar;

                    auto widget = MessagesWidget::create(context);
                    widget->setText(_getMessagesString());
                    p.messagesWidget = widget;

                    auto weak = std::weak_ptr<MiscToolsSystem>(std::dynamic_pointer_cast<MiscToolsSystem>(shared_from_this()));
                    toolBar->setPopupCallback(
                        [weak](bool value)
                        {
                            if (auto system = weak.lock())
                            {
                                system->_p->settings->setMessagesPopup(value);
                            }
                        });
                    toolBar->setCopyCallback(
                        [weak, contextWeak]
                        {
                            if (auto context = contextWeak.lock())
                            {
                                if (auto system = weak.lock())
                                {
                                    auto eventSystem = context->getSystemT<System::Event::IEventSystem>();
                                    eventSystem->setClipboard(system->_getMessagesString().c_str());
                                }
                            }
                        });
                    toolBar->setClearCallback(
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

                    out.titleBar = titleBar;
                    out.toolBar = toolBar;
                    out.widget = widget;
                }
                else if (value == _p->actions["SystemLog"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("widget_log"), context);

                    auto toolBar = SystemLogToolBar::create(context);

                    auto widget = SystemLogWidget::create(context);
                    widget->reloadLog();

                    toolBar->setCopyCallback(
                        [widget]
                        {
                            widget->copyLog();
                        });
                    toolBar->setReloadCallback(
                        [widget]
                        {
                            widget->reloadLog();
                        });
                    toolBar->setClearCallback(
                        [widget]
                        {
                            widget->clearLog();
                        });
                    toolBar->setFilterCallback(
                        [widget](const std::string& value)
                        {
                            widget->setFilter(value);
                        });

                    out.titleBar = titleBar;
                    out.toolBar = toolBar;
                    out.widget = widget;
                }
                else if (value == _p->actions["Debug"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("debug_title"), context);

                    auto widget = DebugWidget::create(context);
                    widget->setBellowsState(p.debugBellowsState);
                    p.debugWidget = widget;

                    out.titleBar = titleBar;
                    out.widget = widget;
                }
            }
            return out;
        }

        void MiscToolsSystem::deleteToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == _p->actions["Debug"])
            {
                if (auto widget = p.debugWidget.lock())
                {
                    _p->debugBellowsState = widget->getBellowsState();
                }
            }
        }

        void MiscToolsSystem::_textUpdate()
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
            }
        }

        void MiscToolsSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Info"]->setShortcuts(_getShortcuts("shortcut_tool_info"));
                p.actions["Messages"]->setShortcuts(_getShortcuts("shortcut_tool_messages"));
                p.actions["SystemLog"]->setShortcuts(_getShortcuts("shortcut_tool_system_log"));
                p.actions["Debug"]->setShortcuts(_getShortcuts("shortcut_tool_debug"));
            }
        }

        void MiscToolsSystem::_messagesPopup()
        {
            DJV_PRIVATE_PTR();
        }

        std::string MiscToolsSystem::_getMessagesString() const
        {
            return String::joinList(_p->messages, '\n');
        }

    } // namespace ViewApp
} // namespace djv

