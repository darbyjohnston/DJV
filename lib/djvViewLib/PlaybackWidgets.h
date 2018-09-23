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

#include <djvViewLib/Util.h>

#include <QWidget>

#include <memory>

class QActionGroup;

namespace djv
{
    namespace UI
    {
        class UIContext;
    
    } // namespace UI

    namespace ViewLib
    {
        class Context;

        //! \class PlaybackButtons
        //!
        //! This class provides a widget with playback control buttons.
        class PlaybackButtons : public QWidget
        {
            Q_OBJECT

        public:
            explicit PlaybackButtons(
                const QPointer<QActionGroup> &,
                const QPointer<UI::UIContext> &,
                QWidget * parent = nullptr);
            ~PlaybackButtons() override;

        Q_SIGNALS:
            //! This signal is emitted when the shuttle is pressed.
            void shuttlePressed(bool);

            //! This signal is emitted when the shuttle value is changed.
            void shuttleChanged(int);

        protected:
            bool event(QEvent *) override;

        private Q_SLOTS:
            void styleUpdate();
            
        private:
            DJV_PRIVATE_COPY(PlaybackButtons);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! \class LoopWidget
        //!
        //! This class provides a playback loop mode widget.
        class LoopWidget : public QWidget
        {
            Q_OBJECT

        public:
            explicit LoopWidget(
                const QPointer<QActionGroup> &,
                const QPointer<Context> &,
                QWidget * parent = nullptr);
            ~LoopWidget() override;

        private Q_SLOTS:
            void widgetUpdate();

        private:
            DJV_PRIVATE_COPY(LoopWidget);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! \class FrameButtons
        //!
        //! This class provides a widget with frame control buttons.
        class FrameButtons : public QWidget
        {
            Q_OBJECT

        public:
            explicit FrameButtons(
                const QPointer<QActionGroup> &,
                const QPointer<UI::UIContext> &,
                QWidget * parent = nullptr);
            ~FrameButtons() override;

        Q_SIGNALS:
            //! This signal is emitted when the shuttle is pressed.
            void shuttlePressed(bool);

            //! This signal is emitted when the shuttle value is changed.
            void shuttleChanged(int);

            //! This signal is emitted when the buttons are pressed.
            void pressed();

            //! This signal is emitted when the buttons are released.
            void released();

        protected:
            bool event(QEvent *) override;

        private Q_SLOTS:
            void styleUpdate();
            
        private:
            DJV_PRIVATE_COPY(FrameButtons);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
