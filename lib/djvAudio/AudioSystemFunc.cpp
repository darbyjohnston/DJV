// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAudio/AudioSystemFunc.h>

#include <array>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace Audio
    {
        DJV_ENUM_HELPERS_IMPLEMENTATION(DeviceFormat);

    } // namespace Audio

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Audio,
        DeviceFormat,
        DJV_TEXT("audio_device_format_s8"),
        DJV_TEXT("audio_device_format_s16"),
        DJV_TEXT("audio_device_format_s24"),
        DJV_TEXT("audio_device_format_s32"),
        DJV_TEXT("audio_device_format_f32"),
        DJV_TEXT("audio_device_format_f64"));

} // namespace djv

