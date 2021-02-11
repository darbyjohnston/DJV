// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MagnifySystem.h>

#include <djvViewApp/MagnifyWidget.h>
#include <djvViewApp/MagnifySettings.h>
#include <djvViewApp/ToolTitleBar.h>

#include <djvUI/Action.h>
#include <djvUI/ShortcutDataFunc.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MagnifySystem::Private
        {
            glm::vec2 magnifyPos = glm::vec2(0.F, 0.F);
            std::shared_ptr<MagnifySettings> settings;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<MagnifyWidget> widget;
        };

        void MagnifySystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::MagnifySystem", context);
            DJV_PRIVATE_PTR();

            p.settings = MagnifySettings::create(context);

            p.actions["Magnify"] = UI::Action::create();
            p.actions["Magnify"]->setIcon("djvIconMagnify");

            _addShortcut(DJV_TEXT("shortcut_magnify"), GLFW_KEY_Y, UI::getSystemModifier());

            _textUpdate();
            _shortcutsUpdate();

            _logInitTime();
        }

        MagnifySystem::MagnifySystem() :
            _p(new Private)
        {}

        MagnifySystem::~MagnifySystem()
        {}

        std::shared_ptr<MagnifySystem> MagnifySystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<MagnifySystem>();
            if (!out)
            {
                out = std::shared_ptr<MagnifySystem>(new MagnifySystem);
                out->_init(context);
            }
            return out;
        }

        int MagnifySystem::getSortKey() const
        {
            return 9;
        }

        std::map<std::string, std::shared_ptr<UI::Action>> MagnifySystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<std::shared_ptr<UI::Action> > MagnifySystem::getToolWidgetActions() const
        {
            DJV_PRIVATE_PTR();
            return
            {
                p.actions["Magnify"]
            };
        }

        std::vector<std::shared_ptr<UI::Action> > MagnifySystem::getToolWidgetToolBarActions() const
        {
            DJV_PRIVATE_PTR();
            return
            {
                p.actions["Magnify"]
            };
        }

        ToolWidgetData MagnifySystem::createToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            ToolWidgetData out;
            if (auto context = getContext().lock())
            {
                if (value == p.actions["Magnify"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("widget_magnify_title"), context);

                    auto widget = MagnifyWidget::create(context);
                    widget->setMagnifyPos(p.magnifyPos);
                    p.widget = widget;

                    out.titleBar = titleBar;
                    out.widget = widget;
                }
            }
            return out;
        }

        void MagnifySystem::deleteToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.actions["Magnify"])
            {
                if (p.widget)
                {
                    p.magnifyPos = p.widget->getMagnifyPos();
                    p.widget.reset();
                }
            }
        }

        void MagnifySystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Magnify"]->setText(_getText(DJV_TEXT("menu_tools_magnify")));
                p.actions["Magnify"]->setTooltip(_getText(DJV_TEXT("menu_magnify_tooltip")));
            }
        }

        void MagnifySystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Magnify"]->setShortcuts(_getShortcuts("shortcut_magnify"));
            }
        }

    } // namespace ViewApp
} // namespace djv

