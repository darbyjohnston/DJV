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

#include <djvViewLib/ViewLib.h>

#include <djvCore/Sequence.h>
#include <djvCore/Speed.h>
#include <djvCore/Util.h>

#include <QAbstractSpinBox>

#include <memory>

class QAbstractButton;

namespace djv
{
    namespace UI
    {
        class UIContext;
    
    } // namespace UI

    namespace ViewLib
    {
        //! This class provides a cache size widget.
        class CacheSizeWidget : public QWidget
        {
            Q_OBJECT

            //! This property holds the cache sizes in gigabytes.
            Q_PROPERTY(
                QVector<float> cacheSizesGB
                READ           cacheSizesGB
                WRITE          setCacheSizesGB
                NOTIFY         cacheSizesGBChanged)

            //! This property holds the cache size in gigabytes.
            Q_PROPERTY(
                float  cacheSizeGB
                READ   cacheSizeGB
                WRITE  setCacheSizeGB
                NOTIFY cacheSizeGBChanged)

        public:
            explicit CacheSizeWidget(const QPointer<UI::UIContext> &, QWidget * parent = nullptr);
            ~CacheSizeWidget() override;

            //! Get the cache sizes in gigabytes.
            const QVector<float> & cacheSizesGB() const;

            //! Get the current cache size in gigabytes.
            float cacheSizeGB() const;

        public Q_SLOTS:
            //! Set the cache sizes in gigabytes.
            void setCacheSizesGB(const QVector<float> &);

            //! Set the current cache size in gigabytes.
            void setCacheSizeGB(float);

        Q_SIGNALS:
            //! This signal is emitted when the cache sizes are changed.
            void cacheSizesGBChanged(const QVector<float> &);

            //! This signal is emitted when the current cache size is changed.
            void cacheSizeGBChanged(float);

        protected:
            bool event(QEvent *) override;

        private Q_SLOTS:
            void buttonCallback();
            void menuCallback();

            void styleUpdate();
            void widgetUpdate();

        private:
            DJV_PRIVATE_COPY(CacheSizeWidget);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides a frame number widget.
        class FrameWidget : public QAbstractSpinBox
        {
            Q_OBJECT

            //! This property holds the frame list.
            Q_PROPERTY(
                Core::FrameList frameList
                READ            frameList
                WRITE           setFrameList)

            //! This property holds the frame.
            Q_PROPERTY(
                qint64 frame
                READ   frame
                WRITE  setFrame
                NOTIFY frameChanged)

            //! This property holds the speed.
            Q_PROPERTY(
                Core::Speed speed
                READ        speed
                WRITE       setSpeed)

        public:
            explicit FrameWidget(const QPointer<UI::UIContext> &, QWidget * parent = nullptr);
            ~FrameWidget() override;

            //! Get the frame list.
            const Core::FrameList & frameList() const;

            //! Get the frame.
            qint64 frame() const;

            //! Get the speed.
            const Core::Speed & speed() const;

            void stepBy(int) override;

            QSize sizeHint() const override;

        public Q_SLOTS:
            //! Set the frame list.
            void setFrameList(const djv::Core::FrameList &);

            //! Set the frame.
            void setFrame(qint64);

            //! Set the speed.
            void setSpeed(const djv::Core::Speed &);

        Q_SIGNALS:
            //! This signal is emitted when the frame is changed.
            void frameChanged(qint64);

        protected:
            QAbstractSpinBox::StepEnabled stepEnabled() const override;
            bool event(QEvent *) override;

        private Q_SLOTS:
            void editingFinishedCallback();
            void timeUnitsCallback();

            void styleUpdate();
            void textUpdate();
            void widgetUpdate();

        private:
            DJV_PRIVATE_COPY(FrameWidget);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides a frame number slider.
        class FrameSlider : public QWidget
        {
            Q_OBJECT

