// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudio/Data.h>

namespace djv
{
    namespace Audio
    {
        void Data::_init(const Info& info, size_t sampleCount)
        {
            _info = info;
            _sampleCount = sampleCount;
            _data.resize(getByteCount());
        }

        Data::Data()
        {}

        std::shared_ptr<Data> Data::create(const Info& info, size_t sampleCount)
        {
            auto out = std::shared_ptr<Data>(new Data);
            out->_init(info, sampleCount);
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
                _sampleCount == other._sampleCount &&
                0 == memcmp(getData(), other.getData(), getByteCount());
        }

        bool Data::operator != (const Data& other) const
        {
            return !(*this == other);
        }

    } // namespace Audio
} // namespace djv

