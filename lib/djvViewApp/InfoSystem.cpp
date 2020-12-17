// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/InfoSystem.h>

#include <djvViewApp/InfoWidget.h>
#include <djvViewApp/ToolTitleBar.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/ShortcutDataFunc.h>
#include <djvUI/ToolBar.h>

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

        struct InfoSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
        };

        void InfoSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::InfoSystem", context);
            DJV_PRIVATE_PTR();

            p.actions["Info"] = UI::Action::create();
            p.actions["Info"]->setIcon("djvIconInfo");

            _addShortcut("shortcut_tool_info", GLFW_KEY_I, UI::getSystemModifier());

            _textUpdate();
            _shortcutsUpdate();
        }

        InfoSystem::InfoSystem() :
            _p(new Private)
        {}

        InfoSystem::~InfoSystem()
        {}

        std::shared_ptr<InfoSystem> InfoSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<InfoSystem>();
            if (!out)
            {
                out = std::shared_ptr<InfoSystem>(new InfoSystem);
                out->_init(context);
            }
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > InfoSystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<ActionData> InfoSystem::getToolActionData() const
        {
            return
            {
                { _p->actions["Info"], "Z0" }
            };
        }

        ToolWidgetData InfoSystem::createToolWidget(const std::shared_ptr<UI::Action>& value)
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
                    toolBar->setStretch(searchBox);

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
            }
            return out;
        }

        void InfoSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Info"]->setText(_getText(DJV_TEXT("menu_tools_information")));
                p.actions["Info"]->setTooltip(_getText(DJV_TEXT("menu_tools_information_widget_tooltip")));
            }
        }

        void InfoSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Info"]->setShortcuts(_getShortcuts("shortcut_tool_info"));
            }
        }

    } // namespace ViewApp
} // namespace djv

