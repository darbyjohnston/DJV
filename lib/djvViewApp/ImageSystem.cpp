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

#include <djvViewApp/ImageSystem.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

#include <djvAV/Image.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageSystem::Private
        {
            std::shared_ptr<ValueSubject<bool> > frameStoreEnabled;
            std::shared_ptr<ValueSubject<std::shared_ptr<AV::Image::Image> > > frameStore;
            std::shared_ptr<AV::Image::Image> currentImage;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > currentImageObserver;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void ImageSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewApp::ImageSystem", context);

            DJV_PRIVATE_PTR();

            p.frameStoreEnabled = ValueSubject<bool>::create();
            p.frameStore = ValueSubject<std::shared_ptr<AV::Image::Image> >::create();

            //! \todo Implement me!
            p.actions["ColorManager"] = UI::Action::create();
            p.actions["ColorManager"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorManager"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["ColorChannels"] = UI::Action::create();
            p.actions["ColorChannels"]->setShortcut(GLFW_KEY_C);
            p.actions["ColorChannels"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["RedChannel"] = UI::Action::create();
            p.actions["RedChannel"]->setShortcut(GLFW_KEY_R);
            p.actions["RedChannel"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["GreenChannel"] = UI::Action::create();
            p.actions["GreenChannel"]->setShortcut(GLFW_KEY_G);
            p.actions["GreenChannel"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["BlueChannel"] = UI::Action::create();
            p.actions["BlueChannel"]->setShortcut(GLFW_KEY_B);
            p.actions["BlueChannel"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["AlphaChannel"] = UI::Action::create();
            p.actions["AlphaChannel"]->setShortcut(GLFW_KEY_A);
            p.actions["AlphaChannel"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["PremultipliedAlpha"] = UI::Action::create();
            p.actions["PremultipliedAlpha"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["Transform"] = UI::Action::create();
            p.actions["Transform"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Transform"]->setEnabled(false);
            p.actions["FrameStoreEnabled"] = UI::Action::create();
            p.actions["FrameStoreEnabled"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FrameStoreEnabled"]->setShortcut(GLFW_KEY_F);
            p.actions["FrameStoreEnabled"]->setEnabled(false);
            p.actions["LoadFrameStore"] = UI::Action::create();
            p.actions["LoadFrameStore"]->setShortcut(GLFW_KEY_F, GLFW_MOD_SHIFT);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["ColorManager"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["ColorChannels"]);
            p.menu->addAction(p.actions["RedChannel"]);
            p.menu->addAction(p.actions["GreenChannel"]);
            p.menu->addAction(p.actions["BlueChannel"]);
            p.menu->addAction(p.actions["AlphaChannel"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["PremultipliedAlpha"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Transform"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["FrameStoreEnabled"]);
            p.menu->addAction(p.actions["LoadFrameStore"]);

            auto weak = std::weak_ptr<ImageSystem>(std::dynamic_pointer_cast<ImageSystem>(shared_from_this()));
            p.clickedObservers["FrameStoreEnabled"] = ValueObserver<bool>::create(
                p.actions["FrameStoreEnabled"]->observeChecked(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->frameStoreEnabled->setIfChanged(value);
                    }
                });

            p.clickedObservers["LoadFrameStore"] = ValueObserver<bool>::create(
                p.actions["LoadFrameStore"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->actions["FrameStoreEnabled"]->setEnabled(system->_p->currentImage ? true : false);
                            system->_p->frameStore->setIfChanged(system->_p->currentImage);
                        }
                    }
                });

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                system->_p->currentImageObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                                    value->observeCurrentImage(),
                                    [weak](const std::shared_ptr<AV::Image::Image>& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->currentImage = value;
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->currentImage.reset();
                                system->_p->currentImageObserver.reset();
                            }
                        }
                    });
            }

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

        std::shared_ptr<Core::IValueSubject<bool> > ImageSystem::observeFrameStoreEnabled() const
        {
            return _p->frameStoreEnabled;
        }

        std::shared_ptr<IValueSubject<std::shared_ptr<AV::Image::Image> > > ImageSystem::observeFrameStore() const
        {
            return _p->frameStore;
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
            p.actions["ColorManager"]->setText(_getText(DJV_TEXT("Color Manager")));
            p.actions["ColorManager"]->setTooltip(_getText(DJV_TEXT("Color manager tooltip")));
            p.actions["ColorChannels"]->setText(_getText(DJV_TEXT("Color Channels")));
            p.actions["ColorChannels"]->setTooltip(_getText(DJV_TEXT("Color channels tooltip")));
            p.actions["RedChannel"]->setText(_getText(DJV_TEXT("Red Channel")));
            p.actions["RedChannel"]->setTooltip(_getText(DJV_TEXT("Red channel tooltip")));
            p.actions["GreenChannel"]->setText(_getText(DJV_TEXT("Green Channel")));
            p.actions["GreenChannel"]->setTooltip(_getText(DJV_TEXT("Green channel tooltip")));
            p.actions["BlueChannel"]->setText(_getText(DJV_TEXT("Blue Channel")));
            p.actions["BlueChannel"]->setTooltip(_getText(DJV_TEXT("Blue channel tooltip")));
            p.actions["AlphaChannel"]->setText(_getText(DJV_TEXT("Alpha Channel")));
            p.actions["AlphaChannel"]->setTooltip(_getText(DJV_TEXT("Alpha channel tooltip")));
            p.actions["PremultipliedAlpha"]->setText(_getText(DJV_TEXT("Premultiplied Alpha")));
            p.actions["PremultipliedAlpha"]->setTooltip(_getText(DJV_TEXT("Premultiplied alpha tooltip")));
            p.actions["Transform"]->setText(_getText(DJV_TEXT("Transform")));
            p.actions["Transform"]->setTooltip(_getText(DJV_TEXT("Transform tooltip")));
            p.actions["FrameStoreEnabled"]->setText(_getText(DJV_TEXT("Frame Store")));
            p.actions["FrameStoreEnabled"]->setTooltip(_getText(DJV_TEXT("Frame store tooltip")));
            p.actions["LoadFrameStore"]->setText(_getText(DJV_TEXT("Load Frame Store")));
            p.actions["LoadFrameStore"]->setTooltip(_getText(DJV_TEXT("Load frame store tooltip")));

            p.menu->setText(_getText(DJV_TEXT("Image")));
        }

    } // namespace ViewApp
} // namespace djv

