// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Models/AudioModel.h>

#include <tlCore/AudioSystem.h>

#include <feather-tk/ui/Settings.h>
#include <feather-tk/core/Context.h>
#include <feather-tk/core/Math.h>

namespace djv
{
    namespace app
    {
        struct AudioModel::Private
        {
            std::shared_ptr<feather_tk::Settings> settings;
            std::shared_ptr<feather_tk::ObservableList<tl::audio::DeviceID> > devices;
            std::shared_ptr<feather_tk::ObservableValue<tl::audio::DeviceID> > device;
            std::shared_ptr<feather_tk::ObservableValue<float> > volume;
            std::shared_ptr<feather_tk::ObservableValue<bool> > mute;
            std::shared_ptr<feather_tk::ObservableList<bool> > channelMute;
            std::shared_ptr<feather_tk::ObservableValue<double> > syncOffset;
            std::shared_ptr<feather_tk::ListObserver<tl::audio::DeviceInfo> > devicesObserver;
        };

        void AudioModel::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<feather_tk::Settings>& settings)
        {
            FEATHER_TK_P();

            p.settings = settings;

            p.devices = feather_tk::ObservableList<tl::audio::DeviceID>::create();
            p.device = feather_tk::ObservableValue<tl::audio::DeviceID>::create();

            float volume = 1.F;
            p.settings->get("/Audio/Volume", volume);
            p.volume = feather_tk::ObservableValue<float>::create(volume);

            bool mute = false;
            p.settings->get("/Audio/Mute", mute);
            p.mute = feather_tk::ObservableValue<bool>::create(mute);

            p.channelMute = feather_tk::ObservableList<bool>::create();

            p.syncOffset = feather_tk::ObservableValue<double>::create(0.0);

            auto audioSystem = context->getSystem<tl::audio::System>();
            p.devicesObserver = feather_tk::ListObserver<tl::audio::DeviceInfo>::create(
                audioSystem->observeDevices(),
                [this](const std::vector<tl::audio::DeviceInfo>& devices)
                {
                    std::vector<tl::audio::DeviceID> ids;
                    for (const auto& device : devices)
                    {
                        ids.push_back(device.id);
                    }
                    _p->devices->setIfChanged(ids);
                });
        }

        AudioModel::AudioModel() :
            _p(new Private)
        {}

        AudioModel::~AudioModel()
        {
            FEATHER_TK_P();
            p.settings->set("/Audio/Volume", p.volume->get());
            p.settings->set("/Audio/Mute", p.mute->get());
        }

        std::shared_ptr<AudioModel> AudioModel::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<feather_tk::Settings>& settings)
        {
            auto out = std::shared_ptr<AudioModel>(new AudioModel);
            out->_init(context, settings);
            return out;
        }

        const std::vector<tl::audio::DeviceID>& AudioModel::getDevices()
        {
            return _p->devices->get();
        }

        std::shared_ptr<feather_tk::IObservableList<tl::audio::DeviceID> > AudioModel::observeDevices() const
        {
            return _p->devices;
        }

        const tl::audio::DeviceID& AudioModel::getDevice() const
        {
            return _p->device->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<tl::audio::DeviceID> > AudioModel::observeDevice() const
        {
            return _p->device;
        }

        void AudioModel::setDevice(const tl::audio::DeviceID& value)
        {
            _p->device->setIfChanged(value);
        }

        float AudioModel::getVolume() const
        {
            return _p->volume->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<float> > AudioModel::observeVolume() const
        {
            return _p->volume;
        }

        void AudioModel::setVolume(float value)
        {
            const float tmp = feather_tk::clamp(value, 0.F, 1.F);
            _p->volume->setIfChanged(tmp);
        }

        void AudioModel::volumeUp()
        {
            setVolume(_p->volume->get() + .1F);
        }

        void AudioModel::volumeDown()
        {
            setVolume(_p->volume->get() - .1F);
        }

        bool AudioModel::isMuted() const
        {
            return _p->mute->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<bool> > AudioModel::observeMute() const
        {
            return _p->mute;
        }

        void AudioModel::setMute(bool value)
        {
            _p->mute->setIfChanged(value);
        }

        const std::vector<bool>& AudioModel::getChannelMute() const
        {
            return _p->channelMute->get();
        }

        std::shared_ptr<feather_tk::IObservableList<bool> > AudioModel::observeChannelMute() const
        {
            return _p->channelMute;
        }

        void AudioModel::setChannelMute(const std::vector<bool>& value)
        {
            _p->channelMute->setIfChanged(value);
        }

        double AudioModel::getSyncOffset() const
        {
            return _p->syncOffset->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<double> > AudioModel::observeSyncOffset() const
        {
            return _p->syncOffset;
        }

        void AudioModel::setSyncOffset(double value)
        {
            _p->syncOffset->setIfChanged(value);
        }
    }
}
