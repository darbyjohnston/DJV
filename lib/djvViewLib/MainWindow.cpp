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

#include <djvViewLib/MainWindow.h>

#include <djvViewLib/Application.h>
#include <djvViewLib/FileSystem.h>
#include <djvViewLib/IViewSystem.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/MDIWindow.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/WindowSystem.h>
#include <djvViewLib/SettingsSystem.h>

#include <djvUI/Action.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/TabBar.h>
#include <djvUI/ToolButton.h>

#include <djvCore/FileInfo.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct MainWindow::Private
        {
            std::shared_ptr<UI::MenuBar> menuBar;
            std::shared_ptr<UI::TabBar> tabBar;
            std::shared_ptr<UI::MDI::Canvas> mdiCanvas;
            std::shared_ptr<MDIWindow> maximizedWindow;
            BBox2f maximizedGeometry;
            std::shared_ptr<UI::Layout::Solo> soloLayout;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > fileOpenedObserver;
            std::shared_ptr<ValueObserver<bool> > fileCloseObserver;
            std::shared_ptr<ValueObserver<bool> > windowMaximizeObserver;
            std::shared_ptr<ValueObserver<bool> > windowNextObserver;
            std::shared_ptr<ValueObserver<bool> > windowPrevObserver;
        };
        
        void MainWindow::_init(Core::Context * context)
        {
            UI::Window::_init(context);

            setClassName("djv::ViewLib::MainWindow");

            auto viewSystems = context->getSystemsT<IViewSystem>();
            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
                {
                    for (auto action : system->getActions())
                    {
                        addAction(action.second);
                    }
                }
            }

            _p->tabBar = UI::TabBar::create(context);
            _p->tabBar->setBackgroundRole(UI::Style::ColorRole::Overlay);

            _p->mdiCanvas = UI::MDI::Canvas::create(context);

            _p->soloLayout = UI::Layout::Solo::create(context);
            _p->soloLayout->addWidget(_p->mdiCanvas);
            addWidget(_p->soloLayout);

            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
                {
                    auto newMenu = system->createMenu();
                    if (newMenu.menu)
                    {
                        menus[newMenu.sortKey] = newMenu.menu;
                    }
                }
            }
            _p->menuBar = UI::MenuBar::create(context);
            _p->menuBar->setBackgroundRole(UI::Style::ColorRole::Overlay);
            for (auto i : menus)
            {
                _p->menuBar->addMenu(i.second);
            }
            auto settingsButton = UI::Button::Tool::create(context);
            settingsButton->setIcon("djvIconSettings");
            settingsButton->setInsideMargin(UI::Style::MetricsRole::MarginSmall);
            _p->menuBar->addWidget(settingsButton);
            
            auto layout = UI::Layout::Vertical::create(context);
            layout->setSpacing(UI::Style::MetricsRole::None);
            layout->addWidget(_p->menuBar);
            layout->addWidget(_p->tabBar);
            addWidget(layout);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            _p->tabBar->setCurrentTabCallback(
                [weak](int value)
            {
                if (auto mainWindow = weak.lock())
                {
                }
            });

            _p->mdiCanvas->setActiveWindowCallback(
                [weak, context](const std::shared_ptr<UI::Widget> & value)
            {
                if (auto mainWindow = weak.lock())
                {
                    mainWindow->_setCurrent(std::dynamic_pointer_cast<MDIWindow>(value));
                }
            });

            if (auto fileSystem = context->getSystemT<FileSystem>().lock())
            {
                _p->fileOpenedObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeOpened(),
                    [weak, context](const std::shared_ptr<Media> & value)
                {
                    if (value)
                    {
                        if (auto mainWindow = weak.lock())
                        {
                            auto mdiWindow = MDIWindow::create(context);
                            mdiWindow->setMedia(value);
                            mdiWindow->resize(glm::vec2(400.f, 300.f));
                            mdiWindow->setParent(mainWindow->_p->mdiCanvas);
                            mdiWindow->setMaximizeCallback(
                                [weak, mdiWindow]
                            {
                                if (auto mainWindow = weak.lock())
                                {
                                    mainWindow->_maximize(mdiWindow, !mainWindow->_p->maximizedWindow);
                                }
                            });
                            mdiWindow->setClosedCallback(
                                [weak, mdiWindow]
                            {
                                if (auto mainWindow = weak.lock())
                                {
                                    mainWindow->_p->soloLayout->setCurrentWidget(mainWindow->_p->mdiCanvas);
                                }
                                mdiWindow->setParent(nullptr);
                            });
                        }
                    }
                });

                _p->fileCloseObserver = ValueObserver<bool>::create(
                    fileSystem->observeClose(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto mainWindow = weak.lock())
                        {
                            if (mainWindow->_p->maximizedWindow)
                            {
                                mainWindow->_maximize(mainWindow->_p->maximizedWindow, false);
                            }
                            if (auto activeWindow = mainWindow->_p->mdiCanvas->getActiveWindow())
                            {
                                activeWindow->setParent(nullptr);
                            }
                        }
                    }
                });
            }

            if (auto windowSystem = context->getSystemT<WindowSystem>().lock())
            {
                _p->windowMaximizeObserver = ValueObserver<bool>::create(
                    windowSystem->observeMaximize(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto mainWindow = weak.lock())
                        {
                            if (mainWindow->_p->maximizedWindow)
                            {
                                mainWindow->_maximize(mainWindow->_p->maximizedWindow, false);
                            }
                            else if (auto activeWindow = std::dynamic_pointer_cast<MDIWindow>(mainWindow->_p->mdiCanvas->getActiveWindow()))
                            {
                                mainWindow->_maximize(activeWindow, true);
                            }
                        }
                    }
                });

                _p->windowNextObserver = ValueObserver<bool>::create(
                    windowSystem->observeNext(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto mainWindow = weak.lock())
                        {
                            bool maximize = false;
                            if (mainWindow->_p->maximizedWindow)
                            {
                                maximize = true;
                                mainWindow->_maximize(mainWindow->_p->maximizedWindow, false);
                            }
                            mainWindow->_p->mdiCanvas->nextWindow();
                            if (maximize)
                            {
                                if (auto window = std::dynamic_pointer_cast<MDIWindow>(mainWindow->_p->mdiCanvas->getActiveWindow()))
                                {
                                    mainWindow->_maximize(window, true);
                                }
                            }
                        }
                    }
                });

                _p->windowPrevObserver = ValueObserver<bool>::create(
                    windowSystem->observePrev(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto mainWindow = weak.lock())
                        {
                            bool maximize = false;
                            if (mainWindow->_p->maximizedWindow)
                            {
                                maximize = true;
                                mainWindow->_maximize(mainWindow->_p->maximizedWindow, false);
                            }
                            mainWindow->_p->mdiCanvas->prevWindow();
                            if (maximize)
                            {
                                if (auto window = std::dynamic_pointer_cast<MDIWindow>(mainWindow->_p->mdiCanvas->getActiveWindow()))
                                {
                                    mainWindow->_maximize(window, true);
                                }
                            }
                        }
                    }
                });
            }

            settingsButton->setClickedCallback(
                [context]
            {
                if (auto settingsSystem = context->getSystemT<SettingsSystem>().lock())
                {
                    settingsSystem->showSettingsDialog();
                }
            });
        }

        MainWindow::MainWindow() :
            _p(new Private)
        {}

        MainWindow::~MainWindow()
        {}

        std::shared_ptr<MainWindow> MainWindow::create(Core::Context * context)
        {
            auto out = std::shared_ptr<MainWindow>(new MainWindow);
            out->_init(context);
            return out;
        }

        void MainWindow::_setCurrent(const std::shared_ptr<MDIWindow> & window)
        {
            std::shared_ptr<Media> media;
            if (window)
            {
                media = window->getMedia();
            }
            auto context = getContext();
            for (auto i : context->getSystemsT<IViewSystem>())
            {
                if (auto system = i.lock())
                {
                    system->setCurrentMedia(media);
                }
            }
        }

        void MainWindow::_maximize(const std::shared_ptr<MDIWindow> & window, bool value)
        {
            DJV_PRIVATE_PTR();
            if (value)
            {
                if (p.maximizedWindow)
                {
                    p.maximizedWindow->setUIVisible(true);
                    p.maximizedWindow->setParent(p.mdiCanvas);
                    p.maximizedWindow->moveToFront();
                    p.maximizedWindow->setGeometry(p.maximizedGeometry);
                }
                p.maximizedWindow = window;
                if (p.maximizedWindow)
                {
                    p.maximizedWindow->setUIVisible(false);
                    p.maximizedGeometry = window->getGeometry();
                    p.soloLayout->addWidget(window);
                    p.soloLayout->setCurrentWidget(window);
                    _setCurrent(window);
                }
            }
            else
            {
                p.soloLayout->setCurrentWidget(p.mdiCanvas);
                if (p.maximizedWindow)
                {
                    p.maximizedWindow->setUIVisible(true);
                    p.maximizedWindow->setParent(p.mdiCanvas);
                    p.maximizedWindow->moveToFront();
                    p.maximizedWindow->setGeometry(p.maximizedGeometry);
                    p.maximizedWindow.reset();
                    p.maximizedGeometry = BBox2f(0.f, 0.f, 0.f, 0.f);
                }
            }
        }

        void MainWindow::_dropEvent(Core::Event::Drop& event)
        {
            if (auto fileSystem = getContext()->getSystemT<FileSystem>().lock())
            {
                for (const auto & i : event.getDropPaths())
                {
                    fileSystem->open(i);
                }
            }
        }

    } // namespace ViewLib
} // namespace djv

