//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAVTest/AudioDataTest.h>

#include <djvAV/AudioData.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        void AudioDataTest::run(int &, char **)
        {
            DJV_DEBUG("AudioDataTest::run");
            convert();
            interleave();
            deinterleave();
        }

        void AudioDataTest::convert()
        {
            DJV_DEBUG("AudioDataTest::convert");
            {
                const auto dataInfo = AudioInfo(1, Audio::U8, 1, 10);
                auto data = AV::AudioData(dataInfo);
                auto dataP = reinterpret_cast<Audio::U8_T *>(data.data());
                for (size_t i = 0; i < dataInfo.sampleCount; ++i)
                {
                    dataP[i] = (i / static_cast<float>(dataInfo.sampleCount - 1)) * (Audio::u8Max - Audio::u8Min) - Audio::u8Min;
                }
                auto outData = AV::AudioData::convert(data, Audio::F32);
                auto outDataP = reinterpret_cast<Audio::F32_T *>(outData.data());
                for (size_t i = 0; i < dataInfo.sampleCount; ++i)
                {
                    DJV_DEBUG_PRINT(dataP[i] << " = " << outDataP[i]);
                }
            }
        }

        void AudioDataTest::interleave()
        {
            DJV_DEBUG("AudioDataTest::interleave");
            {
                const auto info = AudioInfo(3, Audio::U8, 1, 9);
                auto planar = AV::AudioData(info);
                memcpy(
                    planar.data(),
                    std::vector<uint8_t>(
                        {
                            0, 0, 0, 1, 1, 1, 2, 2, 2
                        }).data(),
                    info.byteCount());
                const auto interleaved = AV::AudioData::planarInterleave(planar);
                DJV_ASSERT(0 == memcmp(
                    interleaved.data(),
                    std::vector<uint8_t>(
                        {
                            0, 1, 2, 0, 1, 2, 0, 1, 2
                        }).data(),
                    info.byteCount()));
            }
            {
                const auto info = AudioInfo(3, Audio::S16, 1, 9);
                auto planar = AV::AudioData(info);
                memcpy(
                    planar.data(),
                    std::vector<int16_t>(
                        {
                            0, 0, 0, 1, 1, 1, 2, 2, 2
                        }).data(),
                    info.byteCount());
                const auto interleaved = AV::AudioData::planarInterleave(planar);
                DJV_ASSERT(0 == memcmp(
                    interleaved.data(),
                    std::vector<int16_t>(
                        {
                            0, 1, 2, 0, 1, 2, 0, 1, 2
                        }).data(),
                    info.byteCount()));
            }
        }

        void AudioDataTest::deinterleave()
        {
            DJV_DEBUG("AudioDataTest::deinterleave");
            {
                const auto info = AudioInfo(3, Audio::U8, 1, 9);
                auto interleaved = AV::AudioData(info);
                memcpy(
                    interleaved.data(),
                    std::vector<uint8_t>(
                        {
                            0, 1, 2, 0, 1, 2, 0, 1, 2
                        }).data(),
                    info.byteCount());
                const auto planar = AV::AudioData::planarDeinterleave(interleaved);
                DJV_ASSERT(0 == memcmp(
                    planar.data(),
                    std::vector<uint8_t>(
                        {
                            0, 0, 0, 1, 1, 1, 2, 2, 2
                        }).data(),
                    info.byteCount()));
            }
            {
                const auto info = AudioInfo(3, Audio::S16, 1, 9);
                auto interleaved = AV::AudioData(info);
                memcpy(
                    interleaved.data(),
                    std::vector<int16_t>(
                        {
                            0, 1, 2, 0, 1, 2, 0, 1, 2
                        }).data(),
                    info.byteCount());
                const auto planar = AV::AudioData::planarDeinterleave(interleaved);
                DJV_ASSERT(0 == memcmp(
                    planar.data(),
                    std::vector<int16_t>(
                        {
                            0, 0, 0, 1, 1, 1, 2, 2, 2
                        }).data(),
                    info.byteCount()));
            }
        }

    } // namespace AVTest
} // namespace djv
