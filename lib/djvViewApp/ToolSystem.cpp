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

#include <djvViewApp/ColorPickerWidget.h>
#include <djvViewApp/DebugWidget.h>
#include <djvViewApp/ErrorsWidget.h>
#include <djvViewApp/IToolSystem.h>
#include <djvViewApp/InfoWidget.h>
#include <djvViewApp/MagnifyWidget.h>
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
#include <djvCore/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ToolSystem::Private
        {
            std::shared_ptr<ToolSettings> settings;
            int currentToolSystem = -1;
            std::string errorsText;
            bool errorsPopup = false;
            std::map<std::string, bool> debugBellowsState;
            std::vector<std::shared_ptr<IToolSystem> > toolSystems;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> toolActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::weak_ptr<ColorPickerWidget> colorPickerWidget;
            std::weak_ptr<MagnifyWidget> magnifyWidget;
            std::weak_ptr<ErrorsWidget> errorsWidget;
            std::weak_ptr<DebugWidget> debugWidget;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ListObserver<std::string> > warningsObserver;
            std::shared_ptr<ListObserver<std::string> > errorsObserver;
            std::shared_ptr<ValueObserver<bool> > errorsPopupObserver;
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
            p.actions["Magnify"] = UI::Action::create();
            p.actions["Magnify"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Magnify"]->setIcon("djvIconMagnify");
            p.actions["Magnify"]->setShortcut(GLFW_KEY_Y, UI::Shortcut::getSystemModifier());
            p.actions["ColorPicker"] = UI::Action::create();
            p.actions["ColorPicker"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorPicker"]->setIcon("djvIconColorPicker");
            p.actions["ColorPicker"]->setShortcut(GLFW_KEY_K, UI::Shortcut::getSystemModifier());
            p.actions["Info"] = UI::Action::create();
            p.actions["Info"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Info"]->setShortcut(GLFW_KEY_I, UI::Shortcut::getSystemModifier());
            p.actions["Errors"] = UI::Action::create();
            p.actions["Errors"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["SystemLog"] = UI::Action::create();
            p.actions["SystemLog"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Debug"] = UI::Action::create();
            p.actions["Debug"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Debug"]->setShortcut(GLFW_KEY_D, UI::Shortcut::getSystemModifier());
            p.actions["Settings"] = UI::Action::create();
            p.actions["Settings"]->setIcon("djvIconSettings");

            p.menu = UI::Menu::create(context);
            for (const auto& i : toolActions)
            {
                p.menu->addAction(i.second);
            }
            p.menu->addSeparator();
            p.menu->addAction(p.actions["ColorPicker"]);
            p.menu->addAction(p.actions["Magnify"]);
            p.menu->addAction(p.actions["Info"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Errors"]);
            p.menu->addAction(p.actions["SystemLog"]);
            p.menu->addAction(p.actions["Debug"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Settings"]);

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
            p.actionObservers["ColorPicker"] = ValueObserver<bool>::create(
                p.actions["ColorPicker"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                auto widget = ColorPickerWidget::create(context);
                                widget->setSampleSize(system->_p->settings->getColorPickerSampleSize());
                                widget->setColorTypeLock(system->_p->settings->getColorPickerTypeLock());
                                widget->setPickerPos(system->_p->settings->getColorPickerPos());
                                system->_p->colorPickerWidget = widget;
                                system->_openWidget("ColorPicker", widget);
                            }
                            else
                            {
                                system->_closeWidget("ColorPicker");
                            }
                        }
                    }
                });

            p.actionObservers["Magnify"] = ValueObserver<bool>::create(
                p.actions["Magnify"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                auto widget = MagnifyWidget::create(context);
                                widget->setMagnify(system->_p->settings->getMagnify());
                                widget->setMagnifyPos(system->_p->settings->getMagnifyPos());
                                system->_p->magnifyWidget = widget;
                                system->_openWidget("Magnify", widget);
                            }
                            else
                            {
                                system->_closeWidget("Magnify");
                            }
                        }
                    }
                });

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

            p.actionObservers["Errors"] = ValueObserver<bool>::create(
                p.actions["Errors"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                system->_errorsPopup();
                            }
                            else
                            {
                                system->_closeWidget("Errors");
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

            p.actionObservers["Settings"] = ValueObserver<bool>::create(
                p.actions["Settings"]->observeClicked(),
                [weak, contextWeak](bool value)
                {
                    if (value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<SettingsSystem>();
                            settingsSystem->showSettingsDialog();
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
                        std::stringstream ss(system->_p->errorsText);
                        for (const auto& i : value)
                        {
                            ss << "\n";
                            ss << system->_getText(DJV_TEXT("Warning")) << ": ";
                            ss << i;
                        }
                        if (auto errorsWidget = system->_p->errorsWidget.lock())
                        {
                            errorsWidget->setText(system->_p->errorsText);
                        }
                        if (system->_p->errorsPopup)
                        {
                            system->_errorsPopup();
                        }
                    }
                });

            p.errorsObserver = ListObserver<std::string>::create(
                logSystem->observeErrors(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto system = weak.lock())
                    {
                        std::stringstream ss;
                        for (const auto& i : value)
                        {
                            ss << system->_getText(DJV_TEXT("ERROR")) << ": ";
                            ss << i;
                            ss << "\n";
                        }
                        system->_p->errorsText.append(ss.str());
                        if (auto errorsWidget = system->_p->errorsWidget.lock())
                        {
                            errorsWidget->setText(system->_p->errorsText);
                        }
                        if (system->_p->errorsPopup)
                        {
                            system->_errorsPopup();
                        }
                    }
                });

            p.errorsPopupObserver = ValueObserver<bool>::create(
                p.settings->observeErrorsPopup(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->errorsPopup = value;
                        if (auto errorsWidget = system->_p->errorsWidget.lock())
                        {
                            errorsWidget->setPopup(value);
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
            _closeWidget("ColorPicker");
            _closeWidget("Magnify");
            _closeWidget("Info");
            _closeWidget("Errors");
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
            if ("ColorPicker" == value)
            {
                if (auto widget = p.colorPickerWidget.lock())
                {
                    p.settings->setColorPickerSampleSize(widget->getSampleSize());
                    p.settings->setColorPickerTypeLock(widget->getColorTypeLock());
                    p.settings->setColorPickerPos(widget->getPickerPos());
                }
                p.colorPickerWidget.reset();
            }
            else if ("Magnify" == value)
            {
                if (auto widget = p.magnifyWidget.lock())
                {
                    p.settings->setMagnify(widget->getMagnify());
                    p.settings->setMagnifyPos(widget->getMagnifyPos());
                }
                p.magnifyWidget.reset();
            }
            else if ("Errors" == value)
            {
                p.errorsWidget.reset();
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
                p.actions["ColorPicker"]->setText(_getText(DJV_TEXT("Color Picker")));
                p.actions["ColorPicker"]->setTooltip(_getText(DJV_TEXT("Color picker tooltip")));
                p.actions["Magnify"]->setText(_getText(DJV_TEXT("Magnify")));
                p.actions["Magnify"]->setTooltip(_getText(DJV_TEXT("Magnify tooltip")));
                p.actions["Info"]->setText(_getText(DJV_TEXT("Information")));
                p.actions["Info"]->setTooltip(_getText(DJV_TEXT("Information widget tooltip")));
                p.actions["Errors"]->setText(_getText(DJV_TEXT("Errors")));
                p.actions["Errors"]->setTooltip(_getText(DJV_TEXT("Errors widget tooltip")));
                p.actions["SystemLog"]->setText(_getText(DJV_TEXT("System Log")));
                p.actions["SystemLog"]->setTooltip(_getText(DJV_TEXT("System log widget tooltip")));
                p.actions["Debug"]->setText(_getText(DJV_TEXT("Debugging")));
                p.actions["Debug"]->setTooltip(_getText(DJV_TEXT("Debugging widget tooltip")));
                p.actions["Settings"]->setText(_getText(DJV_TEXT("Settings")));
                p.actions["Settings"]->setTooltip(_getText(DJV_TEXT("Settings tooltip")));

                p.menu->setText(_getText(DJV_TEXT("Tools")));
            }
        }

        void ToolSystem::_errorsPopup()
        {
            DJV_PRIVATE_PTR();
            auto contextWeak = getContext();
            if (auto context = contextWeak.lock())
            {
                if (!p.errorsWidget.lock())
                {
                    auto widget = ErrorsWidget::create(context);
                    widget->setText(p.errorsText);
                    widget->setPopup(p.errorsPopup);
                    auto weak = std::weak_ptr<ToolSystem>(std::dynamic_pointer_cast<ToolSystem>(shared_from_this()));
                    widget->setPopupCallback(
                        [weak](bool value)
                        {
                            if (auto system = weak.lock())
                            {
                                system->_p->settings->setErrorsPopup(value);
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
                                    eventSystem->setClipboard(system->_p->errorsText.c_str());
                                }
                            }
                        });
                    widget->setClearCallback(
                        [weak]
                        {
                            if (auto system = weak.lock())
                            {
                                system->_p->errorsText.clear();
                                if (auto errorsWidget = system->_p->errorsWidget.lock())
                                {
                                    errorsWidget->setText(std::string());
                                }
                            }
                        });
                    p.errorsWidget = widget;
                    _openWidget("Errors", widget);
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

