// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudio/AudioFunc.h>

#include <cstring>

namespace djv
{
    namespace Audio
    {
        inline bool Info::isValid() const
        {
            return type != Type::None;
        }

        inline size_t Info::getByteCount() const
        {
            return sampleCount * channelCount * Audio::getByteCount(type);
        }

        inline bool Info::operator == (const Info& other) const
        {
            return
                name == other.name &&
                channelCount == other.channelCount &&
                type == other.type &&
                sampleRate == other.sampleRate &&
                sampleCount == other.sampleCount &&
                codec == other.codec;
        }

        inline bool Info::operator != (const Info& other) const
        {
            return !(*this == other);
        }

        inline const Info& Data::getInfo() const
        {
            return _info;
        }
        
        inline uint8_t Data::getChannelCount() const
        {
            return _info.channelCount;
        }

        inline Type Data::getType() const
        {
            return _info.type;
        }

        inline size_t Data::getSampleRate() const
        {
            return _info.sampleRate;
        }

        inline size_t Data::getSampleCount() const
        {
            return _info.sampleCount;
        }

        inline bool Data::isValid() const
        {
            return _info.isValid();
        }

        inline size_t Data::getByteCount() const
        {
            return _info.getByteCount();
        }

        inline uint8_t* Data::getData()
        {
            return _data.data();
        }

        inline const uint8_t* Data::getData() const
        {
            return _data.data();
        }

    } // namespace Audio
} // namespace djv
