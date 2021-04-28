// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/DebugSystem.h>

#include <djvViewApp/DebugSettings.h>
#include <djvViewApp/DebugWidget.h>
#include <djvViewApp/ToolTitleBar.h>

#include <djvUI/Action.h>
#include <djvUI/ShortcutData.h>

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

        struct DebugSystem::Private
        {
            std::shared_ptr<DebugSettings> settings;
            std::map<std::string, bool> bellowsState;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::weak_ptr<DebugWidget> widget;
        };

        void DebugSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::DebugSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = DebugSettings::create(context);
            p.bellowsState = p.settings->getBellowsState();

            p.actions["Debug"] = UI::Action::create();
            p.actions["Debug"]->setIcon("djvIconDebug");

            _addShortcut(DJV_TEXT("shortcut_tool_debug"), GLFW_KEY_D, UI::getSystemModifier());

            _textUpdate();
            _shortcutsUpdate();

            _logInitTime();
        }

        DebugSystem::DebugSystem() :
            _p(new Private)
        {}

        DebugSystem::~DebugSystem()
        {
            DJV_PRIVATE_PTR();
            p.settings->setBellowsState(p.bellowsState);
        }

        std::shared_ptr<DebugSystem> DebugSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<DebugSystem>();
            if (!out)
            {
                out = std::shared_ptr<DebugSystem>(new DebugSystem);
                out->_init(context);
            }
            return out;
        }

        int DebugSystem::getSortKey() const
        {
            return 14;
        }

        std::map<std::string, std::shared_ptr<UI::Action>> DebugSystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<std::shared_ptr<UI::Action> > DebugSystem::getToolWidgetActions() const
        {
            DJV_PRIVATE_PTR();
            return
            {
                p.actions["Debug"]
            };
        }

        ToolWidgetData DebugSystem::createToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            ToolWidgetData out;
            auto contextWeak = getContext();
            if (auto context = contextWeak.lock())
            {
                if (value == p.actions["Debug"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("debug_title"), context);

                    auto widget = DebugWidget::create(context);
                    widget->setBellowsState(p.bellowsState);
                    p.widget = widget;

                    out.titleBar = titleBar;
                    out.widget = widget;
                }
            }
            return out;
        }

        void DebugSystem::deleteToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.actions["Debug"])
            {
                if (auto widget = p.widget.lock())
                {
                    p.bellowsState = widget->getBellowsState();
                }
            }
        }

        void DebugSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Debug"]->setText(_getText(DJV_TEXT("menu_tools_debugging")));
                p.actions["Debug"]->setTooltip(_getText(DJV_TEXT("menu_tools_debugging_widget_tooltip")));
            }
        }

        void DebugSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Debug"]->setShortcuts(_getShortcuts("shortcut_tool_debug"));
            }
        }

    } // namespace ViewApp
} // namespace djv

