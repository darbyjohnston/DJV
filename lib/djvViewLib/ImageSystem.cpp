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
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void ImageSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::ImageSystem", context);

            DJV_PRIVATE_PTR();
            //! \todo Implement me!
            p.actions["MirrorH"] = UI::Action::create();
            p.actions["MirrorH"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["MirrorH"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["MirrorV"] = UI::Action::create();
            p.actions["MirrorV"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["MirrorV"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["ColorManager"] = UI::Action::create();
            p.actions["ColorManager"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorManager"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["ColorProfile"] = UI::Action::create();
            p.actions["ColorProfile"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorProfile"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["DisplayProfile"] = UI::Action::create();
            p.actions["DisplayProfile"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["DisplayProfile"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["PremultAlpha"] = UI::Action::create();
            p.actions["PremultAlpha"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["PremultAlpha"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["ShowFrameStore"] = UI::Action::create();
            p.actions["ShowFrameStore"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ShowFrameStore"]->setShortcut(GLFW_KEY_E);
            p.actions["ShowFrameStore"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["SetFrameStore"] = UI::Action::create();
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

        NewMenu ImageSystem::createMenu()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.menus["Image"] = UI::Menu::create(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Image")), context);
            p.menus["Image"]->addAction(p.actions["MirrorH"]);
            p.menus["Image"]->addAction(p.actions["MirrorV"]);
            //! \todo Implement me!
            p.menus["Scale"] = UI::Menu::create(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Scale")), context);
            p.menus["Image"]->addMenu(p.menus["Scale"]);
            //! \todo Implement me!
            p.menus["Rotate"] = UI::Menu::create(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Rotate")), context);
            p.menus["Image"]->addMenu(p.menus["Rotate"]);
            p.menus["Image"]->addSeparator();
            p.menus["Image"]->addAction(p.actions["ColorManager"]);
            p.menus["Image"]->addAction(p.actions["ColorProfile"]);
            p.menus["Image"]->addAction(p.actions["DisplayProfile"]);
            p.menus["Image"]->addAction(p.actions["PremultAlpha"]);
            p.menus["Image"]->addSeparator();
            p.menus["Image"]->addAction(p.actions["ShowFrameStore"]);
            p.menus["Image"]->addAction(p.actions["SetFrameStore"]);
            return { p.menus["Image"], "D" };
        }

        void ImageSystem::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.actions["MirrorH"]->setText(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Mirror Horizontal")));
            p.actions["MirrorH"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Mirror Horizontal Tooltip")));
            p.actions["MirrorV"]->setText(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Mirror Vertical")));
            p.actions["MirrorV"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Mirror Vertical Tooltip")));
            p.actions["ColorManager"]->setText(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Color Manager")));
            p.actions["ColorManager"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Color Manager Tooltip")));
            p.actions["ColorProfile"]->setText(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Color Profile")));
            p.actions["ColorProfile"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Color Profile Tooltip")));
            p.actions["DisplayProfile"]->setText(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Display Profile")));
            p.actions["DisplayProfile"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Display Profile Tooltip")));
            p.actions["PremultAlpha"]->setText(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Premulitplied Alpha")));
            p.actions["PremultAlpha"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Premulitplied Alpha Tooltip")));
            p.actions["ShowFrameStore"]->setText(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Show Frame Store")));
            p.actions["ShowFrameStore"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Show Frame Store Tooltip")));
            p.actions["SetFrameStore"]->setText(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Set Frame Store")));
            p.actions["SetFrameStore"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Set Frame Store Tooltip")));

            p.menus["Image"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Image")));
            p.menus["Scale"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Scale")));
            p.menus["Rotate"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib::ImageSystem", "Rotate")));
        }

    } // namespace ViewLib
} // namespace djv

