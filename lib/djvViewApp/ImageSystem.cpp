//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvViewApp/ColorSpaceWidget.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/ImageControlsWidget.h>
#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/ImageView.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>

#include <djvAV/Image.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageSystem::Private
        {
            std::shared_ptr<ImageSettings> settings;

            int colorSpaceCurrentTab = 0;
            int colorCurrentTab = 0;
            std::shared_ptr<ValueSubject<bool> > frameStoreEnabled;
            std::shared_ptr<ValueSubject<std::shared_ptr<AV::Image::Image> > > frameStore;
            std::shared_ptr<AV::Image::Image> currentImage;
            std::shared_ptr<MediaWidget> activeWidget;
            AV::Render2D::ImageOptions imageOptions;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> channelActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::weak_ptr<ImageControlsWidget> imageControlsWidget;
            std::weak_ptr<ColorSpaceWidget> colorSpaceWidget;

            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > currentImageObserver;
            std::shared_ptr<ValueObserver<AV::Render2D::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
        };

        void ImageSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::ImageSystem", context);

            DJV_PRIVATE_PTR();

            p.settings = ImageSettings::create(context);
            p.colorSpaceCurrentTab = p.settings->getColorSpaceCurrentTab();
            p.colorCurrentTab = p.settings->getColorCurrentTab();
            _setWidgetGeom(p.settings->getWidgetGeom());

            p.frameStoreEnabled = ValueSubject<bool>::create();
            p.frameStore = ValueSubject<std::shared_ptr<AV::Image::Image> >::create();

            p.actions["ImageControls"] = UI::Action::create();
            p.actions["ImageControls"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ImageControls"]->setShortcut(GLFW_KEY_M, UI::Shortcut::getSystemModifier());
            p.actions["ColorSpace"] = UI::Action::create();
            p.actions["ColorSpace"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorSpace"]->setShortcut(GLFW_KEY_P, UI::Shortcut::getSystemModifier());
            p.actions["RedChannel"] = UI::Action::create();
            p.actions["RedChannel"]->setShortcut(GLFW_KEY_R);
            p.actions["GreenChannel"] = UI::Action::create();
            p.actions["GreenChannel"]->setShortcut(GLFW_KEY_G);
            p.actions["BlueChannel"] = UI::Action::create();
            p.actions["BlueChannel"]->setShortcut(GLFW_KEY_B);
            p.actions["AlphaChannel"] = UI::Action::create();
            p.actions["AlphaChannel"]->setShortcut(GLFW_KEY_A);
            p.channelActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.channelActionGroup->addAction(p.actions["RedChannel"]);
            p.channelActionGroup->addAction(p.actions["GreenChannel"]);
            p.channelActionGroup->addAction(p.actions["BlueChannel"]);
            p.channelActionGroup->addAction(p.actions["AlphaChannel"]);
            p.actions["MirrorH"] = UI::Action::create();
            p.actions["MirrorH"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["MirrorH"]->setShortcut(GLFW_KEY_H);
            p.actions["MirrorV"] = UI::Action::create();
            p.actions["MirrorV"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["MirrorV"]->setShortcut(GLFW_KEY_V);
            p.actions["LoadFrameStore"] = UI::Action::create();
            p.actions["LoadFrameStore"]->setShortcut(GLFW_KEY_F, GLFW_MOD_SHIFT);
            p.actions["FrameStoreEnabled"] = UI::Action::create();
            p.actions["FrameStoreEnabled"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FrameStoreEnabled"]->setShortcut(GLFW_KEY_F);
            p.actions["FrameStoreEnabled"]->setEnabled(false);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["ImageControls"]);
            p.menu->addAction(p.actions["ColorSpace"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["RedChannel"]);
            p.menu->addAction(p.actions["GreenChannel"]);
            p.menu->addAction(p.actions["BlueChannel"]);
            p.menu->addAction(p.actions["AlphaChannel"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["MirrorH"]);
            p.menu->addAction(p.actions["MirrorV"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["LoadFrameStore"]);
            p.menu->addAction(p.actions["FrameStoreEnabled"]);

            _actionsUpdate();

            auto weak = std::weak_ptr<ImageSystem>(std::dynamic_pointer_cast<ImageSystem>(shared_from_this()));
            p.channelActionGroup->setExclusiveCallback(
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->imageOptions.channel = static_cast<AV::Render2D::ImageChannel>(value + 1);
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getImageView()->setImageOptions(system->_p->imageOptions);
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.actionObservers["ImageControls"] = ValueObserver<bool>::create(
                p.actions["ImageControls"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                auto widget = ImageControlsWidget::create(context);
                                widget->setCurrentTab(system->_p->colorCurrentTab);
                                system->_p->imageControlsWidget = widget;
                                system->_openWidget("ImageControls", widget);
                            }
                            else
                            {
                                system->_closeWidget("ImageControls");
                            }
                        }
                    }
                });

            p.actionObservers["ColorSpace"] = ValueObserver<bool>::create(
                p.actions["ColorSpace"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                auto colorSpaceWidget = ColorSpaceWidget::create(context);
                                colorSpaceWidget->setCurrentTab(system->_p->colorSpaceCurrentTab);
                                system->_p->colorSpaceWidget = colorSpaceWidget;
                                system->_openWidget("ColorSpace", colorSpaceWidget);
                            }
                            else
                            {
                                system->_closeWidget("ColorSpace");
                            }
                        }
                    }
                });

            p.actionObservers["MirrorH"] = ValueObserver<bool>::create(
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

            p.actionObservers["MirrorV"] = ValueObserver<bool>::create(
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

            p.actionObservers["LoadFrameStore"] = ValueObserver<bool>::create(
                p.actions["LoadFrameStore"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->loadFrameStore();
                        }
                    }
                });

            p.actionObservers["FrameStoreEnabled"] = ValueObserver<bool>::create(
                p.actions["FrameStoreEnabled"]->observeChecked(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setFrameStoreEnabled(value);
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
                                            system->_actionsUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->currentImage.reset();
                                system->_p->currentImageObserver.reset();
                                system->_actionsUpdate();
                            }
                        }
                    });
            }

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->activeWidget = value;
                            if (system->_p->activeWidget)
                            {
                                system->_p->imageOptionsObserver = ValueObserver<AV::Render2D::ImageOptions>::create(
                                    system->_p->activeWidget->getImageView()->observeImageOptions(),
                                    [weak](const AV::Render2D::ImageOptions& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->imageOptions = value;
                                            system->_actionsUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->imageOptionsObserver.reset();
                            }
                            system->_actionsUpdate();
                        }
                    });
            }
        }

        ImageSystem::ImageSystem() :
            _p(new Private)
        {}

        ImageSystem::~ImageSystem()
        {
            DJV_PRIVATE_PTR();
            _closeWidget("ImageControls");
            _closeWidget("ColorSpace");
            p.settings->setColorSpaceCurrentTab(p.colorSpaceCurrentTab);
            p.settings->setColorCurrentTab(p.colorCurrentTab);
            p.settings->setWidgetGeom(_getWidgetGeom());
        }

        std::shared_ptr<ImageSystem> ImageSystem::create(const std::shared_ptr<Core::Context>& context)
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

        void ImageSystem::loadFrameStore()
        {
            DJV_PRIVATE_PTR();
            p.actions["FrameStoreEnabled"]->setEnabled(p.currentImage ? true : false);
            p.frameStore->setIfChanged(p.currentImage);
        }

        void ImageSystem::setFrameStoreEnabled(bool value)
        {
            DJV_PRIVATE_PTR();
            p.frameStoreEnabled->setIfChanged(value);
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ImageSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData ImageSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "D"
            };
        }

        void ImageSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if ("ImageControls" == value)
            {
                if (auto imageControlsWidget = p.imageControlsWidget.lock())
                {
                    p.colorCurrentTab = imageControlsWidget->getCurrentTab();
                }
                p.imageControlsWidget.reset();
            }
            else if ("ColorSpace" == value)
            {
                if (auto colorSpaceWidget = p.colorSpaceWidget.lock())
                {
                    p.colorSpaceCurrentTab = colorSpaceWidget->getCurrentTab();
                }
                p.colorSpaceWidget.reset();
            }
            const auto i = p.actions.find(value);
            if (i != p.actions.end())
            {
                i->second->setChecked(false);
            }
            IViewSystem::_closeWidget(value);
        }

        void ImageSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["ImageControls"]->setText(_getText(DJV_TEXT("Image Controls")));
                p.actions["ImageControls"]->setTooltip(_getText(DJV_TEXT("Image controls widget tooltip")));
                p.actions["ColorSpace"]->setText(_getText(DJV_TEXT("Color Space")));
                p.actions["ColorSpace"]->setTooltip(_getText(DJV_TEXT("Color space widget tooltip")));
                p.actions["RedChannel"]->setText(_getText(DJV_TEXT("Red Channel")));
                p.actions["RedChannel"]->setTooltip(_getText(DJV_TEXT("Red channel tooltip")));
                p.actions["GreenChannel"]->setText(_getText(DJV_TEXT("Green Channel")));
                p.actions["GreenChannel"]->setTooltip(_getText(DJV_TEXT("Green channel tooltip")));
                p.actions["BlueChannel"]->setText(_getText(DJV_TEXT("Blue Channel")));
                p.actions["BlueChannel"]->setTooltip(_getText(DJV_TEXT("Blue channel tooltip")));
                p.actions["AlphaChannel"]->setText(_getText(DJV_TEXT("Alpha Channel")));
                p.actions["AlphaChannel"]->setTooltip(_getText(DJV_TEXT("Alpha channel tooltip")));
                p.actions["MirrorH"]->setText(_getText(DJV_TEXT("Mirror Horizontal")));
                p.actions["MirrorH"]->setTooltip(_getText(DJV_TEXT("Mirror horizontal tooltip")));
                p.actions["MirrorV"]->setText(_getText(DJV_TEXT("Mirror Vertical")));
                p.actions["MirrorV"]->setTooltip(_getText(DJV_TEXT("Mirror vertical tooltip")));
                p.actions["LoadFrameStore"]->setText(_getText(DJV_TEXT("Load Frame Store")));
                p.actions["LoadFrameStore"]->setTooltip(_getText(DJV_TEXT("Load frame store tooltip")));
                p.actions["FrameStoreEnabled"]->setText(_getText(DJV_TEXT("Frame Store")));
                p.actions["FrameStoreEnabled"]->setTooltip(_getText(DJV_TEXT("Frame store tooltip")));

                p.menu->setText(_getText(DJV_TEXT("Image")));
            }
        }

        void ImageSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            const bool activeWidget = p.activeWidget.get();
            p.actions["RedChannel"]->setEnabled(activeWidget);
            p.actions["GreenChannel"]->setEnabled(activeWidget);
            p.actions["BlueChannel"]->setEnabled(activeWidget);
            p.actions["AlphaChannel"]->setEnabled(activeWidget);
            p.actions["MirrorH"]->setEnabled(activeWidget);
            p.actions["MirrorV"]->setEnabled(activeWidget);

            p.channelActionGroup->setChecked(static_cast<int>(p.imageOptions.channel) - 1);
        }

    } // namespace ViewApp
} // namespace djv

