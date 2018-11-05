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
#include <djvViewLib/ImageView.h>
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
            QMap<Enum::UI_COMPONENT, QPointer<QToolBar> > toolBars;
            QPointer<QToolBar> playbackControls;
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

            _p->toolBars[Enum::UI_FILE_TOOL_BAR] = mainWindow->fileGroup()->createToolBar();
            _p->toolBars[Enum::UI_WINDOW_TOOL_BAR] = mainWindow->windowGroup()->createToolBar();
            _p->toolBars[Enum::UI_VIEW_TOOL_BAR] = mainWindow->viewGroup()->createToolBar();
            _p->toolBars[Enum::UI_IMAGE_TOOL_BAR] = mainWindow->imageGroup()->createToolBar();
            _p->toolBars[Enum::UI_TOOLS_TOOL_BAR] = mainWindow->toolGroup()->createToolBar();
            Q_FOREACH(auto toolBar, _p->toolBars)
            {
                addToolBar(toolBar);
            }
            _p->playbackControls = mainWindow->playbackGroup()->createToolBar();
            addToolBar(Qt::ToolBarArea::BottomToolBarArea, _p->playbackControls);

            setStatusBar(new StatusBar(mainWindow, context));

            widgetUpdate();

            connect(
                mainWindow->windowGroup(),
                SIGNAL(uiComponentVisibleChanged(const QMap<djv::ViewLib::Enum::UI_COMPONENT, bool> &)),
                SLOT(widgetUpdate()));
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

        void ControlsWindow::keyPressEvent(QKeyEvent * event)
        {
            QMainWindow::keyPressEvent(event);
            switch (event->key())
            {
            case Qt::Key_Escape:
                if (_p->mainWindow->windowGroup()->hasFullScreen())
                {
                    _p->mainWindow->windowGroup()->setFullScreen(false);
                    _p->mainWindow->viewWidget()->viewFit();
                }
                break;
            }
        }

        void ControlsWindow::widgetUpdate()
        {
            const auto & visible = _p->mainWindow->windowGroup()->uiComponentVisible();
            Q_FOREACH(auto key, _p->toolBars.keys())
            {
                _p->toolBars[key]->setVisible(visible[key]);
            }
            _p->playbackControls->setVisible(visible[Enum::UI_PLAYBACK_CONTROLS]);
            statusBar()->setVisible(visible[Enum::UI_STATUS_BAR]);
        }

    } // namespace ViewLib
} // namespace djv
