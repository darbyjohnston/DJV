// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Audio
    {
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
            return _sampleCount;
        }

        inline bool Data::isValid() const
        {
            return _info.isValid();
        }

        inline size_t Data::getByteCount() const
        {
            return _info.getByteCount() * _sampleCount;
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
