// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "IO.h"

enum AVSampleFormat;
struct AVRational;

AVRational toFFmpeg(const djv::Math::Rational& value);

djv::Math::Rational fromFFmpeg(const AVRational& value);

djv::Audio::Type fromFFmpeg(AVSampleFormat value);

void extractAudio(
    uint8_t** inData,
    int inFormat,
    uint8_t inChannelCount,
    std::shared_ptr<djv::Audio::Data>);
