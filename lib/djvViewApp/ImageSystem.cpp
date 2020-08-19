// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ImageSystem.h>

#include <djvViewApp/ColorSpaceWidget.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/ImageControlsWidget.h>
#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ShortcutData.h>

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

            std::map<std::string, bool> controlsBellowsState;
            std::map<std::string, bool> colorSpaceBellowsState;
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

            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > currentImageObserver;
            std::shared_ptr<ValueObserver<AV::Render2D::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<MapObserver<std::string, std::vector<UI::ShortcutData> > > shortcutsObserver;
        };

        void ImageSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::ImageSystem", context);

            DJV_PRIVATE_PTR();

            p.settings = ImageSettings::create(context);
            p.controlsBellowsState = p.settings->getControlsBellowsState();
            p.colorSpaceBellowsState = p.settings->getColorSpaceBellowsState();
            _setWidgetGeom(p.settings->getWidgetGeom());

            p.frameStoreEnabled = ValueSubject<bool>::create();
            p.frameStore = ValueSubject<std::shared_ptr<AV::Image::Image> >::create();

            p.actions["ImageControls"] = UI::Action::create();
            p.actions["ImageControls"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorSpace"] = UI::Action::create();
            p.actions["ColorSpace"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["RedChannel"] = UI::Action::create();
            p.actions["GreenChannel"] = UI::Action::create();
            p.actions["BlueChannel"] = UI::Action::create();
            p.actions["AlphaChannel"] = UI::Action::create();
            p.channelActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.channelActionGroup->setActions({
                p.actions["RedChannel"],
                p.actions["GreenChannel"],
                p.actions["BlueChannel"],
                p.actions["AlphaChannel"] });
            p.actions["MirrorH"] = UI::Action::create();
            p.actions["MirrorH"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["MirrorV"] = UI::Action::create();
            p.actions["MirrorV"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FrameStoreEnabled"] = UI::Action::create();
            p.actions["FrameStoreEnabled"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FrameStoreEnabled"]->setEnabled(false);
            p.actions["LoadFrameStore"] = UI::Action::create();
            p.actions["ClearFrameStore"] = UI::Action::create();

            _addShortcut("shortcut_image_controls", GLFW_KEY_B, UI::ShortcutData::getSystemModifier());
            _addShortcut("shortcut_image_color_space", GLFW_KEY_P, UI::ShortcutData::getSystemModifier());
            _addShortcut("shortcut_image_red_channel", GLFW_KEY_R);
            _addShortcut("shortcut_image_green_channel", GLFW_KEY_G);
            _addShortcut("shortcut_image_blue_channel", GLFW_KEY_B);
            _addShortcut("shortcut_image_alpha_channel", GLFW_KEY_A);
            _addShortcut("shortcut_image_mirror_h", GLFW_KEY_H);
            _addShortcut("shortcut_image_mirror_v", GLFW_KEY_V);
            _addShortcut("shortcut_image_frame_store_enabled", GLFW_KEY_F);
            _addShortcut("shortcut_image_load_frame_store", GLFW_KEY_F, GLFW_MOD_SHIFT);

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
            p.menu->addAction(p.actions["FrameStoreEnabled"]);
            p.menu->addAction(p.actions["LoadFrameStore"]);
            p.menu->addAction(p.actions["ClearFrameStore"]);

            _actionsUpdate();
            _textUpdate();
            _shortcutsUpdate();

            auto weak = std::weak_ptr<ImageSystem>(std::dynamic_pointer_cast<ImageSystem>(shared_from_this()));
            p.channelActionGroup->setExclusiveCallback(
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->imageOptions.channelDisplay = static_cast<AV::Render2D::ImageChannelDisplay>(value + 1);
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getViewWidget()->setImageOptions(system->_p->imageOptions);
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.actions["ImageControls"]->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                auto widget = ImageControlsWidget::create(context);
                                widget->setBellowsState(system->_p->controlsBellowsState);
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

            p.actions["ColorSpace"]->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                auto widget = ColorSpaceWidget::create(context);
                                widget->setBellowsState(system->_p->colorSpaceBellowsState);
                                system->_p->colorSpaceWidget = widget;
                                system->_openWidget("ColorSpace", widget);
                            }
                            else
                            {
                                system->_closeWidget("ColorSpace");
                            }
                        }
                    }
                });

            p.actions["MirrorH"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->imageOptions.mirror.x = value;
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getViewWidget()->setImageOptions(system->_p->imageOptions);
                        }
                    }
                });

            p.actions["MirrorV"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->imageOptions.mirror.y = value;
                        if (system->_p->activeWidget)
                        {
                            system->_p->activeWidget->getViewWidget()->setImageOptions(system->_p->imageOptions);
                        }
                    }
                });

            p.actions["FrameStoreEnabled"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->setFrameStoreEnabled(value);
                    }
                });

            p.actions["LoadFrameStore"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->loadFrameStore();
                    }
                });

            p.actions["ClearFrameStore"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        system->clearFrameStore();
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
                                    system->_p->activeWidget->getViewWidget()->observeImageOptions(),
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
            _closeWidget("ColorSpace");
            _closeWidget("ImageControls");
            p.settings->setControlsBellowsState(p.controlsBellowsState);
            p.settings->setColorSpaceBellowsState(p.colorSpaceBellowsState);
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

        void ImageSystem::setFrameStoreEnabled(bool value)
        {
            DJV_PRIVATE_PTR();
            p.frameStoreEnabled->setIfChanged(value);
        }

        void ImageSystem::loadFrameStore()
        {
            DJV_PRIVATE_PTR();
            p.actions["FrameStoreEnabled"]->setEnabled(p.currentImage ? true : false);
            p.frameStore->setIfChanged(p.currentImage);
        }

        void ImageSystem::clearFrameStore()
        {
            DJV_PRIVATE_PTR();
            p.actions["FrameStoreEnabled"]->setEnabled(p.currentImage ? true : false);
            p.frameStore->setIfChanged(nullptr);
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
                "E"
            };
        }

        void ImageSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if ("ImageControls" == value)
            {
                if (auto imageControlsWidget = p.imageControlsWidget.lock())
                {
                    p.controlsBellowsState = imageControlsWidget->getBellowsState();
                }
                p.imageControlsWidget.reset();
            }
            else if ("ColorSpace" == value)
            {
                if (auto colorSpaceWidget = p.colorSpaceWidget.lock())
                {
                    p.colorSpaceBellowsState = colorSpaceWidget->getBellowsState();
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
                p.actions["ImageControls"]->setText(_getText(DJV_TEXT("menu_image_controls_widget")));
                p.actions["ImageControls"]->setTooltip(_getText(DJV_TEXT("menu_image_controls_widget_tooltip")));
                p.actions["ColorSpace"]->setText(_getText(DJV_TEXT("menu_image_color_space")));
                p.actions["ColorSpace"]->setTooltip(_getText(DJV_TEXT("menu_image_color_space_widget_tooltip")));
                p.actions["RedChannel"]->setText(_getText(DJV_TEXT("menu_image_red_channel")));
                p.actions["RedChannel"]->setTooltip(_getText(DJV_TEXT("menu_image_red_channel_tooltip")));
                p.actions["GreenChannel"]->setText(_getText(DJV_TEXT("menu_image_green_channel")));
                p.actions["GreenChannel"]->setTooltip(_getText(DJV_TEXT("menu_image_green_channel_tooltip")));
                p.actions["BlueChannel"]->setText(_getText(DJV_TEXT("menu_image_blue_channel")));
                p.actions["BlueChannel"]->setTooltip(_getText(DJV_TEXT("menu_image_blue_channel_tooltip")));
                p.actions["AlphaChannel"]->setText(_getText(DJV_TEXT("menu_image_alpha_channel")));
                p.actions["AlphaChannel"]->setTooltip(_getText(DJV_TEXT("menu_image_alpha_channel_tooltip")));
                p.actions["MirrorH"]->setText(_getText(DJV_TEXT("menu_image_mirror_horizontal")));
                p.actions["MirrorH"]->setTooltip(_getText(DJV_TEXT("menu_image_mirror_horizontal_tooltip")));
                p.actions["MirrorV"]->setText(_getText(DJV_TEXT("menu_image_mirror_vertical")));
                p.actions["MirrorV"]->setTooltip(_getText(DJV_TEXT("menu_image_mirror_vertical_tooltip")));
                p.actions["FrameStoreEnabled"]->setText(_getText(DJV_TEXT("menu_image_frame_store")));
                p.actions["FrameStoreEnabled"]->setTooltip(_getText(DJV_TEXT("menu_image_frame_store_tooltip")));
                p.actions["LoadFrameStore"]->setText(_getText(DJV_TEXT("menu_image_load_frame_store")));
                p.actions["LoadFrameStore"]->setTooltip(_getText(DJV_TEXT("menu_image_load_frame_store_tooltip")));
                p.actions["ClearFrameStore"]->setText(_getText(DJV_TEXT("menu_image_clear_frame_store")));
                p.actions["ClearFrameStore"]->setTooltip(_getText(DJV_TEXT("menu_image_clear_frame_store_tooltip")));

                p.menu->setText(_getText(DJV_TEXT("menu_image")));
            }
        }

        void ImageSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["ImageControls"]->setShortcuts(_getShortcuts("shortcut_image_controls"));
                p.actions["ColorSpace"]->setShortcuts(_getShortcuts("shortcut_image_color_space"));
                p.actions["RedChannel"]->setShortcuts(_getShortcuts("shortcut_image_red_channel"));
                p.actions["GreenChannel"]->setShortcuts(_getShortcuts("shortcut_image_green_channel"));
                p.actions["BlueChannel"]->setShortcuts(_getShortcuts("shortcut_image_blue_channel"));
                p.actions["AlphaChannel"]->setShortcuts(_getShortcuts("shortcut_image_alpha_channel"));
                p.actions["MirrorH"]->setShortcuts(_getShortcuts("shortcut_image_mirror_h"));
                p.actions["MirrorV"]->setShortcuts(_getShortcuts("shortcut_image_mirror_v"));
                p.actions["FrameStoreEnabled"]->setShortcuts(_getShortcuts("shortcut_image_frame_store_enabled"));
                p.actions["LoadFrameStore"]->setShortcuts(_getShortcuts("shortcut_image_load_frame_store"));
            }
        }

        void ImageSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            const bool activeWidget = p.activeWidget.get();
#if defined(DJV_OPENGL_ES2)
            p.actions["ColorSpace"]->setEnabled(false);
#endif // DJV_OPENGL_ES2
            p.actions["RedChannel"]->setEnabled(activeWidget);
            p.actions["GreenChannel"]->setEnabled(activeWidget);
            p.actions["BlueChannel"]->setEnabled(activeWidget);
            p.actions["AlphaChannel"]->setEnabled(activeWidget);
            p.actions["MirrorH"]->setEnabled(activeWidget);
            p.actions["MirrorV"]->setEnabled(activeWidget);

            p.channelActionGroup->setChecked(static_cast<int>(p.imageOptions.channelDisplay) - 1);
        }

    } // namespace ViewApp
} // namespace djv

