// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/Widget.h>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            class Speed;

        } // namespace Time
    } // namespace Core

    namespace ViewApp
    {

        //! This class provides a playback speed widget.
        class PlaybackSpeedWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(PlaybackSpeedWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            PlaybackSpeedWidget();

        public:
            ~PlaybackSpeedWidget() override;

            static std::shared_ptr<PlaybackSpeedWidget> create(const std::shared_ptr<Core::Context>&);

            void setPlaybackSpeed(PlaybackSpeed);
            void setDefaultSpeed(const Core::Math::Rational&);
            void setCustomSpeed(const Core::Math::Rational&);
            void setSpeed(const Core::Math::Rational&);
            void setPlayEveryFrame(bool);
            
            void setPlaybackSpeedCallback(const std::function<void(PlaybackSpeed)>&);
            void setCustomSpeedCallback(const std::function<void(const Core::Math::Rational&)>&);
            void setPlayEveryFrameCallback(const std::function<void(bool)>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            
            void _initEvent(Core::Event::Init &) override;

        private:
            void _textUpdate();
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

