//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

using namespace djv::Core;
using namespace djv::AV;

#define CONVERT(TA, RANGE, TB) \
    { \
        auto bMin = static_cast<Audio::TB##_T>(0); \
        auto bMax = static_cast<Audio::TB##_T>(0); \
        Audio::TA##To##TB(RANGE.min, bMin); \
        Audio::TA##To##TB(RANGE.max, bMax); \
        std::stringstream ss; \
        ss << "    " << #TB << " " << static_cast<int64_t>(bMin) << " " << static_cast<int64_t>(bMax); \
        _print(ss.str()); \
    }

namespace djv
{
    namespace AVTest
    {
        AudioTest::AudioTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::AudioTest", context)
        {}
        
        void AudioTest::run(const std::vector<std::string>& args)
        {
            _enum();
            _constants();
            _convert();
        }
        
        void AudioTest::_enum()
        {
            for (auto i : Audio::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "type string: " << _getText(ss.str());
                _print(ss2.str());
            }
        }
        
        void AudioTest::_constants()
        {
            {
                std::stringstream ss;
                ss << "S8 range: " << Audio::S8Range;
                _print(ss.str());
            }
            {
                std::stringstream ss;
                ss << "S16 range: " << Audio::S16Range;
                _print(ss.str());
            }
            {
                std::stringstream ss;
                ss << "S32 range: " << Audio::S32Range;
                _print(ss.str());
            }
            {
                std::stringstream ss;
                ss << "F32 range: " << Audio::F32Range;
                _print(ss.str());
            }
            {
                std::stringstream ss;
                ss << "F64 range: " << Audio::F64Range;
                _print(ss.str());
            }
        }
        
        void AudioTest::_convert()
        {
            {
                std::stringstream ss;
                ss << "S8 " << Audio::S8Range << " =";
                _print(ss.str());
                CONVERT(S8, Audio::S8Range, S16);
                CONVERT(S8, Audio::S8Range, S32);
                CONVERT(S8, Audio::S8Range, F32);
                CONVERT(S8, Audio::S8Range, F64);
            }
            
            {
                std::stringstream ss;
                ss << "S16 " << Audio::S16Range << " =";
                _print(ss.str());
                CONVERT(S16, Audio::S16Range, S8);
                CONVERT(S16, Audio::S16Range, S32);
                CONVERT(S16, Audio::S16Range, F32);
                CONVERT(S16, Audio::S16Range, F64);
            }
            
            {
                std::stringstream ss;
                ss << "S32 " << Audio::S32Range << " =";
                _print(ss.str());
                CONVERT(S32, Audio::S32Range, S8);
                CONVERT(S32, Audio::S32Range, S16);
                CONVERT(S32, Audio::S32Range, F32);
                CONVERT(S32, Audio::S32Range, F64);
            }
            
            {
                std::stringstream ss;
                ss << "F32 " << Audio::F32Range << " =";
                _print(ss.str());
                CONVERT(F32, Audio::F32Range, S8);
                CONVERT(F32, Audio::F32Range, S16);
                CONVERT(F32, Audio::F32Range, S32);
                CONVERT(F32, Audio::F32Range, F64);
            }
            
            {
                std::stringstream ss;
                ss << "F64 " << Audio::F64Range << " =";
                _print(ss.str());
                CONVERT(F64, Audio::F64Range, S8);
                CONVERT(F64, Audio::F64Range, S16);
                CONVERT(F64, Audio::F64Range, S32);
                CONVERT(F64, Audio::F64Range, F32);
            }
        }
        
    } // namespace AVTest
} // namespace djv

