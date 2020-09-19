// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAudio/AudioSystem.h>

#include <djvCore/Enum.h>

namespace djv
{
    namespace Audio
    {
        DJV_ENUM_HELPERS(DeviceFormat);

    } // namespace Audio

    DJV_ENUM_SERIALIZE_HELPERS(Audio::DeviceFormat);

} // namespace djv
