// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudio/TypeFunc.h>

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
            return channelCount * Audio::getByteCount(type);
        }

        inline bool Info::operator == (const Info& other) const
        {
            return
                name == other.name &&
                channelCount == other.channelCount &&
                type == other.type &&
                sampleRate == other.sampleRate &&
                codec == other.codec;
        }

        inline bool Info::operator != (const Info& other) const
        {
            return !(*this == other);
        }

    } // namespace Audio
} // namespace djv
