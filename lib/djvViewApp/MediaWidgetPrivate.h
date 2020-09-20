// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MediaWidget.h>

#include <djvMath/FrameNumber.h>

namespace djv
{
    namespace Math
    {
        class Rational;
    
    } // namespace Math

    namespace ViewApp
    {
        class PointerWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(PointerWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            PointerWidget();

        public:
            static std::shared_ptr<PointerWidget> create(const std::shared_ptr<System::Context>&);

            void setHoverCallback(const std::function<void(const PointerData&)>&);
            void setDragCallback(const std::function<void(const PointerData&)>&);
            void setScrollCallback(const std::function<void(const ScrollData&)>&);

        protected:
            void _pointerEnterEvent(System::Event::PointerEnter&) override;
            void _pointerLeaveEvent(System::Event::PointerLeave&) override;
            void _pointerMoveEvent(System::Event::PointerMove&) override;
            void _buttonPressEvent(System::Event::ButtonPress&) override;
            void _buttonReleaseEvent(System::Event::ButtonRelease&) override;
            void _keyPressEvent(System::Event::KeyPress&) override;
            void _keyReleaseEvent(System::Event::KeyRelease&) override;
            void _scrollEvent(System::Event::Scroll&) override;

        private:
            void _doHoverCallback(const PointerData&);
            void _doDragCallback(const PointerData&);

            DJV_PRIVATE();
        };

        class PlaybackSpeedWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(PlaybackSpeedWidget);

        protected:
            void _init(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<System::Context>&);
            PlaybackSpeedWidget();

        public:
            ~PlaybackSpeedWidget() override;

            static std::shared_ptr<PlaybackSpeedWidget> create(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _textUpdate();
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        class FrameWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(FrameWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            FrameWidget();

        public:
            static std::shared_ptr<FrameWidget> create(const std::shared_ptr<System::Context>&);

            void setSequence(const Math::Frame::Sequence&);
            void setSpeed(const Math::Rational&);
            void setFrame(const Math::Frame::Index);
            void setCallback(const std::function<void(Math::Frame::Index)>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;
            void _paintEvent(System::Event::Paint&) override;

        private:
            void _setFrame(Math::Frame::Index);
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        class AudioWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(AudioWidget);

        protected:
            void _init(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<System::Context>&);
            AudioWidget();

        public:
            static std::shared_ptr<AudioWidget> create(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

