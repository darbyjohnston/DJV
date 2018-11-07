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

#include <djvViewLib/MainWindow.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/SequencePrefs.h>

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
            QScopedPointer<ViewLib::ViewContext> context;
        };

        Application::Application(int & argc, char ** argv) :
            QApplication(argc, argv),
            _p(new Private)
        {
            //DJV_DEBUG("Application::Application");

            setOrganizationName("djv.sourceforge.net");
            setApplicationName("djv_view");

            // Create the context.
            _p->context.reset(new ViewLib::ViewContext(argc, argv, this));

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
                QVector<QPointer<ViewLib::Session> > sessionList = ViewLib::Session::sessionList();
                if (sessionList.count())
                {
                    sessionList[0]->fileOpen(e->file());
                    sessionList[0]->mainWindow()->raise();
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

        namespace
        {
            QPointer<ViewLib::Session> createSession(const Core::FileInfo & fileInfo, const QPointer<ViewLib::ViewContext> & context)
            {
                auto out = QPointer<ViewLib::Session>(new ViewLib::Session(nullptr, context));
                out->fileOpen(fileInfo);
                if (context->commandLineOptions().fileLayer.data())
                {
                    out->setFileLayer(*context->commandLineOptions().fileLayer);
                }
                if (context->commandLineOptions().fileProxy.data())
                {
                    out->setFileProxy(*context->commandLineOptions().fileProxy);
                }
                if (context->commandLineOptions().fileCacheEnable.data())
                {
                    out->setFileCacheEnabled(*context->commandLineOptions().fileCacheEnable);
                }
                if (context->commandLineOptions().windowFullScreen.data() && *context->commandLineOptions().windowFullScreen)
                {
                    out->showFullScreen();
                }
                if (context->commandLineOptions().playback.data())
                {
                    out->setPlayback(*context->commandLineOptions().playback);
                }
                if (context->commandLineOptions().playbackFrame.data())
                {
                    out->setPlaybackFrame(*context->commandLineOptions().playbackFrame);
                }
                if (context->commandLineOptions().playbackSpeed.data())
                {
                    out->setPlaybackSpeed(*context->commandLineOptions().playbackSpeed);
                }
                out->mainWindow()->show();
                return out;
            }

        } // namespace

        void Application::work()
        {
            //DJV_DEBUG("Application::work");

            // Initialize user interface.
            DJV_LOG(_p->context->debugLog(), "djv::view::Application", "Initialize user interface...");
            _p->context->setValid(true);
            setWindowIcon(QPixmap(":djv_view_256x256.png"));

            QStringList input = _p->context->commandLineOptions().input;
            if (input.count())
            {
                // Combine command line arguments.
                if (_p->context->commandLineOptions().combine)
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

                // Create and show a session for each input.
                for (int i = 0; i < input.count(); ++i)
                {
                    // Parse the input.
                    const Core::FileInfo fileInfo = Core::FileInfoUtil::parse(
                        input[i],
                        _p->context->sequencePrefs()->format(),
                        _p->context->sequencePrefs()->isAutoEnabled());
                    DJV_LOG(_p->context->debugLog(), "djv::view::Application", QString("Input = \"%1\"").arg(fileInfo));

                    // Create a session.
                    DJV_LOG(_p->context->debugLog(), "djv::view::Application", "Show window...");
                    createSession(fileInfo, _p->context.data());
                }
            }
            else
            {
                // Create a session.
                DJV_LOG(_p->context->debugLog(), "djv::view::Application", "Show window...");
                createSession(Core::FileInfo(), _p->context.data());
            }
        }

    } // namespace view
} // namespace djv
