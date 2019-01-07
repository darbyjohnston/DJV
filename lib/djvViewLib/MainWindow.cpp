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
#include <djvViewLib/MDIWindow.h>
#include <djvViewLib/Media.h>

#include <djvUI/Action.h>
#include <djvUI/Dialog.h>
#include <djvUI/FileBrowser.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Shortcut.h>
#include <djvUI/TabWidget.h>

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
            std::shared_ptr<UI::TabWidget> tabWidget;
            //std::shared_ptr<UI::MDI::Canvas> canvas;
            //std::shared_ptr<UI::ScrollWidget> scrollWidget;
            std::shared_ptr<UI::ActionGroup> playbackActionGroup;
            std::shared_ptr<ValueObserver<bool> > fileOpenObserver;
            std::shared_ptr<ValueObserver<bool> > exitObserver;
        };
        
        void MainWindow::_init(Core::Context * context)
        {
            UI::MainWindow::_init(context);

            setClassName("djv::ViewLib::MainWindow");

            auto fileOpenAction = UI::Action::create();
            fileOpenAction->setText("Open");
            fileOpenAction->setShortcut(GLFW_KEY_O, GLFW_MOD_CONTROL);
            addAction(fileOpenAction);
            auto fileReloadAction = UI::Action::create();
            fileReloadAction->setText("Reload");
            addAction(fileReloadAction);
            auto fileCloseAction = UI::Action::create();
            fileCloseAction->setText("Close");
            addAction(fileCloseAction);
            auto fileExportAction = UI::Action::create();
            fileExportAction->setText("Export");
            addAction(fileExportAction);
            auto exitAction = UI::Action::create();
            exitAction->setText("Exit");
            exitAction->setShortcut(GLFW_KEY_Q, GLFW_MOD_CONTROL);
            addAction(exitAction);

            auto windowNewTabAction = UI::Action::create();
            windowNewTabAction->setText("New Tab");
            addAction(windowNewTabAction);
            auto windowCloseTabAction = UI::Action::create();
            windowCloseTabAction->setText("Close Tab");
            addAction(windowCloseTabAction);

            auto viewZoomInAction = UI::Action::create();
            viewZoomInAction->setText("Zoom In");
            addAction(viewZoomInAction);
            auto viewZoomOutAction = UI::Action::create();
            viewZoomOutAction->setText("Zoom Out");
            addAction(viewZoomOutAction);
            auto viewResetZoomAction = UI::Action::create();
            viewResetZoomAction->setText("Reset Zoom");
            addAction(viewResetZoomAction);

            auto playbackStopAction = UI::Action::create();
            playbackStopAction->setText("Stop");
            playbackStopAction->setIcon(context->getPath(Core::FileSystem::ResourcePath::IconsDirectory, "djvIconPlaybackStop90DPI.png"));
            playbackStopAction->setShortcut(GLFW_KEY_K);
            addAction(playbackStopAction);
            auto playbackForwardAction = UI::Action::create();
            playbackForwardAction->setText("Forward");
            playbackForwardAction->setIcon(context->getPath(Core::FileSystem::ResourcePath::IconsDirectory, "djvIconPlaybackForward90DPI.png"));
            playbackForwardAction->setShortcut(GLFW_KEY_L);
            addAction(playbackForwardAction);
            auto playbackReverseAction = UI::Action::create();
            playbackReverseAction->setText("Reverse");
            playbackReverseAction->setIcon(context->getPath(Core::FileSystem::ResourcePath::IconsDirectory, "djvIconPlaybackReverse90DPI.png"));
            playbackReverseAction->setShortcut(GLFW_KEY_J);
            addAction(playbackReverseAction);
            _p->playbackActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            _p->playbackActionGroup->addAction(playbackStopAction);
            _p->playbackActionGroup->addAction(playbackForwardAction);
            _p->playbackActionGroup->addAction(playbackReverseAction);

            auto fileMenu = UI::Menu::create("File", context);
            fileMenu->addAction(fileOpenAction);
            fileMenu->addAction(fileReloadAction);
            fileMenu->addAction(fileCloseAction);
            fileMenu->addAction(fileExportAction);
            fileMenu->addAction(exitAction);

            auto windowMenu = UI::Menu::create("Window", context);
            windowMenu->addAction(windowNewTabAction);
            windowMenu->addAction(windowCloseTabAction);

            auto viewMenu = UI::Menu::create("View", context);
            viewMenu->addAction(viewZoomInAction);
            viewMenu->addAction(viewZoomOutAction);
            viewMenu->addAction(viewResetZoomAction);

            auto playbackMenu = UI::Menu::create("Playback", context);
            playbackMenu->addAction(playbackStopAction);
            playbackMenu->addAction(playbackForwardAction);
            playbackMenu->addAction(playbackReverseAction);

            auto menuBar = getMenuBar();
            menuBar->addMenu(fileMenu);
            menuBar->addMenu(windowMenu);
            menuBar->addMenu(viewMenu);
            menuBar->addMenu(playbackMenu);

            _p->tabWidget = UI::TabWidget::create(context);
            setCentralWidget(_p->tabWidget);

            //_p->canvas = UI::MDI::Canvas::create(context);
            //_p->canvas->setCanvasSize(glm::vec2(8192, 8192));
            //_p->scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            //_p->scrollWidget->addWidget(_p->canvas);
            //setCentralWidget(_p->scrollWidget);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            _p->tabWidget->setCurrentTabCallback(
                [weak](int value)
            {

            });

            _p->fileOpenObserver = ValueObserver<bool>::create(
                fileOpenAction->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto mainWindow = weak.lock())
                    {
                        UI::FileBrowser::dialog(
                            mainWindow,
                            [weak, context](const FileSystem::FileInfo & value)
                        {
                            if (auto mainWindow = weak.lock())
                            {
                                mainWindow->_open(value.getPath());
                            }
                        });
                    }
                }
            });

            _p->exitObserver = ValueObserver<bool>::create(
                exitAction->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto mainWindow = weak.lock())
                    {
                        UI::Dialog::confirmation(
                            DJV_TEXT("Are you sure you want to exit?"),
                            DJV_TEXT("Yes"),
                            DJV_TEXT("No"),
                            mainWindow,
                            [context](bool value)
                        {
                            if (value)
                            {
                                dynamic_cast<Application *>(context)->exit();
                            }
                        });
                    }
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

        void MainWindow::_dropEvent(Core::Event::Drop& event)
        {
            for (const auto & i : event.getDropPaths())
            {
                _open(i);
            }
        }

        void MainWindow::_open(const FileSystem::Path & path)
        {
            //auto media = Media::create(FileInfo::getFileSequence(i), getContext());
            auto media = Media::create(path, getContext());
            auto window = MDIWindow::create(media, getContext());
            const auto title = path.getFileName();
            window->setTitle(title);
            window->resize(glm::vec2(500.f, 300.f));
            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            auto weakWindow = std::weak_ptr<MDIWindow>(std::dynamic_pointer_cast<MDIWindow>(window));
            window->setClosedCallback(
                [weak, weakWindow]
            {
                if (auto mainWindow = weak.lock())
                {
                    if (auto window = weakWindow.lock())
                    {
                        mainWindow->_p->tabWidget->removeTab(window);
                    }
                }
            });
            const size_t index = _p->tabWidget->addTab(title, window);
            _p->tabWidget->setCurrentTab(static_cast<int>(index));
        }

    } // namespace ViewLib
} // namespace djv

