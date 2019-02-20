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

#include <djvViewLib/ColorPickerWidget.h>
#include <djvViewLib/DebugWidget.h>
#include <djvViewLib/HistogramWidget.h>
#include <djvViewLib/InformationWidget.h>
#include <djvViewLib/MagnifierWidget.h>

#include <djvUIComponents/ActionButton.h>

#include <djvUI/Action.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>

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
            std::shared_ptr<UI::PopupWidget> toolBarWidget;
            std::map<std::string, std::shared_ptr<IToolWidget> > widgets;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > checkedObservers;
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

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addWidget(UI::ActionButton::create(p.actions["Magnifier"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["ColorPicker"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["Histogram"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["Information"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["Debug"], context));
            p.toolBarWidget = UI::PopupWidget::create(context);
            p.toolBarWidget->setIcon("djvIconColorPicker");
            p.toolBarWidget->setWidget(vLayout);

            p.widgets["Magnifier"] = MagnifierWidget::create(context);
            p.widgets["ColorPicker"] = ColorPickerWidget::create(context);
            p.widgets["Histogram"] = HistogramWidget::create(context);
            p.widgets["Information"] = InformationWidget::create(context);
            p.widgets["Debug"] = DebugWidget::create(context);

            auto weak = std::weak_ptr<ToolSystem>(std::dynamic_pointer_cast<ToolSystem>(shared_from_this()));
			p.widgets["Magnifier"]->setCloseCallback(
				[weak]
			{
				if (auto system = weak.lock())
				{
					system->_p->actions["Magnifier"]->setChecked(false);
					system->_p->actions["Magnifier"]->doChecked();
				}
			});
			p.widgets["ColorPicker"]->setCloseCallback(
				[weak]
			{
				if (auto system = weak.lock())
				{
					system->_p->actions["ColorPicker"]->setChecked(false);
					system->_p->actions["ColorPicker"]->doChecked();
				}
			});
			p.widgets["Histogram"]->setCloseCallback(
				[weak]
			{
				if (auto system = weak.lock())
				{
					system->_p->actions["Histogram"]->setChecked(false);
					system->_p->actions["Histogram"]->doChecked();
				}
			});
            p.widgets["Information"]->setCloseCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    system->_p->actions["Information"]->setChecked(false);
                    system->_p->actions["Information"]->doChecked();
                }
            });
            p.widgets["Debug"]->setCloseCallback(
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
						system->_p->widgets["Magnifier"]->moveToFront();
					}
                    system->_p->widgets["Magnifier"]->setVisible(value);
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
						system->_p->widgets["ColorPicker"]->moveToFront();
					}
					system->_p->widgets["ColorPicker"]->setVisible(value);
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
						system->_p->widgets["Histogram"]->moveToFront();
					}
					system->_p->widgets["Histogram"]->setVisible(value);
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
						system->_p->widgets["Information"]->moveToFront();
					}
					system->_p->widgets["Information"]->setVisible(value);
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
						system->_p->widgets["Debug"]->moveToFront();
					}
					system->_p->widgets["Debug"]->setVisible(value);
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

        std::vector<std::shared_ptr<IToolWidget> > ToolSystem::getToolWidgets()
        {
            DJV_PRIVATE_PTR();
            return
            {
                p.widgets["Magnifier"],
                p.widgets["ColorPicker"],
                p.widgets["Histogram"],
                p.widgets["Information"],
                p.widgets["Debug"]
            };
        }

        ToolBarWidget ToolSystem::getToolBarWidget()
        {
            return
            {
                _p->toolBarWidget,
                "E"
            };
        }

        void ToolSystem::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.actions["Magnifier"]->setText(_getText(DJV_TEXT("Magnifier")));
            p.actions["Magnifier"]->setTooltip(_getText(DJV_TEXT("Magnifier tooltip")));
            p.actions["ColorPicker"]->setText(_getText(DJV_TEXT("Color picker")));
            p.actions["ColorPicker"]->setTooltip(_getText(DJV_TEXT("Color picker tooltip")));
            p.actions["Histogram"]->setText(_getText(DJV_TEXT("Histogram")));
            p.actions["Histogram"]->setTooltip(_getText(DJV_TEXT("Histogram tooltip")));
            p.actions["Information"]->setText(_getText(DJV_TEXT("Information")));
            p.actions["Information"]->setTooltip(_getText(DJV_TEXT("Information tooltip")));
            p.actions["Debug"]->setText(_getText(DJV_TEXT("Debugging")));
            p.actions["Debug"]->setTooltip(_getText(DJV_TEXT("Debugging tooltip")));

            p.toolBarWidget->setTooltip(_getText(DJV_TEXT("Tool system tool bar tooltip")));
        }

    } // namespace ViewLib
} // namespace djv

