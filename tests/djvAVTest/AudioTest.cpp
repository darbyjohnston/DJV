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

#include <djvAVTest/AudioTest.h>

#include <djvAV/Audio.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        void AudioTest::run(int &, char **)
        {
            DJV_DEBUG("AudioTest::run");
            members();
            convert();
            operators();
        }

        void AudioTest::members()
        {
            DJV_DEBUG("AudioTest::members");
        }

        void AudioTest::convert()
        {
            DJV_DEBUG("AudioTest::convert");
            {
                DJV_DEBUG_PRINT("U8 min = " << std::numeric_limits<uint8_t>::min());
                DJV_DEBUG_PRINT("U8 max = " << std::numeric_limits<uint8_t>::max());
                DJV_DEBUG_PRINT("S16 min = " << std::numeric_limits<int16_t>::min());
                DJV_DEBUG_PRINT("S16 max = " << std::numeric_limits<int16_t>::max());
                DJV_DEBUG_PRINT("S32 min = " << std::numeric_limits<int32_t>::min());
                DJV_DEBUG_PRINT("S32 max = " << std::numeric_limits<int32_t>::max());

                Audio::S16_T s16 = 0;
                Audio::S32_T s32 = 0;
                Audio::F32_T f32 = 0.f;
                AV::Audio::convert(std::numeric_limits<uint8_t>::min(), s16);
                DJV_DEBUG_PRINT("u8toS16 min = " << s16);
                AV::Audio::convert(std::numeric_limits<uint8_t>::max(), s16);
                DJV_DEBUG_PRINT("u8toS16 max = " << s16);
                AV::Audio::convert(std::numeric_limits<uint8_t>::min(), s32);
                DJV_DEBUG_PRINT("u8toS32 min = " << s32);
                AV::Audio::convert(std::numeric_limits<uint8_t>::max(), s32);
                DJV_DEBUG_PRINT("u8toS32 max = " << s32);
                AV::Audio::convert(std::numeric_limits<uint8_t>::min(), f32);
                DJV_DEBUG_PRINT("u8toF32 min = " << f32);
                AV::Audio::convert(std::numeric_limits<uint8_t>::max(), f32);
                DJV_DEBUG_PRINT("u8toF32 max = " << f32);

                Audio::U8_T u8 = 0;
                AV::Audio::convert(std::numeric_limits<int16_t>::min(), u8);
                DJV_DEBUG_PRINT("s16toU8 min = " << u8);
                AV::Audio::convert(std::numeric_limits<int16_t>::max(), u8);
                DJV_DEBUG_PRINT("s16toU8 max = " << u8);
                AV::Audio::convert(std::numeric_limits<int16_t>::min(), s32);
                DJV_DEBUG_PRINT("s16toS32 min = " << s32);
                AV::Audio::convert(std::numeric_limits<int16_t>::max(), s32);
                DJV_DEBUG_PRINT("s16toS32 max = " << s32);
                AV::Audio::convert(std::numeric_limits<int16_t>::min(), f32);
                DJV_DEBUG_PRINT("s16toF32 min = " << f32);
                AV::Audio::convert(std::numeric_limits<int16_t>::max(), f32);
                DJV_DEBUG_PRINT("s16toF32 max = " << f32);

                AV::Audio::convert(std::numeric_limits<int32_t>::min(), u8);
                DJV_DEBUG_PRINT("s32toU8 min = " << u8);
                AV::Audio::convert(std::numeric_limits<int32_t>::max(), u8);
                DJV_DEBUG_PRINT("s32toU8 max = " << u8);
                AV::Audio::convert(std::numeric_limits<int32_t>::min(), s16);
                DJV_DEBUG_PRINT("s32toS16 min = " << s16);
                AV::Audio::convert(std::numeric_limits<int32_t>::max(), s16);
                DJV_DEBUG_PRINT("s32toS16 max = " << s16);
                AV::Audio::convert(std::numeric_limits<int32_t>::min(), f32);
                DJV_DEBUG_PRINT("s32toF32 min = " << f32);
                AV::Audio::convert(std::numeric_limits<int32_t>::max(), f32);
                DJV_DEBUG_PRINT("s32toF32 max = " << f32);

                AV::Audio::convert(-1.f, u8);
                DJV_DEBUG_PRINT("f32toU8 min = " << u8);
                AV::Audio::convert(1.f, u8);
                DJV_DEBUG_PRINT("f32toU8 max = " << u8);
                AV::Audio::convert(-1.f, s16);
                DJV_DEBUG_PRINT("f32toS16 min = " << s16);
                AV::Audio::convert(1.f, s16);
                DJV_DEBUG_PRINT("f32toS16 max = " << s16);
                AV::Audio::convert(-1.f, s32);
                DJV_DEBUG_PRINT("f32toS32 min = " << s32);
                AV::Audio::convert(1.f, s32);
                DJV_DEBUG_PRINT("f32toS32 max = " << s32);
            }
        }

        void AudioTest::operators()
        {
            DJV_DEBUG("AudioTest::operators");
        }

    } // namespace AVTest
} // namespace djv
