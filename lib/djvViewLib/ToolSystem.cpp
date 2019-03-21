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

#include <djvViewLib/ToolSystem.h>

#include <djvViewLib/ColorPickerTool.h>
#include <djvViewLib/DebugTool.h>
#include <djvViewLib/HistogramTool.h>
#include <djvViewLib/InformationTool.h>
#include <djvViewLib/MagnifierTool.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct ToolSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::map<std::string, std::shared_ptr<ITool> > tools;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > checkedObservers;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void ToolSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::ToolSystem", context);

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

            p.tools["Magnifier"] = MagnifierTool::create(context);
            p.tools["ColorPicker"] = ColorPickerTool::create(context);
            p.tools["Histogram"] = HistogramTool::create(context);
            p.tools["Information"] = InformationTool::create(context);
            p.tools["Debug"] = DebugTool::create(context);

            auto weak = std::weak_ptr<ToolSystem>(std::dynamic_pointer_cast<ToolSystem>(shared_from_this()));
            p.tools["Magnifier"]->setCloseCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    system->_p->actions["Magnifier"]->setChecked(false);
                    system->_p->actions["Magnifier"]->doChecked();
                }
            });
            p.tools["ColorPicker"]->setCloseCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    system->_p->actions["ColorPicker"]->setChecked(false);
                    system->_p->actions["ColorPicker"]->doChecked();
                }
            });
            p.tools["Histogram"]->setCloseCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    system->_p->actions["Histogram"]->setChecked(false);
                    system->_p->actions["Histogram"]->doChecked();
                }
            });
            p.tools["Information"]->setCloseCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    system->_p->actions["Information"]->setChecked(false);
                    system->_p->actions["Information"]->doChecked();
                }
            });
            p.tools["Debug"]->setCloseCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    system->_p->actions["Debug"]->setChecked(false);
                    system->_p->actions["Debug"]->doChecked();
                }
            });

            p.checkedObservers["Magnifier"] = ValueObserver<bool>::create(
                p.actions["Magnifier"]->observeChecked(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (value)
                    {
                        system->_p->tools["Magnifier"]->moveToFront();
                    }
                    system->_p->tools["Magnifier"]->setVisible(value);
                }
            });
            p.checkedObservers["ColorPicker"] = ValueObserver<bool>::create(
                p.actions["ColorPicker"]->observeChecked(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (value)
                    {
                        system->_p->tools["ColorPicker"]->moveToFront();
                    }
                    system->_p->tools["ColorPicker"]->setVisible(value);
                }
            });
            p.checkedObservers["Histogram"] = ValueObserver<bool>::create(
                p.actions["Histogram"]->observeChecked(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (value)
                    {
                        system->_p->tools["Histogram"]->moveToFront();
                    }
                    system->_p->tools["Histogram"]->setVisible(value);
                }
            });
            p.checkedObservers["Information"] = ValueObserver<bool>::create(
                p.actions["Information"]->observeChecked(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (value)
                    {
                        system->_p->tools["Information"]->moveToFront();
                    }
                    system->_p->tools["Information"]->setVisible(value);
                }
            });
            p.checkedObservers["Debug"] = ValueObserver<bool>::create(
                p.actions["Debug"]->observeChecked(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (value)
                    {
                        system->_p->tools["Debug"]->moveToFront();
                    }
                    system->_p->tools["Debug"]->setVisible(value);
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

        std::vector<std::shared_ptr<ITool> > ToolSystem::getTools()
        {
            DJV_PRIVATE_PTR();
            return
            {
                p.tools["Magnifier"],
                p.tools["ColorPicker"],
                p.tools["Histogram"],
                p.tools["Information"],
                p.tools["Debug"]
            };
        }

        MenuData ToolSystem::getMenu()
        {
            return
            {
                _p->menu,
                "E"
            };
        }

        void ToolSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.actions["Magnifier"]->setTitle(context->getText(DJV_TEXT("Magnifier")));
            p.actions["Magnifier"]->setTooltip(context->getText(DJV_TEXT("Magnifier tooltip")));
            p.actions["ColorPicker"]->setTitle(context->getText(DJV_TEXT("Color Picker")));
            p.actions["ColorPicker"]->setTooltip(context->getText(DJV_TEXT("Color picker tooltip")));
            p.actions["Histogram"]->setTitle(context->getText(DJV_TEXT("Histogram")));
            p.actions["Histogram"]->setTooltip(context->getText(DJV_TEXT("Histogram tooltip")));
            p.actions["Information"]->setTitle(context->getText(DJV_TEXT("Information")));
            p.actions["Information"]->setTooltip(context->getText(DJV_TEXT("Information tooltip")));
            p.actions["Debug"]->setTitle(context->getText(DJV_TEXT("Debugging")));
            p.actions["Debug"]->setTooltip(context->getText(DJV_TEXT("Debugging tooltip")));

            p.menu->setText(context->getText(DJV_TEXT("Tools")));
        }

    } // namespace ViewLib
} // namespace djv

