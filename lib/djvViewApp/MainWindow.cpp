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
        namespace
        {
            //! \todo Should this be configurable?
            const size_t elide = 40;

        } // namespace

        struct MainWindow::Private
        {
            std::shared_ptr<Media> currentMedia;
            
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ToolButton> activeButton;
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

            p.activeButton = UI::ToolButton::create(context);
            p.activeButton->setElide(elide);

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
            p.menuBar->addChild(p.activeButton);
            p.menuBar->setStretch(p.activeButton, UI::RowStretch::Expand, UI::Side::Right);
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

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.activeButton->setClickedCallback(
                [contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto fileSystem = context->getSystemT<FileSystem>())
                        {
                            fileSystem->getActions()["Active"]->doClick();
                        }
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

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeCurrentMedia(),
                    [weak, contextWeak](const std::shared_ptr<Media>& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->currentMedia = value;
                                widget->_p->activeButton->setEnabled(value.get());
                                const auto i = widget->_p->mediaWidgets.find(value);
                                if (i != widget->_p->mediaWidgets.end())
                                {
                                    if (auto windowSystem = context->getSystemT<WindowSystem>())
                                    {
                                        windowSystem->setActiveWidget(i->second);
                                    }
                                    widget->_p->mediaLayout->setCurrentWidget(i->second);
                                }
                                widget->_textUpdate();
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
                _textUpdate();
            }
        }

        void MainWindow::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.activeButton->setText(
                p.currentMedia ?
                p.currentMedia->getFileInfo().getFileName(Math::Frame::invalid, false) :
                "-");
            p.activeButton->setTooltip(std::string(
                p.currentMedia ?
                p.currentMedia->getFileInfo() :
                _getText(DJV_TEXT("menu_file_active_tooltip"))));
        }

    } // namespace ViewApp
} // namespace djv