            //! This property holds the frame list.
            Q_PROPERTY(
                djv::Core::FrameList frameList
                READ                frameList
                WRITE        setFrameList)

            //! This property holds the current frame.
            Q_PROPERTY(
                qint64 frame
                READ   frame
                WRITE  setFrame
                NOTIFY frameChanged)

            //! This property holds the speed.
            Q_PROPERTY(
                Core::Speed speed
                READ        speed
                WRITE       setSpeed)

            //! This property holds whether in/out points are enabled.
            Q_PROPERTY(
                bool   inOutEnabled
                READ   isInOutEnabled
                WRITE  setInOutEnabled
                NOTIFY inOutEnabledChanged)

            //! This property holds the in point.
            Q_PROPERTY(
                qint64 inPoint
                READ   inPoint
                WRITE  setInPoint
                NOTIFY inPointChanged)

            //! This property holds the out point.
            Q_PROPERTY(
                qint64 outPoint
                READ   outPoint
                WRITE  setOutPoint
                NOTIFY outPointChanged)

        public:
            explicit FrameSlider(const QPointer<UI::UIContext> &, QWidget * parent = nullptr);
            ~FrameSlider() override;

            //! Get the frame list.
            const Core::FrameList & frameList() const;

            //! Get the current frame.
            qint64 frame() const;

            //! Get the speed.
            const Core::Speed & speed() const;

            //! Get whether the in/out points are enabled.
            bool isInOutEnabled() const;

            //! Get the in point.
            qint64 inPoint() const;

            //! Get the out point.
            qint64 outPoint() const;

            QSize sizeHint() const override;

        public Q_SLOTS:
            //! Set the frame list.
            void setFrameList(const djv::Core::FrameList &);

            //! Set the current frame.
            void setFrame(qint64);

            //! Set the speed.
            void setSpeed(const djv::Core::Speed &);

            //! Set whether the in/out points are enabled.
            void setInOutEnabled(bool);

            //! Set the in/out points.
            void setInOutPoints(qint64, qint64);

            //! Set the in point.
            void setInPoint(qint64);

            //! Set the out point.
            void setOutPoint(qint64);

            //! Mark the in point. This sets the in point to the current frame.
            void markInPoint();

            //! Mark the out point. This sets the out point to the current frame.
            void markOutPoint();

            //! Reset the in/out points.
            void resetInOutPoints();

            //! Reset the in point.
            void resetInPoint();

            //! Reset the out point.
            void resetOutPoint();

            //! Go to the start frame.
            void gotoStartFrame();

            //! Go to the end frame.
            void gotoEndFrame();

            //! Set the list of cached frames.
            void setCachedFrames(const djv::Core::FrameList &);

        Q_SIGNALS:
            //! This signal is emitted when the frame is changed.
            void frameChanged(qint64);

            //! This signal is emitted when the in/out points are enabled or disabled.
            void inOutEnabledChanged(bool);

            //! This signal is emitted when the in point is changed.
            void inPointChanged(qint64);

            //! This signal is emitted when the out point is changed.
            void outPointChanged(qint64);

            //! This signal is emitted when the slider is pressed.
            void pressed(bool);

        protected:
            void mousePressEvent(QMouseEvent *) override;
            void mouseReleaseEvent(QMouseEvent *) override;
            void mouseMoveEvent(QMouseEvent *) override;
            void paintEvent(QPaintEvent *) override;
            bool event(QEvent *) override;

        private Q_SLOTS:
            void timeUnitsCallback();
            
            void styleUpdate();

        private:
            qint64 end() const;

            qint64 posToFrame(int) const;
            float frameToPosF(qint64) const;
            int frameToPos(qint64) const;

            DJV_PRIVATE_COPY(FrameSlider);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides a frame number display.
        class FrameDisplay : public QWidget
        {
            Q_OBJECT

            //! This property holds the frame.
            Q_PROPERTY(
                qint64 frame
                READ   frame
                WRITE  setFrame)

