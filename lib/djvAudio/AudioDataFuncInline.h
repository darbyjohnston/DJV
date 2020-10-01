// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudio/Audio.h>

#include <memory>

namespace djv
{
    namespace Audio
    {
        template<typename T>
        inline void extract(
            const T* value,
            T* out,
            size_t sampleCount,
            uint8_t inChannelCount,
            uint8_t outChannelCount)
        {
            const T* inP = value;
            T* outP = out;
            T* const endP = outP + sampleCount * outChannelCount;
            switch (outChannelCount)
            {
            case 1:
                for (; outP < endP; inP += inChannelCount, outP += 1)
                {
                    outP[0] = inP[0];
                }
                break;
            case 2:
                for (; outP < endP; inP += inChannelCount, outP += 2)
                {
                    outP[0] = inP[0];
                    outP[1] = inP[1];
                }
                break;
            default:
                for (; outP < endP; inP += inChannelCount, outP += outChannelCount)
                {
                    for (size_t i = 0; i < outChannelCount; ++i)
                    {
                        outP[i] = inP[i];
                    }
                }
                break;
            }
        }

        template<typename T>
        inline void planarInterleave(const T** value, T* out, uint8_t channelCount, size_t sampleCount)
        {
            switch (channelCount)
            {
            case 1:
                memcpy(out, value[0], sampleCount * channelCount * sizeof(T));
                break;
            case 2:
            {
                const T* inP0 = value[0];
                const T* inP1 = value[1];
                T* outP = out;
                T* const endP = out + sampleCount * channelCount;
                for (; outP < endP; outP += 2, ++inP0, ++inP1)
                {
                    outP[0] = inP0[0];
                    outP[1] = inP1[0];
                }
                break;
            }
            default:
                for (uint8_t c = 0; c < channelCount; ++c)
                {
                    const T* inP = value[c];
                    const T* endP = inP + sampleCount;
                    T* outP = out + c;
                    for (; inP < endP; ++inP, outP += channelCount)
                    {
                        *outP = *inP;
                    }
                }
                break;
            }
        }

    } // namespace Audio
} // namespace djv
