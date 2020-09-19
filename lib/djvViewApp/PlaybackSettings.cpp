// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/PlaybackSettings.h>

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
            std::shared_ptr<ValueSubject<bool> > startPlayback;
            std::shared_ptr<ValueSubject<PlaybackSpeed> > playbackSpeed;
            std::shared_ptr<ValueSubject<Math::Rational> > customSpeed;
            std::shared_ptr<ValueSubject<bool> > playEveryFrame;
            std::shared_ptr<ValueSubject<PlaybackMode> > playbackMode;
            std::shared_ptr<ValueSubject<bool> > pipEnabled;
        };

        void PlaybackSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::PlaybackSettings", context);

            DJV_PRIVATE_PTR();
            p.startPlayback = ValueSubject<bool>::create(false);
            p.playbackSpeed = ValueSubject<PlaybackSpeed>::create(PlaybackSpeed::Default);
            p.customSpeed = ValueSubject<Math::Rational>::create(Math::Rational(1));
            p.playEveryFrame = ValueSubject<bool>::create(false);
            p.playbackMode = ValueSubject<PlaybackMode>::create(PlaybackMode::Loop);
            p.pipEnabled = ValueSubject<bool>::create(true);
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

        std::shared_ptr<IValueSubject<bool> > PlaybackSettings::observeStartPlayback() const
        {
            return _p->startPlayback;
        }

        void PlaybackSettings::setStartPlayback(bool value)
        {
            _p->startPlayback->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<PlaybackSpeed> > PlaybackSettings::observePlaybackSpeed() const
        {
            return _p->playbackSpeed;
        }

        void PlaybackSettings::setPlaybackSpeed(PlaybackSpeed value)
        {
            _p->playbackSpeed->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<Math::Rational> > PlaybackSettings::observeCustomSpeed() const
        {
            return _p->customSpeed;
        }

        void PlaybackSettings::setCustomSpeed(const Math::Rational& value)
        {
            _p->customSpeed->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > PlaybackSettings::observePlayEveryFrame() const
        {
            return _p->playEveryFrame;
        }

        void PlaybackSettings::setPlayEveryFrame(bool value)
        {
            _p->playEveryFrame->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<PlaybackMode> > PlaybackSettings::observePlaybackMode() const
        {
            return _p->playbackMode;
        }

        void PlaybackSettings::setPlaybackMode(PlaybackMode value)
        {
            _p->playbackMode->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > PlaybackSettings::observePIPEnabled() const
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

