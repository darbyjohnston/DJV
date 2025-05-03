// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Actions/IActions.h>

namespace djv
{
    namespace app
    {
        //! Playback actions.
        class PlaybackActions : public IActions
        {
            DTK_NON_COPYABLE(PlaybackActions);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&);

            PlaybackActions();

        public:
            ~PlaybackActions();

            static std::shared_ptr<PlaybackActions> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&);

        private:
            void _setPlayer(const std::shared_ptr<tl::timeline::Player>&);
            void _playbackUpdate();
            void _loopUpdate();

            tl::timeline::Playback _playbackPrev = tl::timeline::Playback::Forward;

            DTK_PRIVATE();
        };
    }
}
