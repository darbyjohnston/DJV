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

#include <djvViewLib/HelpSystem.h>

#include <djvViewLib/AboutDialog.h>
#include <djvViewLib/LogDialog.h>

#include <djvUIComponents/ActionButton.h>

#include <djvUI/Action.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvCore/Context.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct HelpSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
			std::shared_ptr<AboutDialog> aboutDialog;
			std::shared_ptr<LogDialog> logDialog;
            std::shared_ptr<UI::PopupWidget> toolBarWidget;
			std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void HelpSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::HelpSystem", context);

            DJV_PRIVATE_PTR();
            //! \todo Implement me!
            p.actions["Documentation"] = UI::Action::create();
            p.actions["Documentation"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["Information"] = UI::Action::create();
            p.actions["Information"]->setEnabled(false);
            p.actions["About"] = UI::Action::create();
            p.actions["Log"] = UI::Action::create();

            p.aboutDialog = AboutDialog::create(context);
            p.logDialog = LogDialog::create(context);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addWidget(UI::ActionButton::create(p.actions["Documentation"], context));
            vLayout->addSeparator();
            vLayout->addWidget(UI::ActionButton::create(p.actions["About"], context));
            vLayout->addSeparator();
            vLayout->addWidget(UI::ActionButton::create(p.actions["Log"], context));
            p.toolBarWidget = UI::PopupWidget::create(context);
            p.toolBarWidget->setIcon("djvIconHelp");
            p.toolBarWidget->setWidget(vLayout);

            auto weak = std::weak_ptr<HelpSystem>(std::dynamic_pointer_cast<HelpSystem>(shared_from_this()));
            p.aboutDialog->setCloseCallback(
                [weak, context]
            {
                if (auto system = weak.lock())
                {
                    system->_p->aboutDialog->hide();
                    system->_p->aboutDialog->setParent(nullptr);
                }
            });

            p.logDialog->setCloseCallback(
                [weak, context]
            {
                if (auto system = weak.lock())
                {
                    system->_p->logDialog->hide();
                    system->_p->logDialog->setParent(nullptr);
                }
            });

            p.clickedObservers["About"] = ValueObserver<bool>::create(
                p.actions["About"]->observeClicked(),
                [weak, context](bool value)
            {
				if (value)
				{
					if (auto system = weak.lock())
					{
                        if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
                        {
                            if (auto window = windowSystem->observeCurrentWindow()->get())
                            {
                                system->_p->toolBarWidget->close();
                                window->addWidget(system->_p->aboutDialog);
                                system->_p->aboutDialog->show();
                            }
                        }
					}
				}
            });

            p.clickedObservers["Log"] = ValueObserver<bool>::create(
                p.actions["Log"]->observeClicked(),
                [weak, context](bool value)
            {
				if (value)
				{
					if (auto system = weak.lock())
					{
                        if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
                        {
                            if (auto window = windowSystem->observeCurrentWindow()->get())
                            {
                                system->_p->toolBarWidget->close();
                                if (value)
                                {
                                    system->_p->logDialog->reloadLog();
                                }
                                else
                                {
                                    system->_p->logDialog->clearLog();
                                }
                                window->addWidget(system->_p->logDialog);
                                system->_p->logDialog->show();
                            }
                        }
					}
				}
            });
        }

        HelpSystem::HelpSystem() :
            _p(new Private)
        {}

        HelpSystem::~HelpSystem()
        {}

        std::shared_ptr<HelpSystem> HelpSystem::create(Context * context)
        {
            auto out = std::shared_ptr<HelpSystem>(new HelpSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > HelpSystem::getActions()
        {
            return _p->actions;
        }

        ToolBarWidget HelpSystem::getToolBarWidget()
        {
            return
            {
                _p->toolBarWidget,
                "F"
            };
        }

        void HelpSystem::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.actions["Documentation"]->setText(_getText(DJV_TEXT("Documentation")));
            p.actions["Documentation"]->setTooltip(_getText(DJV_TEXT("Documentation tooltip")));
            p.actions["About"]->setText(_getText(DJV_TEXT("About")));
            p.actions["About"]->setTooltip(_getText(DJV_TEXT("About tooltip")));
            p.actions["Log"]->setText(_getText(DJV_TEXT("System Log")));
            p.actions["Log"]->setTooltip(_getText(DJV_TEXT("System log tooltip")));

            p.toolBarWidget->setTooltip(_getText(DJV_TEXT("Help system tool bar tooltip")));
        }

    } // namespace ViewLib
} // namespace djv

