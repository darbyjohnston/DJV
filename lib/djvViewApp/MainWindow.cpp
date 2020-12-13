// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MainWindow.h>

#include <djvViewApp/Application.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/TimelineWidget.h>
#include <djvViewApp/ToolSystem.h>
#include <djvViewApp/ViewSystem.h>
#include <djvViewApp/WindowBackgroundWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/ThermometerWidget.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Separator.h>
#include <djvUI/Shortcut.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/Splitter.h>
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
            const size_t textElide = 40;

        } // namespace

        struct MainWindow::Private
        {
            std::shared_ptr<Media> currentMedia;
            
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ToolButton> activeButton;
            std::shared_ptr<UI::ToolButton> drawerButton;
            std::shared_ptr<UI::MenuBar> menuBar;
            std::shared_ptr<TimelineWidget> timelineWidget;
            std::map<std::shared_ptr<Media>, std::shared_ptr<MediaWidget> > mediaWidgets;
            std::shared_ptr<UI::VerticalLayout> menuBarLayout;
            std::shared_ptr<UI::SoloLayout> mediaLayout;
            std::shared_ptr<UI::StackLayout> drawerLayout;
            std::shared_ptr<UI::VerticalLayout> timelineLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > openedObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > closedObserver;
            std::shared_ptr<Observer::Value<bool> > presentationObserver;
        };
        
        void MainWindow::_init(const std::shared_ptr<System::Context>& context)
        {
            UI::Window::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MainWindow");

            auto viewSystems = context->getSystemsT<IViewAppSystem>();
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            for (auto system : viewSystems)
            {
                for (auto action : system->getActions())
                {
                    addAction(action.second);
                }
                for (const auto& menuData : system->getMenuData())
                {
                    menus[menuData.sortKey] = menuData.menu;
                }
            }

            p.actions["Escape"] = UI::Action::create();
            p.actions["Escape"]->setShortcut(GLFW_KEY_ESCAPE);
            for (auto i : p.actions)
            {
                addAction(i.second);
            }

            p.activeButton = UI::ToolButton::create(context);
            p.activeButton->setTextElide(textElide);
            p.activeButton->setInsideMargin(UI::Layout::Margin(
                UI::MetricsRole::Margin,
                UI::MetricsRole::Margin,
                UI::MetricsRole::MarginInside,
                UI::MetricsRole::MarginInside));

            auto viewFrameButton = UI::ToolButton::create(context);
            auto viewCenterButton = UI::ToolButton::create(context);
            auto viewSystem = context->getSystemT<ViewSystem>();
            if (viewSystem)
            {
                viewFrameButton->addAction(viewSystem->getActions()["FrameLock"]);
                viewCenterButton->addAction(viewSystem->getActions()["CenterLock"]);
            }

            p.drawerButton = UI::ToolButton::create(context);
            p.drawerButton->setButtonType(UI::ButtonType::Toggle);
            p.drawerButton->setIcon("djvIconDrawerRight");

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
            p.menuBar->addChild(p.drawerButton);

            auto backgroundWidget = WindowBackgroundWidget::create(context);

            p.timelineWidget = TimelineWidget::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.menuBarLayout = UI::VerticalLayout::create(context);
            p.menuBarLayout->setSpacing(UI::MetricsRole::None);
            p.menuBarLayout->addChild(p.menuBar);
            p.menuBarLayout->addSeparator();
            p.layout->addChild(p.menuBarLayout);
            auto stackLayout = UI::StackLayout::create(context);
            stackLayout->addChild(backgroundWidget);
            p.mediaLayout = UI::SoloLayout::create(context);
            stackLayout->addChild(p.mediaLayout);
            auto splitter = UI::Layout::Splitter::create(UI::Orientation::Horizontal, context);
            splitter->addChild(stackLayout);
            p.drawerLayout = UI::StackLayout::create(context);
            splitter->addChild(p.drawerLayout);
            p.layout->addChild(splitter);
            p.layout->setStretch(splitter, UI::Layout::RowStretch::Expand);
            p.timelineLayout = UI::VerticalLayout::create(context);
            p.timelineLayout->setSpacing(UI::MetricsRole::None);
            p.timelineLayout->addSeparator();
            p.timelineLayout->addChild(p.timelineWidget);
            p.layout->addChild(p.timelineLayout);
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

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.drawerButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (value)
                            {
                                auto toolDrawerWidget = context->getSystemT<ToolSystem>()->createToolDrawerWidget();
                                widget->_p->drawerLayout->addChild(toolDrawerWidget);
                                widget->_p->drawerLayout->show();
                            }
                            else
                            {
                                widget->_p->drawerLayout->hide();
                                widget->_p->drawerLayout->clearChildren();
                            }
                        }
                    }
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

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = Observer::Value<std::shared_ptr<Media>>::create(
                    fileSystem->observeCurrentMedia(),
                    [weak, contextWeak](const std::shared_ptr<Media>& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->currentMedia = value;
                                widget->_p->activeButton->setEnabled(value.get());
                                std::shared_ptr<MediaWidget> mediaWidget;
                                const auto i = widget->_p->mediaWidgets.find(value);
                                if (i != widget->_p->mediaWidgets.end())
                                {
                                    mediaWidget = i->second;
                                }
                                if (auto windowSystem = context->getSystemT<WindowSystem>())
                                {
                                    windowSystem->setActiveWidget(mediaWidget);
                                }
                                widget->_p->mediaLayout->setCurrentWidget(mediaWidget);
                                widget->_textUpdate();
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
                                    widget->_p->mediaLayout->addChild(mediaWidget);
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
                                    widget->_p->mediaLayout->removeChild(i->second);
                                    widget->_p->mediaWidgets.erase(i);
                                }
                            }
                        }
                    });
            }

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.presentationObserver = Observer::Value<bool>::create(
                    windowSystem->observePresentation(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->menuBarLayout->setVisible(!value);
                            widget->_p->timelineLayout->setVisible(!value);
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

