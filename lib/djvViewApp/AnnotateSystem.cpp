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

#include <djvViewApp/AnnotateSystem.h>

#include <djvViewApp/AnnotateWidget.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AnnotateSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void AnnotateSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IToolSystem::_init("djv::ViewApp::AnnotateSystem", context);

            DJV_PRIVATE_PTR();
            p.actions["Annotate"] = UI::Action::create();
            p.actions["Annotate"]->setIcon("djvIconAnnotate");
            p.actions["Annotate"]->setShortcut(GLFW_KEY_4);

            p.menu = UI::Menu::create(context);

            auto weak = std::weak_ptr<AnnotateSystem>(std::dynamic_pointer_cast<AnnotateSystem>(shared_from_this()));
            _setCloseWidgetCallback(
                [weak](const std::string & name)
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

        AnnotateSystem::AnnotateSystem() :
            _p(new Private)
        {}

        AnnotateSystem::~AnnotateSystem()
        {}

        std::shared_ptr<AnnotateSystem> AnnotateSystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<AnnotateSystem>(new AnnotateSystem);
            out->_init(context);
            return out;
        }

        ToolActionData AnnotateSystem::getToolAction() const
        {
            return
            {
                _p->actions["Annotate"],
                "G"
            };
        }

        void AnnotateSystem::setCurrentTool(bool value)
        {
            if (value)
            {
                if (auto context = getContext().lock())
                {
                    _openWidget("Annotate", AnnotateWidget::create(context));
                }
            }
            else
            {
                _closeWidget("Annotate");
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > AnnotateSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData AnnotateSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "G"
            };
        }

        void AnnotateSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["Annotate"]->setText(_getText(DJV_TEXT("Annotate")));
            p.actions["Annotate"]->setTooltip(_getText(DJV_TEXT("Annotate tooltip")));

            p.menu->setText(_getText(DJV_TEXT("Annotate")));
        }

    } // namespace ViewApp
} // namespace djv

