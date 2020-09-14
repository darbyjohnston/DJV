// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MediaWidget.h>

#include <djvCore/FrameNumber.h>

namespace djv
{
    namespace ViewApp
    {
        class PointerWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(PointerWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            PointerWidget();

        public:
            static std::shared_ptr<PointerWidget> create(const std::shared_ptr<Core::Context>&);

            void setHoverCallback(const std::function<void(const PointerData&)>&);
            void setDragCallback(const std::function<void(const PointerData&)>&);
            void setScrollCallback(const std::function<void(const ScrollData&)>&);

        protected:
            void _pointerEnterEvent(Core::Event::PointerEnter&) override;
            void _pointerLeaveEvent(Core::Event::PointerLeave&) override;
            void _pointerMoveEvent(Core::Event::PointerMove&) override;
            void _buttonPressEvent(Core::Event::ButtonPress&) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;
            void _keyPressEvent(Core::Event::KeyPress&) override;
            void _keyReleaseEvent(Core::Event::KeyRelease&) override;
            void _scrollEvent(Core::Event::Scroll&) override;

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
                const std::shared_ptr<Core::Context>&);
            PlaybackSpeedWidget();

        public:
            ~PlaybackSpeedWidget() override;

            static std::shared_ptr<PlaybackSpeedWidget> create(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<Core::Context>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _textUpdate();
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        class FrameWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(FrameWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            FrameWidget();

        public:
            static std::shared_ptr<FrameWidget> create(const std::shared_ptr<Core::Context>&);

            void setSequence(const Core::Frame::Sequence&);
            void setSpeed(const Core::Math::Rational&);
            void setFrame(const Core::Frame::Index);
            void setCallback(const std::function<void(Core::Frame::Index)>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint&) override;

        private:
            void _setFrame(Core::Frame::Index);
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        class AudioWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(AudioWidget);

        protected:
            void _init(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<Core::Context>&);
            AudioWidget();

        public:
            static std::shared_ptr<AudioWidget> create(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<Core::Context>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

