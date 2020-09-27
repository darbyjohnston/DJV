// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewSystem.h>

#include <djvUI/Enum.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the playback system.
        class PlaybackSystem : public IViewSystem
        {
            DJV_NON_COPYABLE(PlaybackSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            PlaybackSystem();

        public:
            ~PlaybackSystem() override;

            static std::shared_ptr<PlaybackSystem> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            MenuData getMenu() const override;

        protected:
            void _actionsUpdate();

            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            static float _getScrollWheelSpeed(UI::ScrollWheelSpeed);

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

