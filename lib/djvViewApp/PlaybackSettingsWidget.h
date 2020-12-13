// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SettingsIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the playback settings widget.
        class PlaybackSettingsWidget : public UIComponents::Settings::IWidget
        {
            DJV_NON_COPYABLE(PlaybackSettingsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            PlaybackSettingsWidget();

        public:
            static std::shared_ptr<PlaybackSettingsWidget> create(const std::shared_ptr<System::Context>&);

            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        protected:
            void _initEvent(System::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

        //! This class provides the timeline settings widget.
        class TimelineSettingsWidget : public UIComponents::Settings::IWidget
        {
            DJV_NON_COPYABLE(TimelineSettingsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            TimelineSettingsWidget();

        public:
            static std::shared_ptr<TimelineSettingsWidget> create(const std::shared_ptr<System::Context>&);

            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        protected:
            void _initEvent(System::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

