// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MessagesSystem.h>

#include <djvViewApp/MessagesSettings.h>
#include <djvViewApp/MessagesWidget.h>
#include <djvViewApp/ToolSettings.h>
#include <djvViewApp/ToolSystem.h>
#include <djvViewApp/ToolTitleBar.h>

#include <djvUI/Action.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ShortcutDataFunc.h>

#include <djvSystem/Context.h>
#include <djvSystem/IEventSystem.h>
#include <djvSystem/LogSystem.h>

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

        struct MessagesSystem::Private
        {
            std::shared_ptr<MessagesSettings> settings;
            std::list<std::string> messages;
            bool popup = false;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::weak_ptr<MessagesToolBar> toolBar;
            std::weak_ptr<MessagesWidget> widget;
            std::shared_ptr<Observer::List<std::string> > warningsObserver;
            std::shared_ptr<Observer::List<std::string> > errorsObserver;
            std::shared_ptr<Observer::Value<bool> > popupObserver;
        };

        void MessagesSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::MessagesSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = MessagesSettings::create(context);

            p.actions["Messages"] = UI::Action::create();
            p.actions["Messages"]->setIcon("djvIconMessages");

            _addShortcut(DJV_TEXT("shortcut_tool_messages"), GLFW_KEY_S, UI::getSystemModifier());

            _textUpdate();
            _shortcutsUpdate();

            auto weak = std::weak_ptr<MessagesSystem>(std::dynamic_pointer_cast<MessagesSystem>(shared_from_this()));
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
                        if (auto messagesWidget = system->_p->widget.lock())
                        {
                            messagesWidget->setText(system->_getString());
                        }
                        if (system->_p->popup)
                        {
                            system->_popup();
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
                        if (auto messagesWidget = system->_p->widget.lock())
                        {
                            messagesWidget->setText(system->_getString());
                        }
                        if (system->_p->popup)
                        {
                            system->_popup();
                        }
                    }
                });

            p.popupObserver = Observer::Value<bool>::create(
                p.settings->observePopup(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->popup = value;
                        if (auto messagesToolBar = system->_p->toolBar.lock())
                        {
                            messagesToolBar->setPopup(value);
                        }
                    }
                });

            _logInitTime();
        }

        MessagesSystem::MessagesSystem() :
            _p(new Private)
        {}

        MessagesSystem::~MessagesSystem()
        {}

        std::shared_ptr<MessagesSystem> MessagesSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<MessagesSystem>();
            if (!out)
            {
                out = std::shared_ptr<MessagesSystem>(new MessagesSystem);
                out->_init(context);
            }
            return out;
        }

        int MessagesSystem::getSortKey() const
        {
            return 12;
        }

        std::map<std::string, std::shared_ptr<UI::Action>> MessagesSystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<std::shared_ptr<UI::Action> > MessagesSystem::getToolWidgetActions() const
        {
            DJV_PRIVATE_PTR();
            return
            {
                p.actions["Messages"]
            };
        }

        std::vector<std::shared_ptr<UI::Action> > MessagesSystem::getToolWidgetToolBarActions() const
        {
            DJV_PRIVATE_PTR();
            return
            {
                p.actions["Messages"]
            };
        }

        ToolWidgetData MessagesSystem::createToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            ToolWidgetData out;
            auto contextWeak = getContext();
            if (auto context = contextWeak.lock())
            {
                if (value == p.actions["Messages"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("widget_messages"), context);

                    auto toolBar = MessagesToolBar::create(context);
                    toolBar->setPopup(p.popup);
                    p.toolBar = toolBar;

                    auto widget = MessagesWidget::create(context);
                    widget->setText(_getString());
                    p.widget = widget;

                    auto weak = std::weak_ptr<MessagesSystem>(std::dynamic_pointer_cast<MessagesSystem>(shared_from_this()));
                    toolBar->setPopupCallback(
                        [weak](bool value)
                        {
                            if (auto system = weak.lock())
                            {
                                system->_p->settings->setPopup(value);
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
                                    eventSystem->setClipboard(system->_getString().c_str());
                                }
                            }
                        });
                    toolBar->setClearCallback(
                        [weak]
                        {
                            if (auto system = weak.lock())
                            {
                                system->_p->messages.clear();
                                if (auto widget = system->_p->widget.lock())
                                {
                                    widget->setText(std::string());
                                }
                            }
                        });

                    out.titleBar = titleBar;
                    out.toolBar = toolBar;
                    out.widget = widget;
                }
            }
            return out;
        }

        void MessagesSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Messages"]->setText(_getText(DJV_TEXT("menu_tools_messages")));
                p.actions["Messages"]->setTooltip(_getText(DJV_TEXT("menu_tools_messages_widget_tooltip")));
            }
        }

        void MessagesSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Messages"]->setShortcuts(_getShortcuts("shortcut_tool_messages"));
            }
        }

        void MessagesSystem::_popup()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                auto toolSettings = settingsSystem->getSettingsT<ToolSettings>();
                auto toolSystem = context->getSystemT<ToolSystem>();
                toolSettings->setCurrentTool(toolSystem->getToolIndex(p.actions["Messages"]));
            }
        }

        std::string MessagesSystem::_getString() const
        {
            return String::joinList(_p->messages, '\n');
        }

    } // namespace ViewApp
} // namespace djv

