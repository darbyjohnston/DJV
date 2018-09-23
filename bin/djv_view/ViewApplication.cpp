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

#include <djv_view/ViewApplication.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/MainWindow.h>

#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfoUtil.h>

#include <QFileOpenEvent>
#include <QPointer>
#include <QScopedPointer>
#include <QTimer>

namespace djv
{
    namespace view
    {
        struct Application::Private
        {
            QScopedPointer<ViewLib::Context> context;
        };

        Application::Application(int & argc, char ** argv) :
            QApplication(argc, argv),
            _p(new Private)
        {
            //DJV_DEBUG("Application::Application");

            setOrganizationName("djv.sourceforge.net");
            setApplicationName("djv_view");

            // Create the context.
            _p->context.reset(new ViewLib::Context(this));

            // Parse the command line.
            if (!_p->context->commandLine(argc, argv))
            {
                QTimer::singleShot(0, this, SLOT(commandLineExit()));
            }
            else
            {
                QTimer::singleShot(0, this, SLOT(work()));
            }
        }

        Application::~Application()
        {
            //DJV_DEBUG("Application::~Application");
        }

        bool Application::event(QEvent * event)
        {
            switch (event->type())
            {
            case QEvent::FileOpen:
            {
                QFileOpenEvent * e = static_cast<QFileOpenEvent *>(event);
                QVector<QPointer<ViewLib::MainWindow> > mainWindowList = ViewLib::MainWindow::mainWindowList();
                if (mainWindowList.count())
                {
                    mainWindowList[0]->fileOpen(e->file());
                    mainWindowList[0]->raise();
                }
            } return true;
            default: break;
            }
            return QApplication::event(event);
        }

        void Application::commandLineExit()
        {
            exit(1);
        }

        void Application::work()
        {
            //DJV_DEBUG("Application::work");

            // Initialize user interface.
            DJV_LOG(_p->context->debugLog(), "djv::view::Application", "Initialize user interface...");
            _p->context->setValid(true);
            setWindowIcon(QPixmap(":projector32x32.png"));
            DJV_LOG(_p->context->debugLog(), "djv::view::Application", "");

            // Show main window(s).
            QStringList input = _p->context->input();
            if (input.count())
            {
                // Combine command line arguments.
                if (_p->context->hasCombine())
                {
                    Core::FileInfo fileInfo(input[0]);
                    if (fileInfo.isSequenceValid())
                    {
                        fileInfo.setType(Core::FileInfo::SEQUENCE);
                    }
                    for (int i = 1; i < input.count(); ++i)
                    {
                        fileInfo.addSequence(input[i]);
                    }
                    fileInfo.sortSequence();
                    input.clear();
                    input += fileInfo;
                }

                // Create and show a window for each input.
                for (int i = 0; i < input.count(); ++i)
                {
                    // Parse the input.
                    const Core::FileInfo fileInfo = Core::FileInfoUtil::parse(
                        input[i], _p->context->sequence(), _p->context->hasAutoSequence());
                    DJV_LOG(_p->context->debugLog(), "djv::view::Application",
                        QString("Input = \"%1\"").arg(fileInfo));

                    // Initialize the window.
                    ViewLib::MainWindow * window =
                        ViewLib::MainWindow::createWindow(_p->context.data());
                    window->fileOpen(fileInfo);
                    if (_p->context->fileLayer().data())
                    {
                        window->setFileLayer(*_p->context->fileLayer());
                    }
                    if (_p->context->playback().data())
                    {
                        window->setPlayback(*_p->context->playback());
                    }
                    if (_p->context->playbackFrame().data())
                    {
                        window->setPlaybackFrame(*_p->context->playbackFrame());
                    }
                    if (_p->context->playbackSpeed().data())
                    {
                        window->setPlaybackSpeed(*_p->context->playbackSpeed());
                    }
                    DJV_LOG(_p->context->debugLog(), "djv::view::Application",
                        "Show window...");
                    window->show();
                    DJV_LOG(_p->context->debugLog(), "djv::view::Application", "");
                }
            }
            else
            {
                // Create and show an empty window.
                DJV_LOG(_p->context->debugLog(), "djv::view::Application", "Show window...");
                ViewLib::MainWindow::createWindow(_p->context.data())->show();
                DJV_LOG(_p->context->debugLog(), "djv::view::Application", "");
            }
        }

    } // namespace view
} // namespace djv
