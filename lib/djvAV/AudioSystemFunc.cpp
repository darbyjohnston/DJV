// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAV/AudioSystemFunc.h>

#include <array>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Audio
        {
            DJV_ENUM_HELPERS_IMPLEMENTATION(DeviceFormat);

        } // namespace Audio
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::Audio,
        DeviceFormat,
        DJV_TEXT("av_audio_device_format_s8"),
        DJV_TEXT("av_audio_device_format_s16"),
        DJV_TEXT("av_audio_device_format_s24"),
        DJV_TEXT("av_audio_device_format_s32"),
        DJV_TEXT("av_audio_device_format_f32"),
        DJV_TEXT("av_audio_device_format_f64"));

} // namespace djv

