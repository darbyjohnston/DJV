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

#include <djvViewLib/PlaybackPrefs.h>

#include <djvUI/Prefs.h>

namespace djv
{
    namespace ViewLib
    {
        PlaybackPrefs::PlaybackPrefs(const QPointer<Context> & context, QObject * parent) :
            AbstractPrefs(context, parent),
            _autoStart(autoStartDefault()),
            _loop(loopDefault()),
            _everyFrame(everyFrameDefault()),
            _layout(layoutDefault())
        {
            UI::Prefs prefs("djv::ViewLib::PlaybackPrefs");
            prefs.get("autoStart", _autoStart);
            prefs.get("loop", _loop);
            prefs.get("everyFrame", _everyFrame);
            prefs.get("layout", _layout);
        }

        PlaybackPrefs::~PlaybackPrefs()
        {
            UI::Prefs prefs("PlaybackPrefs");
            prefs.set("autoStart", _autoStart);
            prefs.set("loop", _loop);
            prefs.set("everyFrame", _everyFrame);
            prefs.set("layout", _layout);
        }

        bool PlaybackPrefs::autoStartDefault()
        {
            return true;
        }

        bool PlaybackPrefs::hasAutoStart() const
        {
            return _autoStart;
        }

        Enum::LOOP PlaybackPrefs::loopDefault()
        {
            return Enum::LOOP_REPEAT;
        }

        Enum::LOOP PlaybackPrefs::loop() const
        {
            return _loop;
        }

        bool PlaybackPrefs::everyFrameDefault()
        {
            return true;
        }

        bool PlaybackPrefs::hasEveryFrame() const
        {
            return _everyFrame;
        }

        Enum::LAYOUT PlaybackPrefs::layoutDefault()
        {
            return Enum::LAYOUT_DEFAULT;
        }

        Enum::LAYOUT PlaybackPrefs::layout() const
        {
            return _layout;
        }

        void PlaybackPrefs::setAutoStart(bool in)
        {
            if (in == _autoStart)
                return;
            _autoStart = in;
            Q_EMIT autoStartChanged(_autoStart);
            Q_EMIT prefChanged();
        }

        void PlaybackPrefs::setLoop(Enum::LOOP in)
        {
            if (in == _loop)
                return;
            _loop = in;
            Q_EMIT loopChanged(_loop);
            Q_EMIT prefChanged();
        }

        void PlaybackPrefs::setEveryFrame(bool in)
        {
            if (in == _everyFrame)
                return;
            _everyFrame = in;
            Q_EMIT everyFrameChanged(_everyFrame);
            Q_EMIT prefChanged();
        }

        void PlaybackPrefs::setLayout(Enum::LAYOUT in)
        {
            if (in == _layout)
                return;
            _layout = in;
            Q_EMIT layoutChanged(_layout);
            Q_EMIT prefChanged();
        }

    } // namespace ViewLib
} // namespace djv
