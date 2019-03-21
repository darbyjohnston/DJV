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

#include <djvViewLib/ImageSystem.h>

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
        struct ImageSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void ImageSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::ImageSystem", context);

            DJV_PRIVATE_PTR();

            //! \todo Implement me!
            p.actions["ColorManager"] = UI::Action::create();
            p.actions["ColorManager"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["FrameStore"] = UI::Action::create();
            p.actions["FrameStore"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FrameStore"]->setShortcut(GLFW_KEY_E);
            p.actions["FrameStore"]->setEnabled(false);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["ColorManager"]);
            p.menu->addAction(p.actions["FrameStore"]);

            auto weak = std::weak_ptr<ImageSystem>(std::dynamic_pointer_cast<ImageSystem>(shared_from_this()));
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

        ImageSystem::ImageSystem() :
            _p(new Private)
        {}

        ImageSystem::~ImageSystem()
        {}

        std::shared_ptr<ImageSystem> ImageSystem::create(Context * context)
        {
            auto out = std::shared_ptr<ImageSystem>(new ImageSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ImageSystem::getActions()
        {
            return _p->actions;
        }

        MenuData ImageSystem::getMenu()
        {
            return
            {
                _p->menu,
                "D"
            };
        }

        void ImageSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.actions["ColorManager"]->setTitle(context->getText(DJV_TEXT("Color Manager")));
            p.actions["ColorManager"]->setTooltip(context->getText(DJV_TEXT("Color manager tooltip")));
            p.actions["FrameStore"]->setTitle(context->getText(DJV_TEXT("Frame Store")));
            p.actions["FrameStore"]->setTooltip(context->getText(DJV_TEXT("Frame store tooltip")));

            p.menu->setText(context->getText(DJV_TEXT("Image")));
        }

    } // namespace ViewLib
} // namespace djv

