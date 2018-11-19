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

#include <Context.h>

#include <PlaybackSystem.h>
#include <IO.h>

#include <djvUI/FileBrowser.h>
#include <djvUI/IconLibrary.h>

#include <djvCore/DebugLog.h>

using namespace djv;

namespace djv
{
    namespace AudioExperiment2
    {
        Context::Context(int & argc, char ** argv, QObject * parent) :
            UIContext(argc, argv, parent)
        {
            _ioQueue = std::shared_ptr<Util::AVQueue>(new Util::AVQueue);

            if (argc > 1)
            {
                _io.reset(new IO(QString(argv[1]), _ioQueue, this));
                _ioInfoFuture = _io->ioInfo();
                _ioInfoTimer = startTimer(100);
            }

            _playbackSystem.reset(new PlaybackSystem(_ioQueue, this));

            _actions["File/Open"] = new QAction(iconLibrary()->icon("djv/UI/FileOpenIcon"), "Open", this);
            _actions["File/Open"]->setShortcut(QKeySequence("Ctrl+O"));
            _actions["File/Close"] = new QAction(iconLibrary()->icon("djv/UI/FileCloseIcon"), "Close", this);
            _actions["File/Close"]->setShortcut(QKeySequence("Ctrl+E"));
            _actions["Playback/Stop"] = new QAction(iconLibrary()->icon("djv/UI/PlayStopIcon"), "Stop", this);
            _actions["Playback/Stop"]->setCheckable(true);
            _actions["Playback/Stop"]->setShortcut(QKeySequence("K"));
            _actions["Playback/Forward"] = new QAction(iconLibrary()->icon("djv/UI/PlayForwardIcon"), "Forward", this);
            _actions["Playback/Forward"]->setCheckable(true);
            _actions["Playback/Forward"]->setShortcut(QKeySequence("L"));

            Q_FOREACH(const QString & message, debugLog()->messages())
            {
                printMessage(message);
            }

            connect(
                _playbackSystem.data(),
                &PlaybackSystem::currentTimeChanged,
                [this](int64_t value)
            {
                if (Util::PLAYBACK_STOP == _playbackSystem->playback() && _io)
                {
                    _io->seek(value);
                }
            });
            connect(
                _playbackSystem.data(),
                &PlaybackSystem::playbackChanged,
                [this](Util::PLAYBACK value)
            {
                if (Util::PLAYBACK_STOP == value && _io)
                {
                    //! \todo
                    _io->seek(_playbackSystem->currentTime());
                }
            });
            connect(
                _actions["File/Open"].data(),
                &QAction::triggered,
                [this]
            {
                auto fileBrowser = this->fileBrowser("Open");
                connect(
                    fileBrowser,
                    &UI::FileBrowser::fileInfoChanged,
                    [this](const Core::FileInfo & value)
                {
                    _io.reset(new IO(value, _ioQueue, this));
                    _ioInfoFuture = _io->ioInfo();
                    _ioInfoTimer = startTimer(100);
                });
                fileBrowser->show();
                fileBrowser->raise();
                fileBrowser->activateWindow();
            });
            connect(
                _actions["File/Close"].data(),
                &QAction::triggered,
                [this]
            {
                _io.reset();
            });
            connect(
                _actions["Playback/Stop"].data(),
                &QAction::toggled,
                [this](bool value)
            {
                if (value)
                {
                    _playbackSystem->setPlayback(Util::PLAYBACK_STOP);
                }
            });
            connect(
                _actions["Playback/Forward"].data(),
                &QAction::toggled,
                [this](bool value)
            {
                if (value)
                {
                    _playbackSystem->setPlayback(Util::PLAYBACK_FORWARD);
                }
            });

            connect(
                debugLog(),
                &Core::DebugLog::message,
                [this](const QString & value)
            {
                printMessage(value);
            });
        }

        Context::~Context()
        {}

        std::shared_ptr<Util::AVQueue> Context::ioQueue() const
        {
            return _ioQueue;
        }

        QPointer<PlaybackSystem> Context::playbackSystem() const
        {
            return _playbackSystem.data();
        }

        const QMap<QString, QPointer<QAction> > & Context::actions() const
        {
            return _actions;
        }

        void Context::timerEvent(QTimerEvent *)
        {
            if (_ioInfoFuture.valid())
            {
                const auto info = _ioInfoFuture.get();
                _playbackSystem->setPlayback(Util::PLAYBACK_STOP);
                _playbackSystem->setInfo(info.audio);
                _playbackSystem->setDuration(info.video.duration);
                _playbackSystem->setCurrentTime(0.f);
                killTimer(_ioInfoTimer);
                _ioInfoTimer = 0;
            }
        }

    } // namespace AudioExperiment2
} // namespace djv
