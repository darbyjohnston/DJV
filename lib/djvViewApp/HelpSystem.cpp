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

#include <djvViewApp/HelpSystem.h>

#include <djvViewApp/AboutDialog.h>
#include <djvViewApp/DebugWidget.h>
#include <djvViewApp/ErrorsWidget.h>
#include <djvViewApp/HelpSettings.h>
#include <djvViewApp/SystemLogWidget.h>

#include <djvDesktopApp/GLFWSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/Window.h>

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
        struct HelpSystem::Private
        {
            std::shared_ptr<HelpSettings> settings;
            
            std::string errorsText;
            bool errorsPopup = false;
            std::map<std::string, bool> debugBellowsState;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::weak_ptr<ErrorsWidget> errorsWidget;
            std::weak_ptr<DebugWidget> debugWidget;
            std::shared_ptr<AboutDialog> aboutDialog;
            std::shared_ptr<UI::Window> aboutWindow;

            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ListObserver<std::string> > warningsObserver;
            std::shared_ptr<ListObserver<std::string> > errorsObserver;
            std::shared_ptr<ValueObserver<bool> > errorsPopupObserver;
        };

        void HelpSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::HelpSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = HelpSettings::create(context);
            p.debugBellowsState = p.settings->getDebugBellowsState();
            _setWidgetGeom(p.settings->getWidgetGeom());

            //! \todo Implement me!
            p.actions["Documentation"] = UI::Action::create();
            p.actions["Documentation"]->setEnabled(false);
            p.actions["About"] = UI::Action::create();
            p.actions["Errors"] = UI::Action::create();
            p.actions["Errors"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["SystemLog"] = UI::Action::create();
            p.actions["SystemLog"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Debug"] = UI::Action::create();
            p.actions["Debug"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Debug"]->setShortcut(GLFW_KEY_D, UI::Shortcut::getSystemModifier());

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Documentation"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["About"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Errors"]);
            p.menu->addAction(p.actions["SystemLog"]);
            p.menu->addAction(p.actions["Debug"]);

            auto weak = std::weak_ptr<HelpSystem>(std::dynamic_pointer_cast<HelpSystem>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.actionObservers["About"] = ValueObserver<bool>::create(
                p.actions["About"]->observeClicked(),
                [weak, contextWeak](bool value)
            {
                if (value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (system->_p->aboutWindow)
                            {
                                system->_p->aboutWindow->close();
                                system->_p->aboutWindow.reset();
                            }
                            system->_p->aboutDialog = AboutDialog::create(context);
                            system->_p->aboutDialog->setCloseCallback(
                                [weak]
                                {
                                    if (auto system = weak.lock())
                                    {
                                        system->_p->aboutDialog.reset();
                                        system->_p->aboutWindow->close();
                                        system->_p->aboutWindow.reset();
                                    }
                                });
                            system->_p->aboutWindow = UI::Window::create(context);
                            system->_p->aboutWindow->setBackgroundRole(UI::ColorRole::None);
                            system->_p->aboutWindow->addChild(system->_p->aboutDialog);
                            system->_p->aboutWindow->show();
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

        HelpSystem::HelpSystem() :
            _p(new Private)
        {}

        HelpSystem::~HelpSystem()
        {
            DJV_PRIVATE_PTR();
            _closeWidget("Errors");
            _closeWidget("SystemLog");
            _closeWidget("Debug");
            p.settings->setDebugBellowsState(p.debugBellowsState);
            p.settings->setWidgetGeom(_getWidgetGeom());
            if (p.aboutWindow)
            {
                p.aboutWindow->close();
            }
        }

        std::shared_ptr<HelpSystem> HelpSystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<HelpSystem>(new HelpSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > HelpSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData HelpSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "I"
            };
        }

        void HelpSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if ("Errors" == value)
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

        void HelpSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Documentation"]->setText(_getText(DJV_TEXT("Documentation")));
                p.actions["About"]->setText(_getText(DJV_TEXT("About")));
                p.actions["Errors"]->setText(_getText(DJV_TEXT("Errors")));
                p.actions["SystemLog"]->setText(_getText(DJV_TEXT("System Log")));
                p.actions["Debug"]->setText(_getText(DJV_TEXT("Debugging")));

                p.menu->setText(_getText(DJV_TEXT("Help")));
            }
        }

        void HelpSystem::_errorsPopup()
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
                    auto weak = std::weak_ptr<HelpSystem>(std::dynamic_pointer_cast<HelpSystem>(shared_from_this()));
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