            //! This property holds the speed.
            Q_PROPERTY(
                Core::Speed speed
                READ        speed
                WRITE       setSpeed)

            //! This property holds whether in/out points are enabled.
            Q_PROPERTY(
                bool   inOutEnabled
                READ   isInOutEnabled
                WRITE  setInOutEnabled)

        public:
            explicit FrameDisplay(const QPointer<UI::UIContext> &, QWidget * parent = nullptr);
            ~FrameDisplay() override;

            //! Get the frame.
            qint64 frame() const;

            //! Get the speed.
            const Core::Speed & speed() const;

            //! Get whether the in/out points are enabled.
            bool isInOutEnabled() const;

            QSize sizeHint() const override;

        public Q_SLOTS:
            //! Set the frame.
            void setFrame(qint64);

            //! Set the speed.
            void setSpeed(const djv::Core::Speed &);

            //! Set whether in/out points are enabled.
            void setInOutEnabled(bool);

        protected:
            bool event(QEvent *) override;

        private Q_SLOTS:
            void timeUnitsCallback();
            
            void styleUpdate();
            void textUpdate();
            void widgetUpdate();

        private:
            DJV_PRIVATE_COPY(FrameDisplay);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides a speed button.
        class SpeedButton : public QWidget
        {
            Q_OBJECT

        public:
            explicit SpeedButton(const QPointer<UI::UIContext> &, QWidget * parent = nullptr);
            ~SpeedButton() override;

            //! Set the default speed.
            void setDefaultSpeed(const Core::Speed &);

        Q_SIGNALS:
            //! This signal is emitted when the speed is changed.
            void speedChanged(const djv::Core::Speed &);

        protected:
            bool event(QEvent *) override;

        private Q_SLOTS:
            void pressedCallback();
            void menuCallback();

            void styleUpdate();
            
        private:
            DJV_PRIVATE_COPY(SpeedButton);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides a speed widget.
        class SpeedWidget : public QWidget
        {
            Q_OBJECT

            //! This property holds the speed.
            Q_PROPERTY(
                Core::Speed speed
                READ        speed
                WRITE       setSpeed
                NOTIFY      speedChanged)

            //! This property holds the default speed.
            Q_PROPERTY(
                Core::Speed defaultSpeed
                READ        defaultSpeed
                WRITE       setDefaultSpeed)

        public:
            explicit SpeedWidget(const QPointer<UI::UIContext> &, QWidget * parent = nullptr);
            ~SpeedWidget() override;

            //! Get the speed.
            const Core::Speed & speed() const;

            //! Get the default speed.
            const Core::Speed & defaultSpeed() const;

        public Q_SLOTS:
            //! Set the speed.
            void setSpeed(const djv::Core::Speed &);

            //! Set the default speed.
            void setDefaultSpeed(const djv::Core::Speed &);

        Q_SIGNALS:
            //! This signal is emitted when the speed is changed.
            void speedChanged(const djv::Core::Speed &);

        protected:
            bool event(QEvent *) override;
            
        private Q_SLOTS:
            void editCallback(float);

            void styleUpdate();
            void widgetUpdate();

        private:
            DJV_PRIVATE_COPY(SpeedWidget);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides a speed display.
        class SpeedDisplay : public QWidget
        {
            Q_OBJECT

        public:
            explicit SpeedDisplay(const QPointer<UI::UIContext> &, QWidget * parent = nullptr);
            ~SpeedDisplay() override;

            QSize sizeHint() const override;

        public Q_SLOTS:
            //! Set the speed.
            void setSpeed(float);

            //! Set whether frames were dropped.
            void setDroppedFrames(bool);

        protected:
            bool event(QEvent *) override;
            
        private Q_SLOTS:
            void styleUpdate();
            void widgetUpdate();

        private:
            DJV_PRIVATE_COPY(SpeedDisplay);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
