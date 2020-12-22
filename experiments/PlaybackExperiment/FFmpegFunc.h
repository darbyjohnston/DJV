// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "IO.h"

extern "C"
{
#include <libavutil/samplefmt.h>

} // extern "C"

struct AVRational;

namespace IO
{
    AVRational toFFmpeg(const djv::Math::IntRational& value);

    djv::Math::IntRational fromFFmpeg(const AVRational& value);

    djv::Audio::Type fromFFmpeg(AVSampleFormat value);

    void extractAudio(
        uint8_t** inData,
        int inFormat,
        uint8_t inChannelCount,
        std::shared_ptr<djv::Audio::Data>);

} // namespace IO
