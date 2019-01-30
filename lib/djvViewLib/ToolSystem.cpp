//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
#include <djvViewLib/HistogramTool.h>
#include <djvViewLib/InformationTool.h>
#include <djvViewLib/MagnifierTool.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>

#include <djvCore/Context.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct ToolSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            std::map<std::string, std::shared_ptr<IToolWidget> > toolWidgets;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > checkedObservers;
        };

        void ToolSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::ToolSystem", context);

            DJV_PRIVATE_PTR();
            p.actions["Magnifier"] = UI::Action::create();
            p.actions["Magnifier"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Magnifier"]->setShortcut(GLFW_KEY_1);

            p.actions["ColorPicker"] = UI::Action::create();
            p.actions["ColorPicker"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorPicker"]->setShortcut(GLFW_KEY_2);

            p.actions["Histogram"] = UI::Action::create();
            p.actions["Histogram"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Histogram"]->setShortcut(GLFW_KEY_3);

            p.actions["Information"] = UI::Action::create();
            p.actions["Information"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Information"]->setShortcut(GLFW_KEY_4);

            p.menus["Tools"] = UI::Menu::create(context);
            p.menus["Tools"]->addAction(p.actions["Magnifier"]);
            p.menus["Tools"]->addAction(p.actions["ColorPicker"]);
            p.menus["Tools"]->addAction(p.actions["Histogram"]);
            p.menus["Tools"]->addAction(p.actions["Information"]);

            p.toolWidgets["Magnifier"] = MagnifierTool::create(context);
            p.toolWidgets["Magnifier"]->hide();
            p.toolWidgets["ColorPicker"] = ColorPickerTool::create(context);
            p.toolWidgets["ColorPicker"]->hide();
            p.toolWidgets["Histogram"] = HistogramTool::create(context);
            p.toolWidgets["Histogram"]->hide();
            p.toolWidgets["Information"] = InformationTool::create(context);
            p.toolWidgets["Information"]->hide();

            auto weak = std::weak_ptr<ToolSystem>(std::dynamic_pointer_cast<ToolSystem>(shared_from_this()));
            p.checkedObservers["Magnifier"] = ValueObserver<bool>::create(
                p.actions["Magnifier"]->observeChecked(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->toolWidgets["Magnifier"]->setVisible(value);
                }
            });
            p.checkedObservers["ColorPicker"] = ValueObserver<bool>::create(
                p.actions["ColorPicker"]->observeChecked(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->toolWidgets["ColorPicker"]->setVisible(value);
                }
            });
            p.checkedObservers["Histogram"] = ValueObserver<bool>::create(
                p.actions["Histogram"]->observeChecked(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->toolWidgets["Histogram"]->setVisible(value);
                }
            });
            p.checkedObservers["Information"] = ValueObserver<bool>::create(
                p.actions["Information"]->observeChecked(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->toolWidgets["Information"]->setVisible(value);
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

        NewMenu ToolSystem::getMenu()
        {
            DJV_PRIVATE_PTR();
            return { p.menus["Tools"], "F" };
        }

        std::vector<NewToolWidget> ToolSystem::getToolWidgets()
        {
            DJV_PRIVATE_PTR();
            return
            {
                { p.toolWidgets["Magnifier"], "F1" },
                { p.toolWidgets["ColorPicker"], "F2" },
                { p.toolWidgets["Histogram"], "F3" },
                { p.toolWidgets["Information"], "F4" }
            };
        }

        void ToolSystem::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.menus["Tools"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib::ToolSystem", "Tools")));
            p.actions["Magnifier"]->setText(_getText(DJV_TEXT("djv::ViewLib::ToolSystem", "Magnifier")));
            p.actions["Magnifier"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ToolSystem", "Magnifier Tooltip")));
            p.actions["ColorPicker"]->setText(_getText(DJV_TEXT("djv::ViewLib::ToolSystem", "Color Picker")));
            p.actions["ColorPicker"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ToolSystem", "Color Picker Tooltip")));
            p.actions["Histogram"]->setText(_getText(DJV_TEXT("djv::ViewLib::ToolSystem", "Histogram")));
            p.actions["Histogram"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ToolSystem", "Histogram Tooltip")));
            p.actions["Information"]->setText(_getText(DJV_TEXT("djv::ViewLib::ToolSystem", "Information")));
            p.actions["Information"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ToolSystem", "Information Tooltip")));
        }

    } // namespace ViewLib
} // namespace djv

