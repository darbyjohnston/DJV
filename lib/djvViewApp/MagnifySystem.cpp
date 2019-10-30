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

#include <djvViewApp/MagnifySystem.h>

#include <djvViewApp/MagnifySettings.h>
#include <djvViewApp/MagnifyWidget.h>

#include <djvUI/Action.h>
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
        struct MagnifySystem::Private
        {
            std::shared_ptr<MagnifySettings> settings;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::weak_ptr<MagnifyWidget> widget;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
        };

        void MagnifySystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IToolSystem::_init("djv::ViewApp::MagnifySystem", context);
            DJV_PRIVATE_PTR();

            p.settings = MagnifySettings::create(context);
            _setWidgetGeom(p.settings->getWidgetGeom());

            //! \todo Implement me!
            p.actions["Magnify"] = UI::Action::create();
            p.actions["Magnify"]->setIcon("djvIconMagnify");
            p.actions["Magnify"]->setShortcut(GLFW_KEY_6);
            p.actions["Magnify"]->setEnabled(false);
        }

        MagnifySystem::MagnifySystem() :
            _p(new Private)
        {}

        MagnifySystem::~MagnifySystem()
        {
            DJV_PRIVATE_PTR();
            _closeWidget("Magnify");
            p.settings->setWidgetGeom(_getWidgetGeom());
        }

        std::shared_ptr<MagnifySystem> MagnifySystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<MagnifySystem>(new MagnifySystem);
            out->_init(context);
            return out;
        }

        ToolActionData MagnifySystem::getToolAction() const
        {
            return
            {
                _p->actions["Magnify"],
                "C"
            };
        }

        void MagnifySystem::setCurrentTool(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value && p.widget.expired())
            {
                if (auto context = getContext().lock())
                {
                    auto widget = MagnifyWidget::create(context);
                    p.widget = widget;
                    _openWidget("Magnify", widget);
                }
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > MagnifySystem::getActions() const
        {
            return _p->actions;
        }

        void MagnifySystem::_closeWidget(const std::string& value)
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

        void MagnifySystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Magnify"]->setText(_getText(DJV_TEXT("Magnify")));
                p.actions["Magnify"]->setTooltip(_getText(DJV_TEXT("Magnify tooltip")));
            }
        }

    } // namespace ViewApp
} // namespace djv

