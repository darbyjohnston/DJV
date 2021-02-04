// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ImageSystem.h>

#include <djvViewApp/ColorSpaceWidget.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/ImageControlsWidget.h>
#include <djvViewApp/ImageData.h>
#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ToolTitleBar.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ShortcutDataFunc.h>

#include <djvRender2D/Render.h>

#include <djvImage/Data.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

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
            ImageData data;
            std::shared_ptr<Observer::ValueSubject<bool> > frameStoreEnabled;
            std::shared_ptr<Observer::ValueSubject<std::shared_ptr<Image::Data> > > frameStoreImage;
            std::shared_ptr<Image::Data> currentImage;
            std::shared_ptr<MediaWidget> activeWidget;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> channelActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<ImageControlsWidget> imageControlsWidget;
            std::shared_ptr<ColorSpaceWidget> colorSpaceWidget;

            std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Image::Data> > > currentImageObserver;
            std::shared_ptr<Observer::Value<ImageData> > dataObserver;
            std::shared_ptr<Observer::Map<std::string, std::vector<UI::ShortcutData> > > shortcutsObserver;
        };

        void ImageSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::ImageSystem", context);

            DJV_PRIVATE_PTR();

            p.settings = ImageSettings::create(context);
            p.controlsBellowsState = p.settings->getControlsBellowsState();
            p.colorSpaceBellowsState = p.settings->getColorSpaceBellowsState();

            p.frameStoreEnabled = Observer::ValueSubject<bool>::create();
            p.frameStoreImage = Observer::ValueSubject<std::shared_ptr<Image::Data> >::create();

            p.actions["ImageControls"] = UI::Action::create();
            p.actions["ImageControls"]->setIcon("djvIconImageControls");
            p.actions["ColorSpace"] = UI::Action::create();
            p.actions["ColorSpace"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorSpace"]->setIcon("djvIconColor");
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
            p.actions["ColorEnabled"] = UI::Action::create();
            p.actions["ColorEnabled"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorEnabled"]->setIcon("djvIconHidden");
            p.actions["ColorEnabled"]->setCheckedIcon("djvIconVisible");
            p.actions["LevelsEnabled"] = UI::Action::create();
            p.actions["LevelsEnabled"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["LevelsEnabled"]->setIcon("djvIconHidden");
            p.actions["LevelsEnabled"]->setCheckedIcon("djvIconVisible");
            p.actions["ExposureEnabled"] = UI::Action::create();
            p.actions["ExposureEnabled"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ExposureEnabled"]->setIcon("djvIconHidden");
            p.actions["ExposureEnabled"]->setCheckedIcon("djvIconVisible");
            p.actions["SoftClipEnabled"] = UI::Action::create();
            p.actions["SoftClipEnabled"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["SoftClipEnabled"]->setIcon("djvIconHidden");
            p.actions["SoftClipEnabled"]->setCheckedIcon("djvIconVisible");
            p.actions["FrameStoreEnabled"] = UI::Action::create();
            p.actions["FrameStoreEnabled"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["FrameStoreEnabled"]->setIcon("djvIconHidden");
            p.actions["FrameStoreEnabled"]->setCheckedIcon("djvIconVisible");
            p.actions["LoadFrameStore"] = UI::Action::create();
            p.actions["ClearFrameStore"] = UI::Action::create();

            _addShortcut(DJV_TEXT("shortcut_image_controls"), GLFW_KEY_B, UI::getSystemModifier());
            _addShortcut(DJV_TEXT("shortcut_image_color_space"), GLFW_KEY_P, UI::getSystemModifier());
            _addShortcut(DJV_TEXT("shortcut_image_red_channel"), GLFW_KEY_R);
            _addShortcut(DJV_TEXT("shortcut_image_green_channel"), GLFW_KEY_G);
            _addShortcut(DJV_TEXT("shortcut_image_blue_channel"), GLFW_KEY_B);
            _addShortcut(DJV_TEXT("shortcut_image_alpha_channel"), GLFW_KEY_A);
            _addShortcut(DJV_TEXT("shortcut_image_mirror_h"), GLFW_KEY_H);
            _addShortcut(DJV_TEXT("shortcut_image_mirror_v"), GLFW_KEY_V);
            _addShortcut(DJV_TEXT("shortcut_image_frame_store_enabled"), GLFW_KEY_F);
            _addShortcut(DJV_TEXT("shortcut_image_load_frame_store"), GLFW_KEY_F, GLFW_MOD_SHIFT);
            _addShortcut(DJV_TEXT("shortcut_image_clear_frame_store"), 0);

            p.menu = UI::Menu::create(context);
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
            p.actions["MirrorH"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        auto data = system->_p->data;
                        data.mirror.x = value;
                        system->_p->settings->setData(data);
                    }
                });
            p.actions["MirrorV"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        auto data = system->_p->data;
                        data.mirror.y = value;
                        system->_p->settings->setData(data);
                    }
                });

            p.actions["ColorEnabled"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        auto data = system->_p->data;
                        data.colorEnabled = value;
                        system->_p->settings->setData(data);
                    }
                });
            p.actions["LevelsEnabled"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        auto data = system->_p->data;
                        data.levelsEnabled = value;
                        system->_p->settings->setData(data);
                    }
                });
            p.actions["ExposureEnabled"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        auto data = system->_p->data;
                        data.exposureEnabled = value;
                        system->_p->settings->setData(data);
                    }
                });
            p.actions["SoftClipEnabled"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        auto data = system->_p->data;
                        data.softClipEnabled = value;
                        system->_p->settings->setData(data);
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

            p.channelActionGroup->setExclusiveCallback(
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        auto data = system->_p->data;
                        data.channelsDisplay = static_cast<Render2D::ImageChannelsDisplay>(value + 1);
                        system->_p->settings->setData(data);
                    }
                });

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = Observer::Value<std::shared_ptr<Media> >::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                system->_p->currentImageObserver = Observer::Value<std::shared_ptr<Image::Data> >::create(
                                    value->observeCurrentImage(),
                                    [weak](const std::shared_ptr<Image::Data>& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->currentImage = value;
                                            system->_p->actions["FrameStoreEnabled"]->setEnabled(value.get());
                                            system->_p->actions["LoadFrameStore"]->setEnabled(value.get());
                                            system->_actionsUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->currentImage.reset();
                                system->_p->actions["FrameStoreEnabled"]->setEnabled(false);
                                system->_p->actions["LoadFrameStore"]->setEnabled(false);
                                system->_p->currentImageObserver.reset();
                                system->_actionsUpdate();
                            }
                        }
                    });
            }

            p.dataObserver = Observer::Value<ImageData>::create(
                p.settings->observeData(),
                [weak](const ImageData& value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->data = value;
                        system->_actionsUpdate();
                    }
                });

            _logInitTime();
        }

        ImageSystem::ImageSystem() :
            _p(new Private)
        {}

        ImageSystem::~ImageSystem()
        {
            DJV_PRIVATE_PTR();
            p.settings->setControlsBellowsState(p.controlsBellowsState);
            p.settings->setColorSpaceBellowsState(p.colorSpaceBellowsState);
        }

        std::shared_ptr<ImageSystem> ImageSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<ImageSystem>();
            if (!out)
            {
                out = std::shared_ptr<ImageSystem>(new ImageSystem);
                out->_init(context);
            }
            return out;
        }

        std::shared_ptr<Core::Observer::IValueSubject<bool> > ImageSystem::observeFrameStoreEnabled() const
        {
            return _p->frameStoreEnabled;
        }

        std::shared_ptr<Observer::IValueSubject<std::shared_ptr<Image::Data> > > ImageSystem::observeFrameStoreImage() const
        {
            return _p->frameStoreImage;
        }

        void ImageSystem::setFrameStoreEnabled(bool value)
        {
            DJV_PRIVATE_PTR();
            if (p.frameStoreEnabled->setIfChanged(value))
            {
                _actionsUpdate();
            }
        }

        void ImageSystem::loadFrameStore()
        {
            DJV_PRIVATE_PTR();
            if (p.frameStoreImage->setIfChanged(p.currentImage))
            {
                p.actions["ClearFrameStore"]->setEnabled(p.currentImage.get());
                _actionsUpdate();
            }
        }

        void ImageSystem::clearFrameStore()
        {
            DJV_PRIVATE_PTR();
            if (p.frameStoreImage->setIfChanged(nullptr))
            {
                p.actions["ClearFrameStore"]->setEnabled(false);
                _actionsUpdate();
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ImageSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData ImageSystem::getMenuData() const
        {
            return
            {
                { _p->menu },
                5
            };
        }

        ActionData ImageSystem::getToolActionData() const
        {
            return
            {
                {
                    _p->actions["ImageControls"],
                    _p->actions["ColorSpace"]
                },
                5
            };
        }

        ToolWidgetData ImageSystem::createToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            ToolWidgetData out;
            if (auto context = getContext().lock())
            {
                if (value == p.actions["ImageControls"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("image_controls_title"), context);

                    auto widget = ImageControlsWidget::create(context);
                    widget->setBellowsState(p.controlsBellowsState);
                    p.imageControlsWidget = widget;

                    out.titleBar = titleBar;
                    out.widget = widget;
                }
                else if (value == p.actions["ColorSpace"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("widget_color_space"), context);

                    auto widget = ColorSpaceWidget::create(context);
                    widget->setBellowsState(p.colorSpaceBellowsState);
                    p.colorSpaceWidget = widget;

                    out.titleBar = titleBar;
                    out.widget = widget;
                }
            }
            return out;
        }

        void ImageSystem::deleteToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.actions["ImageControls"])
            {
                if (p.imageControlsWidget)
                {
                    p.controlsBellowsState = p.imageControlsWidget->getBellowsState();
                    p.imageControlsWidget.reset();
                }
            }
            else if (value == p.actions["ColorSpace"])
            {
                if (p.colorSpaceWidget)
                {
                    p.colorSpaceBellowsState = p.colorSpaceWidget->getBellowsState();
                    p.colorSpaceWidget.reset();
                }
            }
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
                p.actions["RedChannel"]->setTextBrief(_getText(DJV_TEXT("button_image_red_channel")));
                p.actions["RedChannel"]->setTooltip(_getText(DJV_TEXT("menu_image_red_channel_tooltip")));
                p.actions["GreenChannel"]->setText(_getText(DJV_TEXT("menu_image_green_channel")));
                p.actions["GreenChannel"]->setTextBrief(_getText(DJV_TEXT("button_image_green_channel")));
                p.actions["GreenChannel"]->setTooltip(_getText(DJV_TEXT("menu_image_green_channel_tooltip")));
                p.actions["BlueChannel"]->setText(_getText(DJV_TEXT("menu_image_blue_channel")));
                p.actions["BlueChannel"]->setTextBrief(_getText(DJV_TEXT("button_image_blue_channel")));
                p.actions["BlueChannel"]->setTooltip(_getText(DJV_TEXT("menu_image_blue_channel_tooltip")));
                p.actions["AlphaChannel"]->setText(_getText(DJV_TEXT("menu_image_alpha_channel")));
                p.actions["AlphaChannel"]->setTextBrief(_getText(DJV_TEXT("button_image_alpha_channel")));
                p.actions["AlphaChannel"]->setTooltip(_getText(DJV_TEXT("menu_image_alpha_channel_tooltip")));
                p.actions["MirrorH"]->setText(_getText(DJV_TEXT("menu_image_mirror_horizontal")));
                p.actions["MirrorH"]->setTextBrief(_getText(DJV_TEXT("button_image_mirror_horizontal")));
                p.actions["MirrorH"]->setTooltip(_getText(DJV_TEXT("menu_image_mirror_horizontal_tooltip")));
                p.actions["MirrorV"]->setText(_getText(DJV_TEXT("menu_image_mirror_vertical")));
                p.actions["MirrorV"]->setTextBrief(_getText(DJV_TEXT("button_image_mirror_vertical")));
                p.actions["MirrorV"]->setTooltip(_getText(DJV_TEXT("menu_image_mirror_vertical_tooltip")));
                p.actions["ColorEnabled"]->setTooltip(_getText(DJV_TEXT("image_controls_color_enabled_tooltip")));
                p.actions["LevelsEnabled"]->setTooltip(_getText(DJV_TEXT("image_controls_levels_enabled_tooltip")));
                p.actions["ExposureEnabled"]->setTooltip(_getText(DJV_TEXT("image_controls_exposure_enabled_tooltip")));
                p.actions["SoftClipEnabled"]->setTooltip(_getText(DJV_TEXT("image_controls_soft_clip_enabled_tooltip")));
                p.actions["FrameStoreEnabled"]->setText(_getText(DJV_TEXT("menu_image_frame_store")));
                p.actions["FrameStoreEnabled"]->setTooltip(_getText(DJV_TEXT("menu_image_frame_store_tooltip")));
                p.actions["LoadFrameStore"]->setText(_getText(DJV_TEXT("menu_image_load_frame_store")));
                p.actions["LoadFrameStore"]->setTextBrief(_getText(DJV_TEXT("button_image_load_frame_store")));
                p.actions["LoadFrameStore"]->setTooltip(_getText(DJV_TEXT("menu_image_load_frame_store_tooltip")));
                p.actions["ClearFrameStore"]->setText(_getText(DJV_TEXT("menu_image_clear_frame_store")));
                p.actions["ClearFrameStore"]->setTextBrief(_getText(DJV_TEXT("button_image_clear_frame_store")));
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
                p.actions["ClearFrameStore"]->setShortcuts(_getShortcuts("shortcut_image_clear_frame_store"));
            }
        }

        void ImageSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["MirrorH"]->setChecked(p.data.mirror.x);
            p.actions["MirrorV"]->setChecked(p.data.mirror.y);

            p.channelActionGroup->setChecked(static_cast<int>(p.data.channelsDisplay) - 1);

            p.actions["ColorEnabled"]->setChecked(p.data.colorEnabled);
            p.actions["LevelsEnabled"]->setChecked(p.data.levelsEnabled);
            p.actions["ExposureEnabled"]->setChecked(p.data.exposureEnabled);
            p.actions["SoftClipEnabled"]->setChecked(p.data.softClipEnabled);

            p.actions["FrameStoreEnabled"]->setChecked(p.frameStoreEnabled->get());
        }

    } // namespace ViewApp
} // namespace djv

