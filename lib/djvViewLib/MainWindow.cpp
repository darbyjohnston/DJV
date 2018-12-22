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
        };
        
        void MainWindow::_init(Core::Context * context)
        {
            Window::_init(context);

            auto fileMenu = UI::Menu::create("File", context);
            auto action = UI::Action::create();
            action->setText("Open");
            fileMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Reload");
            fileMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Close");
            fileMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Export");
            fileMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Exit");
            fileMenu->addAction(action);

            auto windowMenu = UI::Menu::create("Window", context);
            action = UI::Action::create();
            action->setText("New Tab");
            windowMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Close Tab");
            windowMenu->addAction(action);

            auto viewMenu = UI::Menu::create("View", context);
            action = UI::Action::create();
            action->setText("Zoom In");
            viewMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Zoom Out");
            viewMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Reset Zoom");
            viewMenu->addAction(action);

            auto playbackMenu = UI::Menu::create("Playback", context);
            action = UI::Action::create();
            action->setText("Stop");
            playbackMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Forward");
            playbackMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Reverse");
            playbackMenu->addAction(action);

            auto toolsMenu = UI::Menu::create("Tools", context);
            action = UI::Action::create();
            action->setText("Magnify");
            toolsMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Color Picker");
            toolsMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Histogram");
            toolsMenu->addAction(action);
            action = UI::Action::create();
            action->setText("Information");
            toolsMenu->addAction(action);

            _p->menuBar = UI::MenuBar::create(context);
            _p->menuBar->addMenu(fileMenu);
            _p->menuBar->addMenu(windowMenu);
            _p->menuBar->addMenu(viewMenu);
            _p->menuBar->addMenu(playbackMenu);
            _p->menuBar->addMenu(toolsMenu);

            _p->tabWidget = UI::TabWidget::create(context);

            auto layout = UI::Layout::VerticalLayout::create(context);
            layout->setSpacing(UI::Style::MetricsRole::None);
            layout->addWidget(_p->menuBar);
            layout->addSeparator();
            layout->addWidget(_p->tabWidget, UI::Layout::RowStretch::Expand);
            addWidget(layout);

            //_p->canvas = UI::MDI::Canvas::create(context);
            //_p->canvas->setCanvasSize(glm::vec2(8192, 8192));
            //_p->scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            //_p->scrollWidget->addWidget(_p->canvas);
            //addWidget(_p->scrollWidget);

            auto exitShortcut = UI::Shortcut::create(GLFW_KEY_Q, GLFW_MOD_CONTROL);
            auto exitAction = UI::Action::create();
            exitAction->setShortcut(exitShortcut);
            addAction(exitAction);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            exitShortcut->setCallback(
                [weak, context]
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

        void MainWindow::dropEvent(Core::Event::Drop& event)
        {
            for (const auto & i : event.getDropPaths())
            {
                //auto media = Media::create(FileInfo::getFileSequence(i), getContext());
                auto media = Media::create(i, getContext());
                auto window = MDIWindow::create(media, getContext());
                const auto title = FileSystem::Path(i).getFileName();
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
                            mainWindow->_p->tabWidget->removeWidget(window);
                        }
                    }
                });
                _p->tabWidget->addWidget(title, window);
            }
        }
        
    } // namespace ViewLib
} // namespace djv

