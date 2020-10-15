// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "FFmpegFunc.h"

#include <djvAudio/DataFunc.h>

extern "C"
{
#include <libavutil/rational.h>
#include <libavutil/samplefmt.h>

} // extern "C"

using namespace djv;

AVRational toFFmpeg(const Math::Rational& value)
{
    return av_make_q(value.getNum(), value.getDen());
}

Math::Rational fromFFmpeg(const AVRational& value)
{
    return Math::Rational(value.num, value.den);
}

Audio::Type fromFFmpeg(AVSampleFormat value)
{
    Audio::Type out = Audio::Type::None;
    switch (value)
    {
    case AV_SAMPLE_FMT_S16:  out = Audio::Type::S16; break;
    case AV_SAMPLE_FMT_S32:  out = Audio::Type::S32; break;
    case AV_SAMPLE_FMT_FLT:  out = Audio::Type::F32; break;
    case AV_SAMPLE_FMT_DBL:  out = Audio::Type::F64; break;
    case AV_SAMPLE_FMT_S16P: out = Audio::Type::S16; break;
    case AV_SAMPLE_FMT_S32P: out = Audio::Type::S32; break;
    case AV_SAMPLE_FMT_FLTP: out = Audio::Type::F32; break;
    case AV_SAMPLE_FMT_DBLP: out = Audio::Type::F64; break;
    default: break;
    }
    return out;
}

void extractAudio(
    uint8_t** inData,
    int inFormat,
    uint8_t inChannelCount,
    std::shared_ptr<Audio::Data> out)
{
    const uint8_t outChannelCount = out->getChannelCount();
    switch (inFormat)
    {
    case AV_SAMPLE_FMT_S16:
    {
        if (inChannelCount == outChannelCount)
        {
            memcpy(out->getData(), inData[0], out->getByteCount());
        }
        else
        {
            Audio::extract(
                reinterpret_cast<int16_t*>(inData[0]),
                reinterpret_cast<int16_t*>(out->getData()),
                out->getSampleCount(),
                inChannelCount,
                outChannelCount);
        }
        break;
    }
    case AV_SAMPLE_FMT_S32:
    {
        if (inChannelCount == outChannelCount)
        {
            memcpy(out->getData(), inData[0], out->getByteCount());
        }
        else
        {
            Audio::extract(
                reinterpret_cast<int32_t*>(inData[0]),
                reinterpret_cast<int32_t*>(out->getData()),
                out->getSampleCount(),
                inChannelCount,
                outChannelCount);
        }
        break;
    }
    case AV_SAMPLE_FMT_FLT:
    {
        if (inChannelCount == outChannelCount)
        {
            memcpy(out->getData(), inData[0], out->getByteCount());
        }
        else
        {
            Audio::extract(
                reinterpret_cast<float*>(inData[0]),
                reinterpret_cast<float*>(out->getData()),
                out->getSampleCount(),
                inChannelCount,
                outChannelCount);
        }
        break;
    }
    case AV_SAMPLE_FMT_DBL:
    {
        if (inChannelCount == outChannelCount)
        {
            memcpy(out->getData(), inData[0], out->getByteCount());
        }
        else
        {
            Audio::extract(
                reinterpret_cast<double*>(inData[0]),
                reinterpret_cast<double*>(out->getData()),
                out->getSampleCount(),
                inChannelCount,
                outChannelCount);
        }
        break;
    }
    case AV_SAMPLE_FMT_S16P:
    {
        const int16_t** c = new const int16_t * [outChannelCount];
        for (int i = 0; i < outChannelCount; ++i)
        {
            c[i] = reinterpret_cast<int16_t*>(inData[i]);
        }
        Audio::planarInterleave(
            c,
            reinterpret_cast<int16_t*>(out->getData()),
            outChannelCount,
            out->getSampleCount());
        delete[] c;
        break;
    }
    case AV_SAMPLE_FMT_S32P:
    {
        const int32_t** c = new const int32_t * [outChannelCount];
        for (int i = 0; i < outChannelCount; ++i)
        {
            c[i] = reinterpret_cast<int32_t*>(inData[i]);
        }
        Audio::planarInterleave(
            c,
            reinterpret_cast<int32_t*>(out->getData()),
            outChannelCount,
            out->getSampleCount());
        delete[] c;
        break;
    }
    case AV_SAMPLE_FMT_FLTP:
    {
        const float** c = new const float* [outChannelCount];
        for (int i = 0; i < outChannelCount; ++i)
        {
            c[i] = reinterpret_cast<float*>(inData[i]);
        }
        Audio::planarInterleave(
            c,
            reinterpret_cast<float*>(out->getData()),
            outChannelCount,
            out->getSampleCount());
        delete[] c;
        break;
    }
    case AV_SAMPLE_FMT_DBLP:
    {
        const double** c = new const double* [outChannelCount];
        for (int i = 0; i < outChannelCount; ++i)
        {
            c[i] = reinterpret_cast<double*>(inData[i]);
        }
        Audio::planarInterleave(
            c,
            reinterpret_cast<double*>(out->getData()),
            outChannelCount,
            out->getSampleCount());
        delete[] c;
        break;
    }
    default: break;
    }
}
