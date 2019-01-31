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
#include <djvViewLib/MediaWidget.h>
#include <djvViewLib/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/StackLayout.h>
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
            std::shared_ptr<UI::MDI::Canvas> mdiCanvas;
            std::shared_ptr<UI::ScrollWidget> mdiScrollWidget;
            std::shared_ptr<UI::Button::Tool> mdiButton;
            std::map<std::shared_ptr<Media>, std::shared_ptr<MDIWidget> > mediaToMDIWidget;
            WindowMode windowMode = WindowMode::SDI;
            std::shared_ptr<MediaWidget> sdiWidget;
            std::shared_ptr<UI::Layout::Solo> soloLayout;
            std::shared_ptr<UI::MDI::Canvas> toolCanvas;
            std::shared_ptr<UI::Layout::Stack> stackLayout;
            std::shared_ptr<ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> > > fileOpenedObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > fileClosedObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<WindowMode> > windowModeObserver;
        };
        
        void MainWindow::_init(Core::Context * context)
        {
            UI::Window::_init(context);

            setClassName("djv::ViewLib::MainWindow");
            setBackgroundRole(UI::Style::ColorRole::Trough);

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

            DJV_PRIVATE_PTR();
            p.mdiCanvas = UI::MDI::Canvas::create(context);
            p.mdiScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            p.mdiScrollWidget->setBorder(false);
            p.mdiScrollWidget->addWidget(p.mdiCanvas);
            p.mdiButton = UI::Button::Tool::create(context);
            p.mdiButton->setIcon("djvIconMaximizeSmall");
            p.mdiButton->setInsideMargin(UI::Style::MetricsRole::None);

            p.sdiWidget = MediaWidget::create(context);

            p.soloLayout = UI::Layout::Solo::create(context);
            p.soloLayout->addWidget(p.mdiScrollWidget);
            p.soloLayout->addWidget(p.sdiWidget);

            p.toolCanvas = UI::MDI::Canvas::create(context);
            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
                {
                    for (auto j : system->getToolWidgets())
                    {
                        j.widget->setParent(p.toolCanvas);
                        p.toolCanvas->setWidgetPos(j.widget, j.pos);
                    }
                }
            }

            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
                {
                    auto newMenu = system->getMenu();
                    if (newMenu.menu)
                    {
                        menus[newMenu.sortKey] = newMenu.menu;
                    }
                }
            }
            p.menuBar = UI::MenuBar::create(context);
            p.menuBar->setBackgroundRole(UI::Style::ColorRole::Overlay);
            for (auto i : menus)
            {
                p.menuBar->addMenu(i.second);
            }
            p.menuBar->addWidget(p.mdiButton);
            
            p.stackLayout = UI::Layout::Stack::create(context);
            p.stackLayout->addWidget(p.soloLayout);
            p.stackLayout->addWidget(p.toolCanvas);
            auto vLayout = UI::Layout::Vertical::create(context);
            vLayout->setSpacing(UI::Style::MetricsRole::None);
            vLayout->addWidget(p.menuBar);
            vLayout->addSeparator();
            vLayout->addExpander();
            p.stackLayout->addWidget(vLayout);
            addWidget(p.stackLayout);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.mdiCanvas->setActiveCallback(
                [weak, context](const std::shared_ptr<UI::Widget> & value)
            {
                if (auto mainWindow = weak.lock())
                {
                    if (auto mdiWidget = std::dynamic_pointer_cast<MDIWidget>(value))
                    {
                        if (auto fileSystem = context->getSystemT<FileSystem>().lock())
                        {
                            fileSystem->setCurrentMedia(mdiWidget->getMedia());
                        }
                    }
                }
            });

            p.mdiButton->setClickedCallback(
                [context]
            {
                if (auto windowSystem = context->getSystemT<WindowSystem>().lock())
                {
                    windowSystem->setWindowMode(WindowMode::MDI);
                }
            });

            if (auto fileSystem = context->getSystemT<FileSystem>().lock())
            {
                p.fileOpenedObserver = ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> >::create(
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
                            glm::vec2 size(mainWindow->getSize() / 2.f);
                            mdiWidget->resize(size);
                            mdiWidget->setParent(mainWindow->_p->mdiCanvas);
                            glm::vec2 pos(0.f, 0.f);
                            if (value.second.x >= 0.f && value.second.y >= 0.f)
                            {
                                pos = value.second - mainWindow->_p->mdiCanvas->getGeometry().min;
                            }
                            else
                            {
                                pos = mainWindow->getGeometry().getSize() / 2.f - mainWindow->_p->mdiCanvas->getGeometry().min;
                            }
                            mainWindow->_p->mdiCanvas->setWidgetPos(mdiWidget, pos - size / 2.f);
                            mainWindow->_p->mediaToMDIWidget[value.first] = mdiWidget;
                            mdiWidget->setMaximizeCallback(
                                [context]
                            {
                                if (auto windowSystem = context->getSystemT<WindowSystem>().lock())
                                {
                                    windowSystem->setWindowMode(WindowMode::SDI);
                                }
                            });
                            mdiWidget->setClosedCallback(
                                [context, mdiWidget]
                            {
                                if (auto fileSystem = context->getSystemT<FileSystem>().lock())
                                {
                                    fileSystem->close(mdiWidget->getMedia());
                                }
                            });
                        }
                    }
                });

                p.fileClosedObserver = ValueObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeClosed(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto mainWindow = weak.lock())
                    {
                        const auto i = mainWindow->_p->mediaToMDIWidget.find(value);
                        if (i != mainWindow->_p->mediaToMDIWidget.end())
                        {
                            i->second->setParent(nullptr);
                            mainWindow->_p->mediaToMDIWidget.erase(i);
                        }
                    }
                });

                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto mainWindow = weak.lock())
                    {
                        const auto i = mainWindow->_p->mediaToMDIWidget.find(value);
                        if (i != mainWindow->_p->mediaToMDIWidget.end())
                        {
                            i->second->moveToFront();
                        }
                        mainWindow->_p->sdiWidget->setMedia(value);
                    }
                });
            }

            if (auto windowSystem = context->getSystemT<WindowSystem>().lock())
            {
                windowSystem->setWindowMode(p.windowMode);

                _p->windowModeObserver = ValueObserver<WindowMode>::create(
                    windowSystem->observeWindowMode(),
                    [weak](WindowMode value)
                {
                    if (auto mainWindow = weak.lock())
                    {
                        switch (value)
                        {
                        case WindowMode::SDI:
                            mainWindow->_p->soloLayout->setCurrentWidget(mainWindow->_p->sdiWidget);
                            mainWindow->_p->mdiButton->show();
                            break;
                        case WindowMode::MDI:
                            mainWindow->_p->soloLayout->setCurrentWidget(mainWindow->_p->mdiScrollWidget);
                            mainWindow->_p->mdiButton->hide();
                            break;
                        default: break;
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

        void MainWindow::_dropEvent(Core::Event::Drop& event)
        {
            if (auto style = _getStyle().lock())
            {
                if (auto fileSystem = getContext()->getSystemT<FileSystem>().lock())
                {
                    const float s = style->getMetric(UI::Style::MetricsRole::SpacingLarge);
                    glm::vec2 pos = event.getPointerInfo().projectedPos;
                    for (const auto & i : event.getDropPaths())
                    {
                        fileSystem->open(i, pos);
                        pos += s;
                    }
                }
            }
        }

        void MainWindow::_localeEvent(Core::Event::Locale & event)
        {
            DJV_PRIVATE_PTR();
            p.mdiButton->setTooltip(_getText(DJV_TEXT("djv::ViewLib::MainWindow", "MDI WindowMode Tooltip")));
        }

    } // namespace ViewLib
} // namespace djv

