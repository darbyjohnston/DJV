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
#include <djvViewLib/IToolWidget.h>
#include <djvViewLib/IViewSystem.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/MDIWidget.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/StackLayout.h>

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
            std::shared_ptr<UI::MDI::Canvas> mdiCanvas;
            std::shared_ptr<UI::ScrollWidget> mdiScrollWidget;
            std::shared_ptr<MDIWidget> maximizedWidget;
            glm::vec2 maximizedPos = glm::vec2(0.f, 0.f);
            glm::vec2 maximizedSize = glm::vec2(0.f, 0.f);
            std::shared_ptr<UI::Layout::Solo> soloLayout;
            std::shared_ptr<UI::MDI::Canvas> toolCanvas;
            std::shared_ptr<UI::Layout::Stack> stackLayout;
            std::shared_ptr<ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> > > fileOpenedObserver;
            std::shared_ptr<ValueObserver<bool> > fileCloseObserver;
            std::shared_ptr<ValueObserver<bool> > widgetMaximizeObserver;
            std::shared_ptr<ValueObserver<bool> > widgetNextObserver;
            std::shared_ptr<ValueObserver<bool> > widgetPrevObserver;
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

            _p->mdiCanvas = UI::MDI::Canvas::create(context);
            _p->mdiCanvas->setBackgroundRole(UI::Style::ColorRole::Trough);
            _p->mdiScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            _p->mdiScrollWidget->setBorder(false);
            _p->mdiScrollWidget->addWidget(_p->mdiCanvas);

            _p->soloLayout = UI::Layout::Solo::create(context);
            _p->soloLayout->addWidget(_p->mdiScrollWidget);

            _p->toolCanvas = UI::MDI::Canvas::create(context);

            _p->stackLayout = UI::Layout::Stack::create(context);
            _p->stackLayout->addWidget(_p->soloLayout);
            _p->stackLayout->addWidget(_p->toolCanvas);

            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
                {
                    auto newToolWidget = system->createToolWidget();
                    if (newToolWidget.widget)
                    {
                        newToolWidget.widget->setParent(_p->toolCanvas);
                        _p->toolCanvas->setWidgetPos(newToolWidget.widget, newToolWidget.pos);
                    }
                }
            }

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
            _p->menuBar->setBackgroundRole(UI::Style::ColorRole::Background);
            for (auto i : menus)
            {
                _p->menuBar->addMenu(i.second);
            }
            
            auto layout = UI::Layout::Vertical::create(context);
            layout->setSpacing(UI::Style::MetricsRole::None);
            layout->addWidget(_p->menuBar);
            layout->addSeparator();
            layout->addWidget(_p->stackLayout, UI::Layout::RowStretch::Expand);
            addWidget(layout);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            _p->mdiCanvas->setActiveCallback(
                [weak, context](const std::shared_ptr<UI::Widget> & value)
            {
                if (auto mainWindow = weak.lock())
                {
                    mainWindow->_setCurrent(std::dynamic_pointer_cast<MDIWidget>(value));
                }
            });

            if (auto fileSystem = context->getSystemT<FileSystem>().lock())
            {
                _p->fileOpenedObserver = ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> >::create(
                    fileSystem->observeOpened(),
                    [weak, context](const std::pair<std::shared_ptr<Media>, glm::vec2> & value)
                {
                    if (value.first)
                    {
                        if (auto mainWindow = weak.lock())
                        {
                            auto mdiWidget = MDIWidget::create(context);
                            mdiWidget->setTitle(Core::FileSystem::FileInfo(value.first->getFileName()).getFileName(Frame::Invalid, false));
                            mdiWidget->setMedia(value.first);
                            mdiWidget->setParent(mainWindow->_p->mdiCanvas);
                            glm::vec2 widgetPos(0.f, 0.f);
                            if (value.second.x >= 0.f && value.second.y >= 0.f)
                            {
                                widgetPos = value.second - mainWindow->_p->mdiCanvas->getGeometry().min;
                            }
                            else
                            {
                                widgetPos = mainWindow->getGeometry().getSize() / 2.f - mainWindow->_p->mdiCanvas->getGeometry().min;
                            }
                            mainWindow->_p->mdiCanvas->setWidgetPos(mdiWidget, widgetPos);
                            mdiWidget->setMaximizeCallback(
                                [weak, mdiWidget]
                            {
                                if (auto mainWindow = weak.lock())
                                {
                                    mainWindow->_maximize(mdiWidget, !mainWindow->_p->maximizedWidget);
                                }
                            });
                            mdiWidget->setClosedCallback(
                                [weak, mdiWidget]
                            {
                                if (auto mainWindow = weak.lock())
                                {
                                    mainWindow->_p->soloLayout->setCurrentWidget(mainWindow->_p->mdiScrollWidget);
                                }
                                mdiWidget->setParent(nullptr);
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
                            if (mainWindow->_p->maximizedWidget)
                            {
                                mainWindow->_maximize(mainWindow->_p->maximizedWidget, false);
                            }
                            if (auto activeWidget = mainWindow->_p->mdiCanvas->getActiveWidget())
                            {
                                activeWidget->setParent(nullptr);
                            }
                        }
                    }
                });
            }

            if (auto windowSystem = context->getSystemT<WindowSystem>().lock())
            {
                _p->widgetMaximizeObserver = ValueObserver<bool>::create(
                    windowSystem->observeMaximize(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto mainWindow = weak.lock())
                        {
                            if (mainWindow->_p->maximizedWidget)
                            {
                                mainWindow->_maximize(mainWindow->_p->maximizedWidget, false);
                            }
                            else if (auto activeWidget = std::dynamic_pointer_cast<MDIWidget>(mainWindow->_p->mdiCanvas->getActiveWidget()))
                            {
                                mainWindow->_maximize(activeWidget, true);
                            }
                        }
                    }
                });

                _p->widgetNextObserver = ValueObserver<bool>::create(
                    windowSystem->observeNext(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto mainWindow = weak.lock())
                        {
                            bool maximize = false;
                            if (mainWindow->_p->maximizedWidget)
                            {
                                maximize = true;
                                mainWindow->_maximize(mainWindow->_p->maximizedWidget, false);
                            }
                            mainWindow->_p->mdiCanvas->nextWidget();
                            if (maximize)
                            {
                                if (auto widget = std::dynamic_pointer_cast<MDIWidget>(mainWindow->_p->mdiCanvas->getActiveWidget()))
                                {
                                    mainWindow->_maximize(widget, true);
                                }
                            }
                        }
                    }
                });

                _p->widgetPrevObserver = ValueObserver<bool>::create(
                    windowSystem->observePrev(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto mainWindow = weak.lock())
                        {
                            bool maximize = false;
                            if (mainWindow->_p->maximizedWidget)
                            {
                                maximize = true;
                                mainWindow->_maximize(mainWindow->_p->maximizedWidget, false);
                            }
                            mainWindow->_p->mdiCanvas->prevWidget();
                            if (maximize)
                            {
                                if (auto widget = std::dynamic_pointer_cast<MDIWidget>(mainWindow->_p->mdiCanvas->getActiveWidget()))
                                {
                                    mainWindow->_maximize(widget, true);
                                }
                            }
                        }
                    }
                });
            }
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

        void MainWindow::_setCurrent(const std::shared_ptr<MDIWidget> & widget)
        {
            std::shared_ptr<Media> media;
            if (widget)
            {
                media = widget->getMedia();
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

        void MainWindow::_maximize(const std::shared_ptr<MDIWidget> & widget, bool value)
        {
            DJV_PRIVATE_PTR();
            if (value)
            {
                if (p.maximizedWidget)
                {
                    p.maximizedWidget->setUIVisible(true);
                    p.maximizedWidget->setParent(p.mdiCanvas);
                    p.maximizedWidget->resize(p.maximizedSize);
                    p.maximizedWidget->moveToFront();
                    p.mdiCanvas->setWidgetPos(p.maximizedWidget, p.maximizedSize);
                }
                p.maximizedWidget = widget;
                if (p.maximizedWidget)
                {
                    p.maximizedWidget->setUIVisible(false);
                    p.maximizedPos = _p->mdiCanvas->getWidgetPos(p.maximizedWidget);
                    p.maximizedSize = p.maximizedWidget->getSize();
                    p.soloLayout->addWidget(widget);
                    p.soloLayout->setCurrentWidget(widget);
                    _setCurrent(widget);
                }
            }
            else
            {
                p.soloLayout->setCurrentWidget(p.mdiScrollWidget);
                if (p.maximizedWidget)
                {
                    p.maximizedWidget->setUIVisible(true);
                    p.maximizedWidget->setParent(p.mdiCanvas);
                    p.maximizedWidget->resize(p.maximizedSize);
                    p.maximizedWidget->moveToFront();
                    p.mdiCanvas->setWidgetPos(p.maximizedWidget, p.maximizedPos);
                    p.maximizedWidget.reset();
                    p.maximizedPos = glm::vec2(0.f, 0.f);
                    p.maximizedSize = glm::vec2(0.f, 0.f);
                }
            }
        }

        void MainWindow::_dropEvent(Core::Event::Drop& event)
        {
            if (auto fileSystem = getContext()->getSystemT<FileSystem>().lock())
            {
                for (const auto & i : event.getDropPaths())
                {
                    fileSystem->open(i, event.getPointerInfo().projectedPos);
                }
            }
        }

    } // namespace ViewLib
} // namespace djv

