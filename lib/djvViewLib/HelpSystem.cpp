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

#include <djvUI/Action.h>
#include <djvUI/Menu.h>

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
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
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

            //! \todo Implement me!
            p.actions["About"] = UI::Action::create();
            p.actions["About"]->setEnabled(false);
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

        std::string HelpSystem::getMenuSortKey() const
        {
            return "6";
        }

        std::shared_ptr<UI::Menu> HelpSystem::createMenu()
        {
            DJV_PRIVATE_PTR();
            p.menus["Help"] = UI::Menu::create(_getText("Help"), getContext());
            p.menus["Help"]->addAction(p.actions["Documentation"]);
            p.menus["Help"]->addAction(p.actions["Information"]);
            p.menus["Help"]->addAction(p.actions["About"]);
            return p.menus["Help"];
        }

        void HelpSystem::_localeChangedEvent(Event::LocaleChanged &)
        {
            DJV_PRIVATE_PTR();
            p.actions["Documentation"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Documentation")));
            p.actions["Information"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Information")));
            p.actions["About"]->setText(_getText(DJV_TEXT("djv::ViewLib", "About")));

            p.menus["Help"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib", "Help")));
        }

    } // namespace ViewLib
} // namespace djv

