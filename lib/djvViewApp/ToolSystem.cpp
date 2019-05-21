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

#include <djvViewApp/ColorPickerTool.h>
#include <djvViewApp/DebugTool.h>
#include <djvViewApp/HistogramTool.h>
#include <djvViewApp/InformationTool.h>
#include <djvViewApp/MagnifierTool.h>

#include <djvUI/Action.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ToolSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void ToolSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewApp::ToolSystem", context);

            DJV_PRIVATE_PTR();
            p.actions["Magnifier"] = UI::Action::create();
            p.actions["Magnifier"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Magnifier"]->setShortcut(GLFW_KEY_4);
            p.actions["ColorPicker"] = UI::Action::create();
            p.actions["ColorPicker"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorPicker"]->setShortcut(GLFW_KEY_5);
            p.actions["Histogram"] = UI::Action::create();
            p.actions["Histogram"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Histogram"]->setShortcut(GLFW_KEY_6);
            p.actions["Information"] = UI::Action::create();
            p.actions["Information"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Information"]->setShortcut(GLFW_KEY_7);
            p.actions["Debug"] = UI::Action::create();
            p.actions["Debug"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Debug"]->setShortcut(GLFW_KEY_8);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Magnifier"]);
            p.menu->addAction(p.actions["ColorPicker"]);
            p.menu->addAction(p.actions["Histogram"]);
            p.menu->addAction(p.actions["Information"]);
            p.menu->addAction(p.actions["Debug"]);

            auto weak = std::weak_ptr<ToolSystem>(std::dynamic_pointer_cast<ToolSystem>(shared_from_this()));
            _setCloseToolCallback(
                [weak](const std::string& name)
            {
                if (auto system = weak.lock())
                {
                    const auto i = system->_p->actions.find(name);
                    if (i != system->_p->actions.end())
                    {
                        i->second->setChecked(false);
                    }
                }
            });

            p.clickedObservers["Magnifier"] = ValueObserver<bool>::create(
                p.actions["Magnifier"]->observeChecked(),
                [weak, context](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (value)
                    {
                        system->_openTool("Magnifier", MagnifierTool::create(context));
                    }
                    else
                    {
                        system->_closeTool("Magnifier");
                    }
                }
            });
            p.clickedObservers["ColorPicker"] = ValueObserver<bool>::create(
                p.actions["ColorPicker"]->observeChecked(),
                [weak, context](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (value)
                    {
                        system->_openTool("ColorPicker", ColorPickerTool::create(context));
                    }
                    else
                    {
                        system->_closeTool("ColorPicker");
                    }
                }
            });
            p.clickedObservers["Histogram"] = ValueObserver<bool>::create(
                p.actions["Histogram"]->observeChecked(),
                [weak, context](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (value)
                    {
                        system->_openTool("Histogram", HistogramTool::create(context));
                    }
                    else
                    {
                        system->_closeTool("Histogram");
                    }
                }
            });
            p.clickedObservers["Information"] = ValueObserver<bool>::create(
                p.actions["Information"]->observeChecked(),
                [weak, context](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (value)
                    {
                        system->_openTool("Information", InformationTool::create(context));
                    }
                    else
                    {
                        system->_closeTool("Information");
                    }
                }
            });
            p.clickedObservers["Debug"] = ValueObserver<bool>::create(
                p.actions["Debug"]->observeChecked(),
                [weak, context](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (value)
                    {
                        system->_openTool("Debug", DebugTool::create(context));
                    }
                    else
                    {
                        system->_closeTool("Debug");
                    }
                }
            });

            p.localeObserver = ValueObserver<std::string>::create(
                context->getSystemT<TextSystem>()->observeCurrentLocale(),
                [weak](const std::string & value)
            {
                if (auto system = weak.lock())
                {
                    system->_textUpdate();
                }
            });
        }

        ToolSystem::ToolSystem() :
            _p(new Private)
        {}

        ToolSystem::~ToolSystem()
        {}

        std::shared_ptr<ToolSystem> ToolSystem::create(Context * context)
        {
            auto out = std::shared_ptr<ToolSystem>(new ToolSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ToolSystem::getActions()
        {
            return _p->actions;
        }

        MenuData ToolSystem::getMenu()
        {
            return
            {
                _p->menu,
                "F"
            };
        }

        void ToolSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["Magnifier"]->setText(_getText(DJV_TEXT("Magnifier")));
            p.actions["Magnifier"]->setTooltip(_getText(DJV_TEXT("Magnifier tooltip")));
            p.actions["ColorPicker"]->setText(_getText(DJV_TEXT("Color Picker")));
            p.actions["ColorPicker"]->setTooltip(_getText(DJV_TEXT("Color picker tooltip")));
            p.actions["Histogram"]->setText(_getText(DJV_TEXT("Histogram")));
            p.actions["Histogram"]->setTooltip(_getText(DJV_TEXT("Histogram tooltip")));
            p.actions["Information"]->setText(_getText(DJV_TEXT("Information")));
            p.actions["Information"]->setTooltip(_getText(DJV_TEXT("Information tooltip")));
            p.actions["Debug"]->setText(_getText(DJV_TEXT("Debugging")));
            p.actions["Debug"]->setTooltip(_getText(DJV_TEXT("Debugging tooltip")));

            p.menu->setText(_getText(DJV_TEXT("Tools")));
        }

    } // namespace ViewApp
} // namespace djv

