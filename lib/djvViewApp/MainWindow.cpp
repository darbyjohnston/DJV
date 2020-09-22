// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MainWindow.h>

#include <djvViewApp/Application.h>
#include <djvViewApp/FileSettings.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/IToolSystem.h>
#include <djvViewApp/MDIWidget.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/SettingsWidget.h>
#include <djvViewApp/SettingsSystem.h>
#include <djvViewApp/TimelineWidget.h>
#include <djvViewApp/ToolSystem.h>
#include <djvViewApp/ViewSystem.h>
#include <djvViewApp/WindowBackgroundWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/ThermometerWidget.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/Drawer.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/MenuButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Separator.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolButton.h>

#include <djvSystem/FileInfo.h>

#include <djvCore/OS.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MainWindow::Private
        {
            std::vector<std::shared_ptr<Media> > media;
            
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> mediaActionGroup;
            std::shared_ptr<UI::Menu> mediaMenu;
            std::shared_ptr<UI::Button::Menu> mediaButton;
            std::shared_ptr<UI::ToolButton> settingsButton;
            std::shared_ptr<UI::MenuBar> menuBar;
            std::shared_ptr<UI::Layout::Separator> menuBarSeparator;
            std::shared_ptr<UI::MDI::Canvas> canvas;
            std::shared_ptr<TimelineWidget> timelineWidget;
            std::shared_ptr<UI::Layout::Separator> timelineSeparator;
            std::map<std::shared_ptr<Media>, std::shared_ptr<MediaWidget> > mediaWidgets;
            std::shared_ptr<UI::SoloLayout> mediaLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<ValueObserver<bool> > settingsActionObserver;
            std::shared_ptr<ListObserver<std::shared_ptr<Media> > > mediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > openedObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > closedObserver;
            std::shared_ptr<ValueObserver<bool> > presentationObserver;
        };
        
        void MainWindow::_init(const std::shared_ptr<System::Context>& context)
        {
            UI::Window::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MainWindow");

            auto viewSystems = context->getSystemsT<IViewSystem>();
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            for (auto system : viewSystems)
            {
                for (auto action : system->getActions())
                {
                    addAction(action.second);
                }
                auto menu = system->getMenu();
                if (menu.menu)
                {
                    menus[menu.sortKey] = menu.menu;
                }
            }

            p.actions["Escape"] = UI::Action::create();
            p.actions["Escape"]->setShortcut(GLFW_KEY_ESCAPE);
            for (auto i : p.actions)
            {
                addAction(i.second);
            }

            p.mediaActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.mediaMenu = UI::Menu::create(context);
            p.mediaMenu->setMinimumSizeRole(UI::MetricsRole::None);
            addChild(p.mediaMenu);
            p.mediaButton = UI::Button::Menu::create(UI::MenuButtonStyle::Flat, context);
            p.mediaButton->setPopupIcon("djvIconPopupMenu");
            p.mediaButton->setEnabled(false);

            auto viewFrameButton = UI::ToolButton::create(context);
            auto viewCenterButton = UI::ToolButton::create(context);
            auto viewSystem = context->getSystemT<ViewSystem>();
            if (viewSystem)
            {
                viewFrameButton->addAction(viewSystem->getActions()["FrameLock"]);
                viewCenterButton->addAction(viewSystem->getActions()["CenterLock"]);
            }

            std::map<std::string, std::shared_ptr<UI::ToolButton> > toolButtons;
            for (const auto& i : context->getSystemsT<IToolSystem>())
            {
                auto button = UI::ToolButton::create(context);
                auto data = i->getToolAction();
                button->addAction(data.action);
                toolButtons[data.sortKey] = button;
            }

            p.settingsButton = UI::ToolButton::create(context);
            p.settingsButton->setButtonType(UI::ButtonType::Toggle);
            auto toolSystem = context->getSystemT<ToolSystem>();
            if (toolSystem)
            {
                p.settingsButton->addAction(toolSystem->getActions()["Settings"]);
            }
            auto settingsDrawer = UI::Drawer::create(UI::Side::Right, context);

            p.menuBar = UI::MenuBar::create(context);
            for (auto i : menus)
            {
                p.menuBar->addChild(i.second);
            }
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(p.mediaButton);
            p.menuBar->setStretch(p.mediaButton, UI::RowStretch::Expand, UI::Side::Right);
            p.menuBar->addSeparator(UI::Side::Right);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(viewFrameButton);
            hLayout->addChild(viewCenterButton);
            p.menuBar->addChild(hLayout);
            p.menuBar->addSeparator(UI::Side::Right);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            for (const auto& i : toolButtons)
            {
                hLayout->addChild(i.second);
            }
            p.menuBar->addChild(hLayout);
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(p.settingsButton);

            auto backgroundWidget = WindowBackgroundWidget::create(context);

            p.canvas = UI::MDI::Canvas::create(context);
            for (auto system : viewSystems)
            {
                system->setCanvas(p.canvas);
            }

            p.timelineWidget = TimelineWidget::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.menuBar);
            p.menuBarSeparator = UI::Layout::Separator::create(context);
            p.layout->addChild(p.menuBarSeparator);
            auto stackLayout = UI::StackLayout::create(context);
            stackLayout->addChild(backgroundWidget);
            p.mediaLayout = UI::SoloLayout::create(context);
            stackLayout->addChild(p.mediaLayout);
            stackLayout->addChild(p.canvas);
            stackLayout->addChild(settingsDrawer);
            p.layout->addChild(stackLayout);
            p.layout->setStretch(stackLayout, UI::Layout::RowStretch::Expand);
            p.timelineSeparator = UI::Layout::Separator::create(context);
            p.layout->addChild(p.timelineSeparator);
            p.layout->addChild(p.timelineWidget);
            addChild(p.layout);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.mediaActionGroup->setRadioCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (value >= 0 && value < static_cast<int>(widget->_p->media.size()))
                            {
                                if (auto fileSystem = context->getSystemT<FileSystem>())
                                {
                                    fileSystem->setCurrentMedia(widget->_p->media[value]);
                                }
                            }
                            widget->_p->mediaMenu->close();
                        }
                    }
                });

            p.mediaButton->setOpenCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value)
                        {
                            widget->_p->mediaMenu->popup(widget->_p->mediaButton);
                        }
                        else
                        {
                            widget->_p->mediaMenu->close();
                        }
                    }
                });

            p.mediaMenu->setCloseCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->mediaButton->setOpen(false);
                    }
                });

            settingsDrawer->setOpenCallback(
                [contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = SettingsWidget::create(context);
                    }
                    return out;
                });

            p.actions["Escape"]->setClickedCallback(
                [contextWeak]
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto windowSystem = context->getSystemT<WindowSystem>())
                    {
                        if (windowSystem->observePresentation()->get())
                        {
                            windowSystem->setPresentation(false);
                        }
                        else if (windowSystem->observeFullScreen()->get())
                        {
                            windowSystem->setFullScreen(false);
                        }
                    }
                }
            });

            if (toolSystem)
            {
                p.settingsActionObserver = ValueObserver<bool>::create(
                    toolSystem->getActions()["Settings"]->observeChecked(),
                    [settingsDrawer](bool value)
                    {
                        settingsDrawer->setOpen(value);
                    });
            }

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.mediaObserver = ListObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeMedia(),
                    [weak](const std::vector<std::shared_ptr<Media> >& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->media = value;
                            std::vector<std::shared_ptr<UI::Action> > actions;
                            widget->_p->mediaMenu->clearActions();
                            for (const auto& i : widget->_p->media)
                            {
                                auto action = UI::Action::create();
                                action->setText(i->getFileInfo().getFileName());
                                actions.push_back(action);
                                widget->_p->mediaMenu->addAction(action);
                            }
                            widget->_p->mediaActionGroup->setActions(actions);
                            widget->_p->mediaButton->setEnabled(widget->_p->media.size() > 0);
                        }
                    });

                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeCurrentMedia(),
                    [weak, contextWeak](const std::shared_ptr<Media>& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                const auto i = std::find(widget->_p->media.begin(), widget->_p->media.end(), value);
                                if (i != widget->_p->media.end())
                                {
                                    widget->_p->mediaActionGroup->setChecked(i - widget->_p->media.begin());
                                }
                                widget->_p->mediaButton->setText(
                                    value ?
                                    value->getFileInfo().getFileName(Math::Frame::invalid, false) :
                                    "-");
                                widget->_p->mediaButton->setTooltip(std::string(
                                    value ?
                                    value->getFileInfo() :
                                    System::File::Info()));
                                const auto j = widget->_p->mediaWidgets.find(value);
                                if (j != widget->_p->mediaWidgets.end())
                                {
                                    widget->_p->mediaLayout->setCurrentWidget(j->second);
                                    if (auto windowSystem = context->getSystemT<WindowSystem>())
                                    {
                                        windowSystem->setActiveWidget(j->second);
                                    }
                                }
                            }
                        }
                    });

                p.openedObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeOpened(),
                    [weak, contextWeak](const std::shared_ptr<Media>& value)
                    {
                        if (value)
                        {
                            if (auto context = contextWeak.lock())
                            {
                                if (auto widget = weak.lock())
                                {
                                    auto mediaWidget = MediaWidget::create(value, context);
                                    widget->_p->mediaLayout->addChild(mediaWidget);
                                    widget->_p->mediaWidgets[value] = mediaWidget;
                                }
                            }
                        }
                    });

                p.closedObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeClosed(),
                    [weak](const std::shared_ptr<Media>& value)
                    {
                        if (value)
                        {
                            if (auto widget = weak.lock())
                            {
                                const auto i = widget->_p->mediaWidgets.find(value);
                                if (i != widget->_p->mediaWidgets.end())
                                {
                                    widget->_p->mediaLayout->removeChild(i->second);
                                    widget->_p->mediaWidgets.erase(i);
                                }
                            }
                        }
                    });
            }

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.presentationObserver = ValueObserver<bool>::create(
                    windowSystem->observePresentation(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->menuBar->setVisible(!value);
                            widget->_p->menuBarSeparator->setVisible(!value);
                            widget->_p->timelineWidget->setVisible(!value);
                            widget->_p->timelineSeparator->setVisible(!value);
                        }
                    });
            }
        }

        MainWindow::MainWindow() :
            _p(new Private)
        {}

        MainWindow::~MainWindow()
        {}

        std::shared_ptr<MainWindow> MainWindow::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MainWindow>(new MainWindow);
            out->_init(context);
            return out;
        }

        void MainWindow::_dropEvent(System::Event::Drop & event)
        {
            if (auto context = getContext().lock())
            {
                auto fileSystem = context->getSystemT<FileSystem>();
                fileSystem->open(event.getDropPaths());
            }
        }

        void MainWindow::_initEvent(System::Event::Init& event)
        {
            Window::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.mediaButton->setTooltip(_getText(DJV_TEXT("menu_media_popup_tooltip")));
            }
        }

    } // namespace ViewApp
} // namespace djv

