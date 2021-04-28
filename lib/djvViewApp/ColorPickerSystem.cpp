// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorPickerSystem.h>

#include <djvViewApp/ColorPickerSettings.h>
#include <djvViewApp/ColorPickerWidget.h>
#include <djvViewApp/ToolTitleBar.h>

#include <djvUI/Action.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ShortcutData.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorPickerSystem::Private
        {
            glm::vec2 pickerPos = glm::vec2(0.F, 0.F);
            std::shared_ptr<ColorPickerSettings> settings;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<ColorPickerWidget> widget;
        };

        void ColorPickerSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::ColorPickerSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = ColorPickerSettings::create(context);

            p.actions["ColorPicker"] = UI::Action::create();
            p.actions["ColorPicker"]->setIcon("djvIconColorPicker");

            _addShortcut(DJV_TEXT("shortcut_color_picker"), GLFW_KEY_K, UI::getSystemModifier());

            _textUpdate();
            _shortcutsUpdate();

            _logInitTime();
        }

        ColorPickerSystem::ColorPickerSystem() :
            _p(new Private)
        {}

        ColorPickerSystem::~ColorPickerSystem()
        {}

        std::shared_ptr<ColorPickerSystem> ColorPickerSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<ColorPickerSystem>();
            if (!out)
            {
                out = std::shared_ptr<ColorPickerSystem>(new ColorPickerSystem);
                out->_init(context);
            }
            return out;
        }

        int ColorPickerSystem::getSortKey() const
        {
            return 8;
        }

        std::map<std::string, std::shared_ptr<UI::Action>> ColorPickerSystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<std::shared_ptr<UI::Action> > ColorPickerSystem::getToolWidgetActions() const
        {
            DJV_PRIVATE_PTR();
            return
            {
                p.actions["ColorPicker"]
            };
        }

        std::vector<std::shared_ptr<UI::Action> > ColorPickerSystem::getToolWidgetToolBarActions() const
        {
            DJV_PRIVATE_PTR();
            return
            {
                p.actions["ColorPicker"]
            };
        }

        ToolWidgetData ColorPickerSystem::createToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            ToolWidgetData out;
            if (auto context = getContext().lock())
            {
                if (value == p.actions["ColorPicker"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("widget_color_picker"), context);

                    auto widget = ColorPickerWidget::create(context);
                    widget->setPickerPos(p.pickerPos);
                    p.widget = widget;

                    out.titleBar = titleBar;
                    out.widget = widget;
                }
            }
            return out;
        }

        void ColorPickerSystem::deleteToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.actions["ColorPicker"])
            {
                if (p.widget)
                {
                    p.pickerPos = p.widget->getPickerPos();
                    p.widget.reset();
                }
            }
        }

        void ColorPickerSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["ColorPicker"]->setText(_getText(DJV_TEXT("menu_tools_color_picker")));
                p.actions["ColorPicker"]->setTooltip(_getText(DJV_TEXT("menu_color_picker_tooltip")));
            }
        }

        void ColorPickerSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["ColorPicker"]->setShortcuts(_getShortcuts("shortcut_color_picker"));
            }
        }
        
    } // namespace ViewApp
} // namespace djv

