// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/TimelineWidget.h>

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
                const std::shared_ptr<Core::Context>&);
            SpeedWidget();

        public:
            ~SpeedWidget() override;

            static std::shared_ptr<SpeedWidget> create(
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

