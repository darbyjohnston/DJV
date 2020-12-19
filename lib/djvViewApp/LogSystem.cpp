// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/LogSystem.h>

#include <djvViewApp/LogWidget.h>
#include <djvViewApp/ToolTitleBar.h>

#include <djvUI/Action.h>
#include <djvUI/ShortcutDataFunc.h>

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

        struct LogSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
        };

        void LogSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::LogSystem", context);
            DJV_PRIVATE_PTR();

            p.actions["SystemLog"] = UI::Action::create();
            p.actions["SystemLog"]->setIcon("djvIconFile");

            _addShortcut(DJV_TEXT("shortcut_tool_system_log"), 0);

            _textUpdate();
            _shortcutsUpdate();

            _logInitTime();
        }

        LogSystem::LogSystem() :
            _p(new Private)
        {}

        LogSystem::~LogSystem()
        {}

        std::shared_ptr<LogSystem> LogSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<LogSystem>();
            if (!out)
            {
                out = std::shared_ptr<LogSystem>(new LogSystem);
                out->_init(context);
            }
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > LogSystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<ActionData> LogSystem::getToolActionData() const
        {
            return
            {
                { _p->actions["SystemLog"], "Z2" }
            };
        }

        ToolWidgetData LogSystem::createToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            ToolWidgetData out;
            auto contextWeak = getContext();
            if (auto context = contextWeak.lock())
            {
                if (value == _p->actions["SystemLog"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("widget_log"), context);

                    auto toolBar = LogToolBar::create(context);

                    auto widget = LogWidget::create(context);
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
            }
            return out;
        }

        void LogSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["SystemLog"]->setText(_getText(DJV_TEXT("menu_tools_system_log")));
                p.actions["SystemLog"]->setTooltip(_getText(DJV_TEXT("menu_tools_system_log_widget_tooltip")));
            }
        }

        void LogSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["SystemLog"]->setShortcuts(_getShortcuts("shortcut_tool_system_log"));
            }
        }

    } // namespace ViewApp
} // namespace djv

