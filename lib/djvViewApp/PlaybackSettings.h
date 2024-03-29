// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/ISettings.h>

#include <djvMath/Rational.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        //! Playback settings.
        class PlaybackSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(PlaybackSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            PlaybackSettings();

        public:
            static std::shared_ptr<PlaybackSettings> create(const std::shared_ptr<System::Context>&);

            //! \name Playback
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeStartPlayback() const;
            std::shared_ptr<Core::Observer::IValueSubject<PlaybackMode> > observePlaybackMode() const;

            void setStartPlayback(bool);
            void setPlaybackMode(PlaybackMode);

            ///@}

            //! \name Speed
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<PlaybackSpeed> > observePlaybackSpeed() const;
            std::shared_ptr<Core::Observer::IValueSubject<Math::IntRational> > observeCustomSpeed() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observePlayEveryFrame() const;

            void setPlaybackSpeed(PlaybackSpeed);
            void setCustomSpeed(const Math::IntRational&);
            void setPlayEveryFrame(bool);

            ///@}

            //! \name Options
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observePIPEnabled() const;

            void setPIPEnabled(bool);

            ///@}

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

