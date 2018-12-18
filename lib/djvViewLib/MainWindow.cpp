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
#include <djvUI/ScrollWidget.h>
#include <djvUI/Shortcut.h>

#include <djvCore/FileInfo.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct MainWindow::Private
        {
            std::shared_ptr<UI::MDI::Canvas> canvas;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;
        };
        
        void MainWindow::_init(Core::Context * context)
        {
            Window::_init(context);

            _p->canvas = UI::MDI::Canvas::create(context);
            _p->canvas->setCanvasSize(glm::vec2(8192, 8192));

            _p->scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            _p->scrollWidget->addWidget(_p->canvas);
            addWidget(_p->scrollWidget);

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
                    UI::confirmationDialog(
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

        void MainWindow::dropEvent(Core::DropEvent& event)
        {
            glm::vec2 pos = event.getPointerInfo().projectedPos;
            for (const auto & i : event.getDropPaths())
            {
                //auto media = Media::create(FileInfo::getFileSequence(i), getContext());
                auto media = Media::create(i, getContext());
                auto window = MDIWindow::create(media, getContext());
                window->setTitle(Path(i).getFileName());
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
                            mainWindow->_p->canvas->removeWindow(window);
                        }
                    }
                });
                _p->canvas->addWindow(window, pos);
            }
        }
        
    } // namespace ViewLib
} // namespace djv

