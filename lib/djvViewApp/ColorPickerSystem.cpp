// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorPickerSystem.h>

#include <djvViewApp/ColorPickerSettings.h>
#include <djvViewApp/ColorPickerWidget.h>

#include <djvUI/Action.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ShortcutData.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorPickerSystem::Private
        {
            std::shared_ptr<ColorPickerSettings> settings;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::weak_ptr<ColorPickerWidget> widget;
        };

        void ColorPickerSystem::_init(const std::shared_ptr<Context>& context)
        {
            IToolSystem::_init("djv::ViewApp::ColorPickerSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = ColorPickerSettings::create(context);
            _setWidgetGeom(p.settings->getWidgetGeom());

            p.actions["ColorPicker"] = UI::Action::create();
            p.actions["ColorPicker"]->setIcon("djvIconColorPicker");

            _addShortcut("shortcut_color_picker", GLFW_KEY_K, UI::ShortcutData::getSystemModifier());

            _textUpdate();
            _shortcutsUpdate();
        }

        ColorPickerSystem::ColorPickerSystem() :
            _p(new Private)
        {}

        ColorPickerSystem::~ColorPickerSystem()
        {
            DJV_PRIVATE_PTR();
            _closeWidget("ColorPicker");
            p.settings->setWidgetGeom(_getWidgetGeom());
        }

        std::shared_ptr<ColorPickerSystem> ColorPickerSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorPickerSystem>(new ColorPickerSystem);
            out->_init(context);
            return out;
        }

        ToolActionData ColorPickerSystem::getToolAction() const
        {
            return
            {
                _p->actions["ColorPicker"],
                "B"
            };
        }

        void ColorPickerSystem::setCurrentTool(bool value, int index)
        {
            DJV_PRIVATE_PTR();
            if (value)
            {
                if (p.widget.expired())
                {
                    if (auto context = getContext().lock())
                    {
                        auto widget = ColorPickerWidget::create(context);
                        widget->setSampleSize(p.settings->getSampleSize());
                        widget->setLockType(p.settings->getLockType());
                        widget->setApplyColorOperations(p.settings->getApplyColorOperations());
                        widget->setApplyColorSpace(p.settings->getApplyColorSpace());
                        widget->setPickerPos(p.settings->getPickerPos());
                        auto weak = std::weak_ptr<ColorPickerSystem>(std::dynamic_pointer_cast<ColorPickerSystem>(shared_from_this()));
                        p.widget = widget;
                        _openWidget("ColorPicker", widget);
                    }
                }
            }
            else if (-1 == index)
            {
                _closeWidget("ColorPicker");
            }
            if (auto widget = p.widget.lock())
            {
                widget->setCurrentTool(value);
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ColorPickerSystem::getActions() const
        {
            return _p->actions;
        }

        void ColorPickerSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            const auto i = p.actions.find(value);
            if (i != p.actions.end())
            {
                i->second->setChecked(false);
            }
            if (auto widget = p.widget.lock())
            {
                p.settings->setSampleSize(widget->getSampleSize());
                p.settings->setLockType(widget->getLockType());
                p.settings->setApplyColorOperations(widget->getApplyColorOperations());
                p.settings->setApplyColorSpace(widget->getApplyColorSpace());
                p.settings->setPickerPos(widget->getPickerPos());
                p.widget.reset();
            }
            IToolSystem::_closeWidget(value);
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

