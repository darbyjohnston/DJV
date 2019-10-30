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

#include <djvViewApp/AnnotateSettings.h>
#include <djvViewApp/AnnotateWidget.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AnnotateSystem::Private
        {
            std::shared_ptr<AnnotateSettings> settings;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::weak_ptr<AnnotateWidget> widget;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
        };

        void AnnotateSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IToolSystem::_init("djv::ViewApp::AnnotateSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = AnnotateSettings::create(context);
            _setWidgetGeom(p.settings->getWidgetGeom());

            //! \todo Implement me!
            p.actions["Annotate"] = UI::Action::create();
            p.actions["Annotate"]->setIcon("djvIconAnnotate");
            p.actions["Annotate"]->setShortcut(GLFW_KEY_7);
            p.actions["Annotate"]->setEnabled(false);

            p.menu = UI::Menu::create(context);
        }

        AnnotateSystem::AnnotateSystem() :
            _p(new Private)
        {}

        AnnotateSystem::~AnnotateSystem()
        {
            DJV_PRIVATE_PTR();
            _closeWidget("Annotate");
            p.settings->setWidgetGeom(_getWidgetGeom());
        }

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
            DJV_PRIVATE_PTR();
            if (value && p.widget.expired())
            {
                if (auto context = getContext().lock())
                {
                    auto widget = AnnotateWidget::create(context);
                    p.widget = widget;
                    _openWidget("Annotate", widget);
                }
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

        void AnnotateSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            const auto i = p.actions.find(value);
            if (i != p.actions.end())
            {
                i->second->setChecked(false);
            }
            p.widget.reset();
            IToolSystem::_closeWidget(value);
        }

        void AnnotateSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Annotate"]->setText(_getText(DJV_TEXT("Annotate")));
                p.actions["Annotate"]->setTooltip(_getText(DJV_TEXT("Annotate tooltip")));
            
                p.menu->setText(_getText(DJV_TEXT("Annotate")));
            }
        }

    } // namespace ViewApp
} // namespace djv

