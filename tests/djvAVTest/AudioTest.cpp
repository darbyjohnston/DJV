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

namespace djv
{
    namespace AVTest
    {
        AudioTest::AudioTest(Core::Context * context) :
            ITest("djv::AVTest::AudioTest", context)
        {}
        
#define CONVERT(ta, min, max, tb) \
    { \
        auto bmin = static_cast<AV::Audio::tb##_T>(0); \
        auto bmax = static_cast<AV::Audio::tb##_T>(0); \
        AV::Audio::ta##To##tb(min, bmin); \
        AV::Audio::ta##To##tb(max, bmax); \
        std::stringstream ss; \
        ss << "    " << #tb << " " << static_cast<int64_t>(bmin) << " " << static_cast<int64_t>(bmax); \
        _print(ss.str()); \
    }

        void AudioTest::run(int & argc, char ** argv)
        {
            {
                std::stringstream ss;
                ss << "S8 " << static_cast<int64_t>(AV::Audio::S8Min) << " " << static_cast<int64_t>(AV::Audio::S8Max) << " =";
                _print(ss.str());
                CONVERT(S8, AV::Audio::S8Min, AV::Audio::S8Max, S16);
                CONVERT(S8, AV::Audio::S8Min, AV::Audio::S8Max, S32);
                CONVERT(S8, AV::Audio::S8Min, AV::Audio::S8Max, F32);
                CONVERT(S8, AV::Audio::S8Min, AV::Audio::S8Max, F64);
            }
            {
                std::stringstream ss;
                ss << "S16 " << static_cast<int64_t>(AV::Audio::S16Min) << " " << static_cast<int64_t>(AV::Audio::S16Max) << " =";
                _print(ss.str());
                CONVERT(S16, AV::Audio::S16Min, AV::Audio::S16Max, S8);
                CONVERT(S16, AV::Audio::S16Min, AV::Audio::S16Max, S32);
                CONVERT(S16, AV::Audio::S16Min, AV::Audio::S16Max, F32);
                CONVERT(S16, AV::Audio::S16Min, AV::Audio::S16Max, F64);
            }
            {
                std::stringstream ss;
                ss << "S32 " << static_cast<int64_t>(AV::Audio::S32Min) << " " << static_cast<int64_t>(AV::Audio::S32Max) << " =";
                _print(ss.str());
                CONVERT(S32, AV::Audio::S32Min, AV::Audio::S32Max, S8);
                CONVERT(S32, AV::Audio::S32Min, AV::Audio::S32Max, S16);
                CONVERT(S32, AV::Audio::S32Min, AV::Audio::S32Max, F32);
                CONVERT(S32, AV::Audio::S32Min, AV::Audio::S32Max, F64);
            }
            {
                std::stringstream ss;
                ss << "F32 " << static_cast<int64_t>(AV::Audio::F32Min) << " " << static_cast<int64_t>(AV::Audio::F32Max) << " =";
                _print(ss.str());
                CONVERT(F32, AV::Audio::F32Min, AV::Audio::F32Max, S8);
                CONVERT(F32, AV::Audio::F32Min, AV::Audio::F32Max, S16);
                CONVERT(F32, AV::Audio::F32Min, AV::Audio::F32Max, S32);
                CONVERT(F32, AV::Audio::F32Min, AV::Audio::F32Max, F64);
            }
            {
                std::stringstream ss;
                ss << "F64 " << static_cast<int64_t>(AV::Audio::F64Min) << " " << static_cast<int64_t>(AV::Audio::F64Max) << " =";
                _print(ss.str());
                CONVERT(F64, AV::Audio::F64Min, AV::Audio::F64Max, S8);
                CONVERT(F64, AV::Audio::F64Min, AV::Audio::F64Max, S16);
                CONVERT(F64, AV::Audio::F64Min, AV::Audio::F64Max, S32);
                CONVERT(F64, AV::Audio::F64Min, AV::Audio::F64Max, F32);
            }
        }
        
    } // namespace AVTest
} // namespace djv

