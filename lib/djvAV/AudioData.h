// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Audio.h>

#include <memory>
#include <string>
#include <vector>

namespace djv
{
    namespace AV
    {
        namespace Audio
        {
            //! This constant provides the default name.
            const std::string defaultName = "Audio";

            //! This class provides audio data information.
            class Info
            {
            public:
                Info();
                Info(
                    uint8_t channelCount,
                    Type    type,
                    size_t  sampleRate,
                    size_t  sampleCount);

                std::string name            = defaultName;
                uint8_t     channelCount    = 0;
                Type        type            = Audio::Type::None;
                size_t      sampleRate      = 0;
                size_t      sampleCount     = 0;
                std::string codec;
                
                bool isValid() const;
                size_t getByteCount() const;

                bool operator == (const Info&) const;
                bool operator != (const Info&) const;
            };

            //! This class provides audio data.
            class Data
            {
                DJV_NON_COPYABLE(Data);

            protected:
                void _init(const Info&);
                Data();

            public:
                static std::shared_ptr<Data> create(const Info&);

                const Info& getInfo() const;
                uint8_t getChannelCount() const;
                Type getType() const;
                size_t getSampleRate() const;
                size_t getSampleCount() const;
                bool isValid() const;
                size_t getByteCount() const;

                uint8_t * getData();
                const uint8_t * getData() const;

                void zero();

                static std::shared_ptr<Data> convert(const std::shared_ptr<Data>&, Type);

                template<typename T>
                static void extract(
                    const T*,
                    T*,
                    size_t sampleCount,
                    uint8_t inChannelCount,
                    uint8_t outChannelCount);

                static std::shared_ptr<Data> planarInterleave(const std::shared_ptr<Data>&);
                template<typename T>
                static void planarInterleave(const T**, T*, uint8_t channelCount, size_t sampleCount);
                static std::shared_ptr<Data> planarDeinterleave(const std::shared_ptr<Data>&);

                static void volume(
                    const uint8_t*,
                    uint8_t*,
                    float volume,
                    size_t sampleCount,
                    uint8_t channelCount,
                    Type);

                bool operator == (const Data&) const;
                bool operator != (const Data&) const;

            private:
                Info _info;
                std::vector<uint8_t> _data;
            };

        } // namespace Audio
    } // namespace AV
} // namespace djv

#include <djvAV/AudioDataInline.h>
