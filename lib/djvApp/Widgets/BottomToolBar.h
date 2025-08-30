// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimeline/Player.h>

#include <feather-tk/ui/IWidget.h>

namespace djv
{
    namespace app
    {
        class App;
        class AudioActions;
        class FrameActions;
        class PlaybackActions;

        //! Bottom tool bar.
        class BottomToolBar : public ftk::IWidget
        {
            FTK_NON_COPYABLE(BottomToolBar);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<PlaybackActions>&,
                const std::shared_ptr<FrameActions>&,
                const std::shared_ptr<AudioActions>&,
                const std::shared_ptr<IWidget>& parent);

            BottomToolBar();

        public:
            ~BottomToolBar();

            static std::shared_ptr<BottomToolBar> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<PlaybackActions>&,
                const std::shared_ptr<FrameActions>&,
                const std::shared_ptr<AudioActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            //! Focus the current frame widget.
            void focusCurrentFrame();

            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;

        private:
            void _playerUpdate(const std::shared_ptr<tl::timeline::Player>&);
            void _showSpeedPopup();
            void _showAudioPopup();

            FTK_PRIVATE();
        };
    }
}
