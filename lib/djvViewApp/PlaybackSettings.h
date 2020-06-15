// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/ISettings.h>

#include <djvCore/ValueObserver.h>

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
        //! This class provides the playback settings.
        class PlaybackSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(PlaybackSettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);

            PlaybackSettings();

        public:
            static std::shared_ptr<PlaybackSettings> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::IValueSubject<bool> > observeStartPlayback() const;
            void setStartPlayback(bool);

            std::shared_ptr<Core::IValueSubject<PlaybackSpeed> > observePlaybackSpeed() const;
            void setPlaybackSpeed(PlaybackSpeed);

            std::shared_ptr<Core::IValueSubject<Core::Math::Rational> > observeCustomSpeed() const;
            void setCustomSpeed(const Core::Math::Rational&);

            std::shared_ptr<Core::IValueSubject<bool> > observePlayEveryFrame() const;
            void setPlayEveryFrame(bool);

            std::shared_ptr<Core::IValueSubject<PlaybackMode> > observePlaybackMode() const;
            void setPlaybackMode(PlaybackMode);

            std::shared_ptr<Core::IValueSubject<bool> > observePIPEnabled() const;
            void setPIPEnabled(bool);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

