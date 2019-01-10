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

#include <djvViewLib/ImageSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>

#include <djvCore/Context.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct ImageSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void ImageSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::ImageSystem", context);

            DJV_PRIVATE_PTR();
            //! \todo Implement me!
            p.actions["MirrorH"] = UI::Action::create();
            p.actions["MirrorH"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["MirrorH"]->setText(DJV_TEXT("djv::ViewLib", "Mirror Horizontal"));
            p.actions["MirrorH"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["MirrorV"] = UI::Action::create();
            p.actions["MirrorV"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["MirrorV"]->setText(DJV_TEXT("djv::ViewLib", "Mirror Vertical"));
            p.actions["MirrorV"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["ColorManager"] = UI::Action::create();
            p.actions["ColorManager"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorManager"]->setText(DJV_TEXT("djv::ViewLib", "Color Manager"));
            p.actions["ColorManager"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["ColorProfile"] = UI::Action::create();
            p.actions["ColorProfile"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorProfile"]->setText(DJV_TEXT("djv::ViewLib", "Color Profile"));
            p.actions["ColorProfile"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["DisplayProfile"] = UI::Action::create();
            p.actions["DisplayProfile"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["DisplayProfile"]->setText(DJV_TEXT("djv::ViewLib", "Display Profile"));
            p.actions["DisplayProfile"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["PremultAlpha"] = UI::Action::create();
            p.actions["PremultAlpha"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["PremultAlpha"]->setText(DJV_TEXT("djv::ViewLib", "Premulitplied Alpha"));
            p.actions["PremultAlpha"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["ShowFrameStore"] = UI::Action::create();
            p.actions["ShowFrameStore"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ShowFrameStore"]->setText(DJV_TEXT("djv::ViewLib", "Show Frame Store"));
            p.actions["ShowFrameStore"]->setShortcut(GLFW_KEY_E);
            p.actions["ShowFrameStore"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["SetFrameStore"] = UI::Action::create();
            p.actions["SetFrameStore"]->setText(DJV_TEXT("djv::ViewLib", "Set Frame Store"));
            p.actions["SetFrameStore"]->setShortcut(GLFW_KEY_E, GLFW_MOD_SHIFT);
            p.actions["SetFrameStore"]->setEnabled(false);
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

        std::string ImageSystem::getMenuSortKey() const
        {
            return "3";
        }

        std::shared_ptr<UI::Menu> ImageSystem::createMenu()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            auto menu = UI::Menu::create("Image", context);
            menu->addAction(p.actions["MirrorH"]);
            menu->addAction(p.actions["MirrorV"]);
            //! \todo Implement me!
            auto scaleMenu = UI::Menu::create(DJV_TEXT("djv::ViewLib", "Scale"), context);
            menu->addMenu(scaleMenu);
            //! \todo Implement me!
            auto rotateMenu = UI::Menu::create(DJV_TEXT("djv::ViewLib", "Rotate"), context);
            menu->addMenu(rotateMenu);
            menu->addSeparator();
            menu->addAction(p.actions["ColorManager"]);
            menu->addAction(p.actions["ColorProfile"]);
            menu->addAction(p.actions["DisplayProfile"]);
            menu->addAction(p.actions["PremultAlpha"]);
            menu->addSeparator();
            menu->addAction(p.actions["ShowFrameStore"]);
            menu->addAction(p.actions["SetFrameStore"]);
            return menu;
        }

    } // namespace ViewLib
} // namespace djv

