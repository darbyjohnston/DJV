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

#include <djvViewLib/ImageViewSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>

#include <djvCore/Context.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct ImageViewSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void ImageViewSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::ImageViewSystem", context);

            DJV_PRIVATE_PTR();
            p.actions["Left"] = UI::Action::create();
            p.actions["Left"]->setText(DJV_TEXT("Left"));
            p.actions["Left"]->setShortcut(GLFW_KEY_KP_4, GLFW_MOD_CONTROL);
            //! \todo Implement me!
            p.actions["Left"]->setEnabled(false);
        }

        ImageViewSystem::ImageViewSystem() :
            _p(new Private)
        {}

        ImageViewSystem::~ImageViewSystem()
        {}

        std::shared_ptr<ImageViewSystem> ImageViewSystem::create(Context * context)
        {
            auto out = std::shared_ptr<ImageViewSystem>(new ImageViewSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ImageViewSystem::getActions()
        {
            return _p->actions;
        }

        std::string ImageViewSystem::getMenuSortKey() const
        {
            return "2";
        }

        std::shared_ptr<UI::Menu> ImageViewSystem::createMenu()
        {
            DJV_PRIVATE_PTR();
            auto menu = UI::Menu::create("View", getContext());
            menu->addAction(p.actions["Left"]);
            return menu;
        }

    } // namespace ViewLib
} // namespace djv

