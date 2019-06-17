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
#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/ImageView.h>
#include <djvViewApp/MDIWidget.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

#include <djvAV/Image.h>
#include <djvAV/Render2D.h>

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
            std::shared_ptr<ImageSettings> settings;
            std::shared_ptr<ValueSubject<bool> > frameStoreEnabled;
            std::shared_ptr<ValueSubject<std::shared_ptr<AV::Image::Image> > > frameStore;
            std::shared_ptr<AV::Image::Image> currentImage;
            std::shared_ptr<MDIWidget> activeWidget;
            AV::Render::ImageOptions imageOptions;
            ImageRotate imageRotate = ImageRotate::First;
            ImageAspectRatio imageAspectRatio = ImageAspectRatio::First;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> channelActionGroup;
            std::shared_ptr<UI::ActionGroup> rotateActionGroup;
            std::shared_ptr<UI::ActionGroup> aspectRatioActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > currentImageObserver;
            std::shared_ptr<ValueObserver<AV::Render::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<ValueObserver<ImageRotate> > imageRotateObserver;
            std::shared_ptr<ValueObserver<ImageAspectRatio> > imageAspectRatioObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<MDIWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void ImageSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewApp::ImageSystem", context);

            DJV_PRIVATE_PTR();

            p.settings = ImageSettings::create(context);
            p.frameStoreEnabled = ValueSubject<bool>::create();
            p.frameStore = ValueSubject<std::shared_ptr<AV::Image::Image> >::create();

            //! \todo Implement me!
            p.actions["ColorManager"] = UI::Action::create();
            p.actions["ColorManager"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorManager"]->setEnabled(false);
            p.actions["ColorChannels"] = UI::Action::create();
            p.actions["ColorChannels"]->setShortcut(GLFW_KEY_C);
            p.actions["RedChannel"] = UI::Action::create();
            p.actions["RedChannel"]->setShortcut(GLFW_KEY_R);
            p.actions["GreenChannel"] = UI::Action::create();
            p.actions["GreenChannel"]->setShortcut(GLFW_KEY_G);
            p.actions["BlueChannel"] = UI::Action::create();
            p.actions["BlueChannel"]->setShortcut(GLFW_KEY_B);
            p.actions["AlphaChannel"] = UI::Action::create();
            p.actions["AlphaChannel"]->setShortcut(GLFW_KEY_A);
            p.channelActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.channelActionGroup->addAction(p.actions["ColorChannels"]);
            p.channelActionGroup->addAction(p.actions["RedChannel"]);
            p.channelActionGroup->addAction(p.actions["GreenChannel"]);
            p.channelActionGroup->addAction(p.actions["BlueChannel"]);
            p.channelActionGroup->addAction(p.actions["AlphaChannel"]);
            //! \todo Implement me!
            p.actions["PremultipliedAlpha"] = UI::Action::create();
            p.actions["PremultipliedAlpha"]->setEnabled(false);
            p.actions["MirrorH"] = UI::Action::create();
            p.actions["MirrorH"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["MirrorH"]->setShortcut(GLFW_KEY_H);
            p.actions["MirrorV"] = UI::Action::create();
            p.actions["MirrorV"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["MirrorV"]->setShortcut(GLFW_KEY_V);
            p.actions["Rotate_0"] = UI::Action::create();
            p.actions["Rotate_90"] = UI::Action::create();
            p.actions["Rotate_180"] = UI::Action::create();
            p.actions["Rotate_270"] = UI::Action::create();
            p.rotateActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.rotateActionGroup->addAction(p.actions["Rotate_0"]);
            p.rotateActionGroup->addAction(p.actions["Rotate_90"]);
            p.rotateActionGroup->addAction(p.actions["Rotate_180"]);
            p.rotateActionGroup->addAction(p.actions["Rotate_270"]);
            p.actions["AspectRatio_Default"] = UI::Action::create();
            //! \todo Implement me!
            p.actions["AspectRatio_Auto"] = UI::Action::create();
            p.actions["AspectRatio_Auto"]->setEnabled(false);
            p.actions["AspectRatio_16_9"] = UI::Action::create();
            p.actions["AspectRatio_1_85"] = UI::Action::create();
            p.actions["AspectRatio_2_35"] = UI::Action::create();
            p.aspectRatioActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.aspectRatioActionGroup->addAction(p.actions["AspectRatio_Default"]);
            p.aspectRatioActionGroup->addAction(p.actions["AspectRatio_Auto"]);
            p.aspectRatioActionGroup->addAction(p.actions["AspectRatio_16_9"]);
            p.aspectRatioActionGroup->addAction(p.actions["AspectRatio_1_85"]);
            p.aspectRatioActionGroup->addAction(p.actions["AspectRatio_2_35"]);
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
            p.menu->addAction(p.actions["MirrorH"]);
            p.menu->addAction(p.actions["MirrorV"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Rotate_0"]);
            p.menu->addAction(p.actions["Rotate_90"]);
            p.menu->addAction(p.actions["Rotate_180"]);
            p.menu->addAction(p.actions["Rotate_270"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["AspectRatio_Default"]);
            p.menu->addAction(p.actions["AspectRatio_Auto"]);
            p.menu->addAction(p.actions["AspectRatio_16_9"]);
            p.menu->addAction(p.actions["AspectRatio_1_85"]);
            p.menu->addAction(p.actions["AspectRatio_2_35"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["FrameStoreEnabled"]);
            p.menu->addAction(p.actions["LoadFrameStore"]);

            _imageUpdate();

            auto weak = std::weak_ptr<ImageSystem>(std::dynamic_pointer_cast<ImageSystem>(shared_from_this()));
            p.channelActionGroup->setRadioCallback(
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->imageOptions.channel = static_cast<AV::Render::ImageChannel>(value);
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getImageView()->setImageOptions(system->_p->imageOptions);
                        }
                    }
                });

            p.rotateActionGroup->setRadioCallback(
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->imageRotate = static_cast<ImageRotate>(value);
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getImageView()->setImageRotate(system->_p->imageRotate);
                        }
                    }
                });

            p.aspectRatioActionGroup->setRadioCallback(
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->imageAspectRatio = static_cast<ImageAspectRatio>(value);
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getImageView()->setImageAspectRatio(system->_p->imageAspectRatio);
                        }
                    }
                });

            p.clickedObservers["MirrorH"] = ValueObserver<bool>::create(
                p.actions["MirrorH"]->observeChecked(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->imageOptions.mirror.x = value;
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getImageView()->setImageOptions(system->_p->imageOptions);
                        }
                    }
                });

            p.clickedObservers["MirrorV"] = ValueObserver<bool>::create(
                p.actions["MirrorV"]->observeChecked(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->imageOptions.mirror.y = value;
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getImageView()->setImageOptions(system->_p->imageOptions);
                        }
                    }
                });

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
                                            system->_imageUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->currentImage.reset();
                                system->_p->currentImageObserver.reset();
                                system->_imageUpdate();
                            }
                        }
                    });
            }

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MDIWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MDIWidget>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->activeWidget = value;
                            if (system->_p->activeWidget)
                            {
                                system->_p->imageOptionsObserver = ValueObserver<AV::Render::ImageOptions>::create(
                                    system->_p->activeWidget->getImageView()->observeImageOptions(),
                                    [weak](const AV::Render::ImageOptions& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->imageOptions = value;
                                            system->_imageUpdate();
                                        }
                                    });
                                system->_p->imageRotateObserver = ValueObserver<ImageRotate>::create(
                                    system->_p->activeWidget->getImageView()->observeImageRotate(),
                                    [weak](ImageRotate value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->imageRotate = value;
                                            system->_imageUpdate();
                                        }
                                    });
                                system->_p->imageAspectRatioObserver = ValueObserver<ImageAspectRatio>::create(
                                    system->_p->activeWidget->getImageView()->observeImageAspectRatio(),
                                    [weak](ImageAspectRatio value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->imageAspectRatio = value;
                                            system->_imageUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->imageOptionsObserver.reset();
                                system->_p->imageRotateObserver.reset();
                                system->_p->imageAspectRatioObserver.reset();
                            }
                            system->_imageUpdate();
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

        void ImageSystem::_imageUpdate()
        {
            DJV_PRIVATE_PTR();
            const bool currentImage = p.currentImage.get();
            p.actions["ColorChannels"]->setEnabled(currentImage);
            p.actions["RedChannel"]->setEnabled(currentImage);
            p.actions["GreenChannel"]->setEnabled(currentImage);
            p.actions["BlueChannel"]->setEnabled(currentImage);
            p.actions["AlphaChannel"]->setEnabled(currentImage);
            p.actions["MirrorH"]->setEnabled(currentImage);
            p.actions["MirrorV"]->setEnabled(currentImage);
            p.actions["Rotate_0"]->setEnabled(currentImage);
            p.actions["Rotate_90"]->setEnabled(currentImage);
            p.actions["Rotate_180"]->setEnabled(currentImage);
            p.actions["Rotate_270"]->setEnabled(currentImage);
            p.actions["AspectRatio_Default"]->setEnabled(currentImage);
            p.actions["AspectRatio_Auto"]->setEnabled(currentImage);
            p.actions["AspectRatio_16_9"]->setEnabled(currentImage);
            p.actions["AspectRatio_1_85"]->setEnabled(currentImage);
            p.actions["AspectRatio_2_35"]->setEnabled(currentImage);

            p.channelActionGroup->setChecked(static_cast<int>(p.imageOptions.channel));
            p.rotateActionGroup->setChecked(static_cast<int>(p.imageRotate));
            p.aspectRatioActionGroup->setChecked(static_cast<int>(p.imageAspectRatio));
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
            p.actions["MirrorH"]->setText(_getText(DJV_TEXT("Mirror Horizontal")));
            p.actions["MirrorH"]->setTooltip(_getText(DJV_TEXT("Mirror horizontal tooltip")));
            p.actions["MirrorV"]->setText(_getText(DJV_TEXT("Mirror Vertical")));
            p.actions["MirrorV"]->setTooltip(_getText(DJV_TEXT("Mirror vertical tooltip")));
            p.actions["Rotate_0"]->setText(_getText(DJV_TEXT("Rotate 0")));
            p.actions["Rotate_0"]->setTooltip(_getText(DJV_TEXT("Rotate 0 tooltip")));
            p.actions["Rotate_90"]->setText(_getText(DJV_TEXT("Rotate 90")));
            p.actions["Rotate_90"]->setTooltip(_getText(DJV_TEXT("Rotate 90 tooltip")));
            p.actions["Rotate_180"]->setText(_getText(DJV_TEXT("Rotate 180")));
            p.actions["Rotate_180"]->setTooltip(_getText(DJV_TEXT("Rotate 180 tooltip")));
            p.actions["Rotate_270"]->setText(_getText(DJV_TEXT("Rotate 270")));
            p.actions["Rotate_270"]->setTooltip(_getText(DJV_TEXT("Rotate 270 tooltip")));
            p.actions["AspectRatio_Default"]->setText(_getText(DJV_TEXT("Default Aspect Ratio")));
            p.actions["AspectRatio_Default"]->setTooltip(_getText(DJV_TEXT("Default aspect ratio tooltip")));
            p.actions["AspectRatio_Auto"]->setText(_getText(DJV_TEXT("Automatic Aspect Ratio")));
            p.actions["AspectRatio_Auto"]->setTooltip(_getText(DJV_TEXT("Automatic aspect ratio tooltip")));
            p.actions["AspectRatio_16_9"]->setText(_getText(DJV_TEXT("16:9")));
            p.actions["AspectRatio_16_9"]->setTooltip(_getText(DJV_TEXT("16:9 aspect ratio tooltip")));
            p.actions["AspectRatio_1_85"]->setText(_getText(DJV_TEXT("1.85")));
            p.actions["AspectRatio_1_85"]->setTooltip(_getText(DJV_TEXT("1.85 aspect ratio tooltip")));
            p.actions["AspectRatio_2_35"]->setText(_getText(DJV_TEXT("2.35")));
            p.actions["AspectRatio_2_35"]->setTooltip(_getText(DJV_TEXT("2.35 aspect ratio tooltip")));
            p.actions["FrameStoreEnabled"]->setText(_getText(DJV_TEXT("Frame Store")));
            p.actions["FrameStoreEnabled"]->setTooltip(_getText(DJV_TEXT("Frame store tooltip")));
            p.actions["LoadFrameStore"]->setText(_getText(DJV_TEXT("Load Frame Store")));
            p.actions["LoadFrameStore"]->setTooltip(_getText(DJV_TEXT("Load frame store tooltip")));

            p.menu->setText(_getText(DJV_TEXT("Image")));
        }

    } // namespace ViewApp
} // namespace djv

