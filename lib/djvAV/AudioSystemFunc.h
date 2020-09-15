// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AudioSystem.h>

#include <djvCore/Enum.h>

namespace djv
{
    namespace AV
    {
        namespace Audio
        {
            DJV_ENUM_HELPERS(DeviceFormat);

        } // namespace Audio
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Audio::DeviceFormat);

} // namespace djv
