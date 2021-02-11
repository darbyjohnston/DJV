// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MainWindow.h>

#include <djvViewApp/Application.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/TimelineWidget.h>
#include <djvViewApp/ToolSettings.h>
#include <djvViewApp/ToolSystem.h>
#include <djvViewApp/ViewSystem.h>
#include <djvViewApp/WindowBackgroundWidget.h>
#include <djvViewApp/WindowSettings.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/ThermometerWidget.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/DrawerLayout.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Separator.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/TabBar.h>
#include <djvUI/TabWidget.h>
#include <djvUI/ToolBar.h>
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
            const size_t textElide = 20;

        } // namespace

        struct MainWindow::Private
        {
            std::vector<std::shared_ptr<Media> > media;
            std::shared_ptr<Media> currentMedia;
            bool presentationMode = false;
            bool drawerOpenInit = false;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<TimelineWidget> timelineWidget;
            std::map<std::shared_ptr<Media>, std::shared_ptr<MediaWidget> > mediaWidgets;
            std::shared_ptr<UI::VerticalLayout> menuBarLayout;
            std::shared_ptr<UI::Widget> drawerWidget;
            std::shared_ptr<UI::Layout::Drawer> drawerLayout;
            std::shared_ptr<UI::TabWidget> mediaTabWidget;
            std::shared_ptr<UI::VerticalLayout> timelineLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::List<std::shared_ptr<Media> > > mediaObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > openedObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > closedObserver;
            std::shared_ptr<Observer::Value<bool> > presentationObserver;
            std::shared_ptr<Observer::Value<float> > drawerSplitObserver;
            std::shared_ptr<Observer::Value<int> > currentToolObserver;
        };
        
        void MainWindow::_init(const std::shared_ptr<System::Context>& context)
        {
            UI::Window::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MainWindow");

            auto viewSystems = context->getSystemsT<IViewAppSystem>();
            std::map<int, std::vector<std::shared_ptr<UI::Menu> > > menusSorted;
            for (auto system : viewSystems)
            {
                for (auto actionData : system->getActions())
                {
                    addAction(actionData.second);
                }
                const auto menus = system->getMenus();
                if (!menus.empty())
                {
                    menusSorted[system->getSortKey()] = menus;
                }
            }

            p.actions["Escape"] = UI::Action::create();
            p.actions["Escape"]->setShortcut(GLFW_KEY_ESCAPE);
            for (auto i : p.actions)
            {
                addAction(i.second);
            }

            auto menuBar = UI::MenuBar::create(context);
            for (const auto& i : menusSorted)
            {
                for (const auto& j : i.second)
                {
                    menuBar->addChild(j);
                }
            }
            menuBar->addExpander(UI::Side::Right);
            menuBar->addSeparator(UI::Side::Right);
            std::map<int, std::vector<std::shared_ptr<UI::ToolBar> > > toolBars;
            for (const auto& i : viewSystems)
            {
                const auto j = i->createToolBars();
                if (!j.empty())
                {
                    toolBars[i->getSortKey()] = j;
                }
            }
            size_t count = 0;
            for (const auto& i : toolBars)
            {
                for (const auto& j : i.second)
                {
                    menuBar->addChild(j);
                }
                if (count < toolBars.size() - 1)
                {
                    menuBar->addSeparator(UI::Side::Right);
                }
                ++count;
            }

            auto backgroundWidget = WindowBackgroundWidget::create(context);

            p.mediaTabWidget = UI::TabWidget::create(context);
            p.mediaTabWidget->setTabsClosable(true);
            p.mediaTabWidget->setTextElide(textElide);

            p.timelineWidget = TimelineWidget::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.menuBarLayout = UI::VerticalLayout::create(context);
            p.menuBarLayout->setSpacing(UI::MetricsRole::None);
            p.menuBarLayout->addChild(menuBar);
            p.menuBarLayout->addSeparator();
            p.layout->addChild(p.menuBarLayout);
            p.drawerLayout = UI::Layout::Drawer::create(UI::Side::Right, context);
            auto stackLayout = UI::StackLayout::create(context);
            stackLayout->addChild(backgroundWidget);
            stackLayout->addChild(p.mediaTabWidget);
            p.drawerLayout->addChild(stackLayout);
            p.layout->addChild(p.drawerLayout);
            p.layout->setStretch(p.drawerLayout);
            p.timelineLayout = UI::VerticalLayout::create(context);
            p.timelineLayout->setSpacing(UI::MetricsRole::None);
            p.timelineLayout->addSeparator();
            p.timelineLayout->addChild(p.timelineWidget);
            p.layout->addChild(p.timelineLayout);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<System::Context>(context);
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

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.mediaTabWidget->setCurrentTabCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            std::shared_ptr<Media> media;
                            if (value >= 0 && value < widget->_p->media.size())
                            {
                                media = widget->_p->media[value];
                            }
                            if (auto fileSystem = context->getSystemT<FileSystem>())
                            {
                                fileSystem->setCurrentMedia(media);
                            }
                            std::shared_ptr<MediaWidget> mediaWidget;
                            const auto i = widget->_p->mediaWidgets.find(media);
                            if (i != widget->_p->mediaWidgets.end())
                            {
                                mediaWidget = i->second;
                            }
                            if (auto windowSystem = context->getSystemT<WindowSystem>())
                            {
                                windowSystem->setActiveWidget(mediaWidget);
                            }
                        }
                    }
                });
            p.mediaTabWidget->setTabCloseCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto fileSystem = context->getSystemT<FileSystem>())
                            {
                                if (value >= 0 && value < static_cast<int>(widget->_p->media.size()))
                                {
                                    fileSystem->close(widget->_p->media[value]);
                                }
                            }
                        }
                    }
                });

            p.drawerLayout->setOpenCallback(
                [weak, contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->drawerWidget = context->getSystemT<ToolSystem>()->createToolDrawer();
                            if (widget->_p->drawerWidget)
                            {
                                widget->_p->drawerWidget->setVisible(!widget->_p->presentationMode);
                            }
                            out = widget->_p->drawerWidget;
                        }
                    }
                    return out;
                });
            p.drawerLayout->setCloseCallback(
                [weak](const std::shared_ptr<UI::Widget>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->drawerWidget.reset();
                    }
                });
            p.drawerLayout->setSplitCallback(
                [contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto windowSettings = settingsSystem->getSettingsT<WindowSettings>();
                        windowSettings->setDrawerSplit(value);
                    }
                });

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.mediaObserver = Observer::List<std::shared_ptr<Media>>::create(
                    fileSystem->observeMedia(),
                    [weak, contextWeak](const std::vector<std::shared_ptr<Media> >& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->media = value;
                            }
                        }
                    });

                p.currentMediaObserver = Observer::Value<std::shared_ptr<Media>>::create(
                    fileSystem->observeCurrentMedia(),
                    [weak, contextWeak](const std::shared_ptr<Media>& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->currentMedia = value;
                                int index = -1;
                                const auto i = std::find(widget->_p->media.begin(), widget->_p->media.end(), value);
                                if (i != widget->_p->media.end())
                                {
                                    index = i - widget->_p->media.begin();
                                }
                                widget->_p->mediaTabWidget->setCurrentTab(index);
                                std::shared_ptr<MediaWidget> mediaWidget;
                                const auto j = widget->_p->mediaWidgets.find(value);
                                if (j != widget->_p->mediaWidgets.end())
                                {
                                    mediaWidget = j->second;
                                }
                                if (auto windowSystem = context->getSystemT<WindowSystem>())
                                {
                                    windowSystem->setActiveWidget(mediaWidget);
                                }
                            }
                        }
                    });

                p.openedObserver = Observer::Value<std::shared_ptr<Media> >::create(
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
                                    widget->_p->mediaTabWidget->addChild(mediaWidget);
                                    widget->_p->mediaTabWidget->setText(mediaWidget, value->getFileInfo().getFileName(Math::Frame::invalid, false));
                                    widget->_p->mediaTabWidget->setTooltip(mediaWidget, value->getFileInfo().getFileName());
                                    widget->_p->mediaWidgets[value] = mediaWidget;
                                }
                            }
                        }
                    });

                p.closedObserver = Observer::Value<std::shared_ptr<Media> >::create(
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
                                    widget->_p->mediaTabWidget->removeChild(i->second);
                                    widget->_p->mediaWidgets.erase(i);
                                }
                            }
                        }
                    });
            }

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto windowSettings = settingsSystem->getSettingsT<WindowSettings>();
            p.drawerSplitObserver = Observer::Value<float>::create(
                windowSettings->observeDrawerSplit(),
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->drawerLayout->setSplit(value);
                    }
                });

            auto toolSettings = settingsSystem->getSettingsT<ToolSettings>();
            p.currentToolObserver = Observer::Value<int>::create(
                toolSettings->observeCurrentTool(),
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->drawerLayout->setOpen(value != -1, widget->_p->drawerOpenInit);
                        widget->_p->drawerOpenInit = true;
                    }
                });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.presentationObserver = Observer::Value<bool>::create(
                    windowSystem->observePresentation(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->presentationMode = value;
                            widget->_presentationModeUpdate();
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

        void MainWindow::_dropEvent(System::Event::Drop& event)
        {
            if (auto context = getContext().lock())
            {
                auto fileSystem = context->getSystemT<FileSystem>();
                fileSystem->open(event.getDropPaths());
            }
        }

        void MainWindow::_presentationModeUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.menuBarLayout->setVisible(!p.presentationMode);
                if (p.drawerWidget)
                {
                    p.drawerWidget->setVisible(!p.presentationMode);
                }
                p.mediaTabWidget->getTabBar()->setVisible(!p.presentationMode);
                p.timelineLayout->setVisible(!p.presentationMode);
            }
        }

    } // namespace ViewApp
} // namespace djv

