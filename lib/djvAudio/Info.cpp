// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudio/Info.h>

namespace djv
{
    namespace Audio
    {
        Info::Info()
        {}

        Info::Info(uint8_t channelCount, Type type, size_t sampleRate) :
            channelCount(channelCount),
            type(type),
            sampleRate(sampleRate)
        {}

    } // namespace Audio
} // namespace djv

