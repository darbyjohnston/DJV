// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudio/AudioData.h>

namespace djv
{
    namespace Audio
    {
        Info::Info()
        {}

        Info::Info(uint8_t channelCount, Type type, size_t sampleRate, size_t sampleCount) :
            channelCount(channelCount),
            type(type),
            sampleRate(sampleRate),
            sampleCount(sampleCount)
        {}

        void Data::_init(const Info& info)
        {
            _info = info;
            _data.resize(getByteCount());
        }

        Data::Data()
        {}

        std::shared_ptr<Data> Data::create(const Info& info)
        {
            auto out = std::shared_ptr<Data>(new Data);
            out->_init(info);
            return out;
        }
        
        void Data::zero()
        {
            memset(_data.data(), 0, getByteCount());
        }

        bool Data::operator == (const Data& other) const
        {
            return
                _info == other._info &&
                0 == memcmp(getData(), other.getData(), getByteCount());
        }

        bool Data::operator != (const Data& other) const
        {
            return !(*this == other);
        }

    } // namespace Audio
} // namespace djv

