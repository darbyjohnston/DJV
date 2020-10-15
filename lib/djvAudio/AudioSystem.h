// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>

namespace djv
{
    namespace Audio
    {
        enum class DeviceFormat
        {
            S8,
            S16,
            S24,
            S32,
            F32,
            F64,

            Count,
            First = S8
        };

        struct Device
        {
            std::string                 name;
            uint8_t                     outputChannels      = 0;
            uint8_t                     inputChannels       = 0;
            uint8_t                     duplexChannels      = 0;
            std::vector<size_t>         sampleRates;
            size_t                      preferredSampleRate = 0;
            std::vector<DeviceFormat>   nativeFormats;
        };

        //! This class provides an audio system.
        class AudioSystem : public System::ISystem
        {
            DJV_NON_COPYABLE(AudioSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            AudioSystem();

        public:
            ~AudioSystem() override;

            static std::shared_ptr<AudioSystem> create(const std::shared_ptr<System::Context>&);

            const std::vector<std::string>& getAPIs() const;
            const std::vector<Device>& getDevices() const;
            
            unsigned int getDefaultInputDevice();
            unsigned int getDefaultOutputDevice();

        private:
            DJV_PRIVATE();
        };

    } // namespace Audio
} // namespace djv

