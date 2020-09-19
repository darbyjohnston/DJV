// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/TimelineWidget.h>

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

namespace djv
{
    namespace ViewApp
    {
        class SpeedWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(SpeedWidget);

        protected:
            void _init(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<System::Context>&);
            SpeedWidget();

        public:
            ~SpeedWidget() override;

            static std::shared_ptr<SpeedWidget> create(
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

