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

#pragma once

#include <djvViewLib/AbstractPrefs.h>
#include <djvViewLib/Enum.h>

namespace djv
{
    namespace ViewLib
    {
        //! This class provides the playback group preferences.
        class PlaybackPrefs : public AbstractPrefs
        {
            Q_OBJECT

        public:
            explicit PlaybackPrefs(const QPointer<Context> &, QObject * parent = nullptr);
            ~PlaybackPrefs() override;

            //! Get the default for whether playback is started automatically.
            static bool autoStartDefault();

            //! Get whether playback is started automatically.
            bool hasAutoStart() const;

            //! Get the default playback loop.
            static Enum::LOOP loopDefault();

            //! Get the playback loop.
            Enum::LOOP loop() const;

            //! Get the default for whether every frame is played.
            static bool everyFrameDefault();

            //! Get whether every frame is played.
            bool hasEveryFrame() const;

            //! Get the default playback layout.
            static Enum::LAYOUT layoutDefault();

            //! Get the playback layout.
            Enum::LAYOUT layout() const;

        public Q_SLOTS:
            //! Set whether playback is started automatically.
            void setAutoStart(bool);

            //! Set the playback loop.
            void setLoop(djv::ViewLib::Enum::LOOP);

            //! Set whether every frame is played.
            void setEveryFrame(bool);

            //! Set the playback layout.
            void setLayout(djv::ViewLib::Enum::LAYOUT);

        Q_SIGNALS:
            //! This signal is emitted when playback auto start is changed.
            void autoStartChanged(bool);

            //! This signal is emitted when the playback loop is changed.
            void loopChanged(djv::ViewLib::Enum::LOOP);

            //! This signal is emitted when every frame played is changed.
            void everyFrameChanged(bool);

            //! This signal is emitted when the playback layout is changed.
            void layoutChanged(djv::ViewLib::Enum::LAYOUT);

        private:
            bool          _autoStart;
            Enum::LOOP   _loop;
            bool         _everyFrame;
            Enum::LAYOUT _layout;
        };

    } // namespace ViewLib
} // namespace djv
