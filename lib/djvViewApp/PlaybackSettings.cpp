// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/PlaybackSettings.h>

#include <djvViewApp/EnumFunc.h>

#include <djvSystem/Context.h>

#include <djvMath/RationalFunc.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PlaybackSettings::Private
        {
            std::shared_ptr<Observer::ValueSubject<bool> > startPlayback;
            std::shared_ptr<Observer::ValueSubject<PlaybackMode> > playbackMode;
            std::shared_ptr<Observer::ValueSubject<PlaybackSpeed> > playbackSpeed;
            std::shared_ptr<Observer::ValueSubject<Math::IntRational> > customSpeed;
            std::shared_ptr<Observer::ValueSubject<bool> > playEveryFrame;
            std::shared_ptr<Observer::ValueSubject<bool> > pipEnabled;
        };

        void PlaybackSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::PlaybackSettings", context);

            DJV_PRIVATE_PTR();
            p.startPlayback = Observer::ValueSubject<bool>::create(false);
            p.playbackSpeed = Observer::ValueSubject<PlaybackSpeed>::create(PlaybackSpeed::Default);
            p.customSpeed = Observer::ValueSubject<Math::IntRational>::create(Math::IntRational(1));
            p.playEveryFrame = Observer::ValueSubject<bool>::create(false);
            p.playbackMode = Observer::ValueSubject<PlaybackMode>::create(PlaybackMode::Loop);
            p.pipEnabled = Observer::ValueSubject<bool>::create(true);
            _load();
        }

        PlaybackSettings::PlaybackSettings() :
            _p(new Private)
        {}

        std::shared_ptr<PlaybackSettings> PlaybackSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<PlaybackSettings>(new PlaybackSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<bool> > PlaybackSettings::observeStartPlayback() const
        {
            return _p->startPlayback;
        }

        std::shared_ptr<Observer::IValueSubject<PlaybackMode> > PlaybackSettings::observePlaybackMode() const
        {
            return _p->playbackMode;
        }

        void PlaybackSettings::setStartPlayback(bool value)
        {
            _p->startPlayback->setIfChanged(value);
        }

        void PlaybackSettings::setPlaybackMode(PlaybackMode value)
        {
            _p->playbackMode->setIfChanged(value);
        }

        std::shared_ptr<Observer::IValueSubject<PlaybackSpeed> > PlaybackSettings::observePlaybackSpeed() const
        {
            return _p->playbackSpeed;
        }

        std::shared_ptr<Observer::IValueSubject<Math::IntRational> > PlaybackSettings::observeCustomSpeed() const
        {
            return _p->customSpeed;
        }

        std::shared_ptr<Observer::IValueSubject<bool> > PlaybackSettings::observePlayEveryFrame() const
        {
            return _p->playEveryFrame;
        }

        void PlaybackSettings::setPlaybackSpeed(PlaybackSpeed value)
        {
            _p->playbackSpeed->setIfChanged(value);
        }

        void PlaybackSettings::setCustomSpeed(const Math::IntRational& value)
        {
            _p->customSpeed->setIfChanged(value);
        }

        void PlaybackSettings::setPlayEveryFrame(bool value)
        {
            _p->playEveryFrame->setIfChanged(value);
        }

        std::shared_ptr<Observer::IValueSubject<bool> > PlaybackSettings::observePIPEnabled() const
        {
            return _p->pipEnabled;
        }

        void PlaybackSettings::setPIPEnabled(bool value)
        {
            _p->pipEnabled->setIfChanged(value);
        }

        void PlaybackSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("StartPlayback", value, p.startPlayback);
                UI::Settings::read("PlaybackSpeed", value, p.playbackSpeed);
                UI::Settings::read("CustomSpeed", value, p.customSpeed);
                UI::Settings::read("PlayEveryFrame", value, p.playEveryFrame);
                UI::Settings::read("PlaybackMode", value, p.playbackMode);
                UI::Settings::read("PIPEnabled", value, p.pipEnabled);
            }
        }

        rapidjson::Value PlaybackSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("StartPlayback", p.startPlayback->get(), out, allocator);
            UI::Settings::write("PlaybackSpeed", p.playbackSpeed->get(), out, allocator);
            UI::Settings::write("CustomSpeed", p.customSpeed->get(), out, allocator);
            UI::Settings::write("PlayEveryFrame", p.playEveryFrame->get(), out, allocator);
            UI::Settings::write("PlaybackMode", p.playbackMode->get(), out, allocator);
            UI::Settings::write("PIPEnabled", p.pipEnabled->get(), out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

