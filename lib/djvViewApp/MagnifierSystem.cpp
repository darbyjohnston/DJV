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

#include <djvViewApp/MagnifierSystem.h>

#include <djvViewApp/MagnifierWidget.h>

#include <djvUI/Action.h>
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
        struct MagnifierSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void MagnifierSystem::_init(Context * context)
        {
            IToolSystem::_init("djv::ViewApp::MagnifierSystem", context);

            DJV_PRIVATE_PTR();
            p.actions["Magnifier"] = UI::Action::create();
            p.actions["Magnifier"]->setIcon("djvIconMagnifier");
            p.actions["Magnifier"]->setShortcut(GLFW_KEY_3);

            auto weak = std::weak_ptr<MagnifierSystem>(std::dynamic_pointer_cast<MagnifierSystem>(shared_from_this()));
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

        MagnifierSystem::MagnifierSystem() :
            _p(new Private)
        {}

        MagnifierSystem::~MagnifierSystem()
        {}

        std::shared_ptr<MagnifierSystem> MagnifierSystem::create(Context * context)
        {
            auto out = std::shared_ptr<MagnifierSystem>(new MagnifierSystem);
            out->_init(context);
            return out;
        }

        ToolActionData MagnifierSystem::getToolAction() const
        {
            return
            {
                _p->actions["Magnifier"],
                "C"
            };
        }

        void MagnifierSystem::setCurrentTool(bool value)
        {
            if (value)
            {
                _openWidget("Magnifier", MagnifierWidget::create(getContext()));
            }
            else
            {
                _closeWidget("Magnifier");
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > MagnifierSystem::getActions() const
        {
            return _p->actions;
        }

        void MagnifierSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["Magnifier"]->setText(_getText(DJV_TEXT("Magnifier")));
            p.actions["Magnifier"]->setTooltip(_getText(DJV_TEXT("Magnifier tooltip")));
        }

    } // namespace ViewApp
} // namespace djv

