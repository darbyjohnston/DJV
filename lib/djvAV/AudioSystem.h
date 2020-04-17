// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Audio.h>

#include <djvCore/ISystem.h>

namespace djv
{
    namespace AV
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
            DJV_ENUM_HELPERS(DeviceFormat);

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
            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                System();

            public:
                ~System() override;

                static std::shared_ptr<System> create(const std::shared_ptr<Core::Context>&);

                const std::vector<std::string>& getAPIs() const;
                const std::vector<Device>& getDevices() const;

            private:
                DJV_PRIVATE();
            };

        } // namespace Audio
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Audio::DeviceFormat);

} // namespace djv
