//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvViewLib/ControlsWindow.h>

#include <djvViewLib/FileGroup.h>
#include <djvViewLib/HelpGroup.h>
#include <djvViewLib/ImageGroup.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/StatusBar.h>
#include <djvViewLib/ToolGroup.h>
#include <djvViewLib/ViewGroup.h>
#include <djvViewLib/WindowGroup.h>

#include <QCloseEvent>
#include <QMenuBar>
#include <QToolBar>

namespace djv
{
    namespace ViewLib
    {
        struct ControlsWindow::Private
        {
            QPointer<MainWindow> mainWindow;
        };

        ControlsWindow::ControlsWindow(
            const QPointer<MainWindow> & mainWindow,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            QMainWindow(parent),
            _p(new Private)
        {
            _p->mainWindow = mainWindow;

            setWindowFlags(Qt::Dialog | Qt::Tool);

            menuBar()->setNativeMenuBar(false);
            menuBar()->addMenu(mainWindow->fileGroup()->createMenu());
            menuBar()->addMenu(mainWindow->windowGroup()->createMenu());
            menuBar()->addMenu(mainWindow->viewGroup()->createMenu());
            menuBar()->addMenu(mainWindow->imageGroup()->createMenu());
            menuBar()->addMenu(mainWindow->playbackGroup()->createMenu());
            menuBar()->addMenu(mainWindow->toolGroup()->createMenu());
            menuBar()->addMenu(mainWindow->helpGroup()->createMenu());

            addToolBar(mainWindow->fileGroup()->createToolBar());
            addToolBar(mainWindow->windowGroup()->createToolBar());
            addToolBar(mainWindow->viewGroup()->createToolBar());
            addToolBar(mainWindow->imageGroup()->createToolBar());
            addToolBar(Qt::ToolBarArea::BottomToolBarArea, mainWindow->playbackGroup()->createToolBar());
            addToolBar(mainWindow->toolGroup()->createToolBar());

            setStatusBar(new StatusBar(mainWindow, context));
        }

        ControlsWindow::~ControlsWindow()
        {}

        QMenu * ControlsWindow::createPopupMenu()
        {
            auto out = new QMenu;
            out->addMenu(_p->mainWindow->fileGroup()->createMenu());
            out->addMenu(_p->mainWindow->windowGroup()->createMenu());
            out->addMenu(_p->mainWindow->viewGroup()->createMenu());
            out->addMenu(_p->mainWindow->imageGroup()->createMenu());
            out->addMenu(_p->mainWindow->playbackGroup()->createMenu());
            out->addMenu(_p->mainWindow->toolGroup()->createMenu());
            out->addMenu(_p->mainWindow->helpGroup()->createMenu());
            return out;
        }

        void ControlsWindow::closeEvent(QCloseEvent * event)
        {
            event->ignore();
            Q_EMIT closed();
        }

    } // namespace ViewLib
} // namespace djv
