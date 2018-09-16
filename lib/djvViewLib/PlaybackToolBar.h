//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvViewLib/AbstractToolBar.h>
#include <djvViewLib/Util.h>

#include <djvCore/Sequence.h>

#include <memory>

namespace djv
{
    namespace ViewLib
    {
        class PlaybackActions;

        //! \class PlaybackToolBar
        //!
        //! This class provides the playback group tool bar.
        class PlaybackToolBar : public AbstractToolBar
        {
            Q_OBJECT

        public:
            explicit PlaybackToolBar(
                AbstractActions * actions,
                Context *         context,
                QWidget *         parent = nullptr);

            virtual ~PlaybackToolBar();

        public Q_SLOTS:
            //! Set the playback speed.
            void setSpeed(const djv::Core::Speed &);

            //! Set the default playback speed.
            void setDefaultSpeed(const djv::Core::Speed &);

            //! Set the real playback speed.
            void setRealSpeed(float);

            //! Set whether frames are being dropped.
            void setDroppedFrames(bool);

            //! Set the frame list.
            void setFrameList(const djv::Core::FrameList &);

            //! Set the frame.
            void setFrame(qint64);

            //! Set the cached frames.
            void setCachedFrames(const djv::Core::FrameList &);

            //! Set the start frame.
            void setStart(qint64);

            //! Set the end frame.
            void setEnd(qint64);

            //! Set the duration.
            void setDuration(qint64, bool inOutEnabled);

            //! Set whether the in/out points are enabled.
            void setInOutEnabled(bool);

            //! Set the in point.
            void setInPoint(qint64);

            //! Set the out point.
            void setOutPoint(qint64);

            //! Mark the in point.
            void markInPoint();

            //! Mark the out point.
            void markOutPoint();

            //! Reset the in point.
            void resetInPoint();

            //! Reset the out point.
            void resetOutPoint();

            //! Set the layout.
            void setLayout(djv::ViewLib::Util::LAYOUT);

        Q_SIGNALS:
            //! This signal is emitted when the playback shuttle is pressed.
            void playbackShuttlePressed(bool);

            //! This signal is emitted when the playback shuttle is changed.
            void playbackShuttleValue(int);

            //! This signal is emitted when the speed is changed.
            void speedChanged(const djv::Core::Speed &);

            //! This signal is emitted when the frame is changed.
            void frameChanged(qint64);

            //! This signal is emitted when the frame shuttle is pressed.
            void frameShuttlePressed(bool);

            //! This signal is emitted when the frame shuttle is changed.
            void frameShuttleValue(int);

            //! This signal is emitted when the frame slider is pressed.
            void frameSliderPressed(bool);

            //! This signal is emitted when the frame slider is changed.
            void frameSliderChanged(qint64);

            //! This signal is emitted when the frame buttons are pressed.
            void frameButtonsPressed();

            //! This signal is emitted when the frame buttons are released.
            void frameButtonsReleased();

            //! This signal is emitted when the in point is changed.
            void inPointChanged(qint64);

            //! This signal is emitted when the out point is changed.
            void outPointChanged(qint64);

        private:
            void layoutUpdate();

            DJV_PRIVATE_COPY(PlaybackToolBar);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
