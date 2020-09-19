// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAudioTest/AudioFuncTest.h>

#include <djvAudio/AudioFunc.h>

#include <djvMath/RangeFunc.h>

using namespace djv::Core;
using namespace djv::Audio;

#define CONVERT(TA, RANGE, TB) \
    { \
        auto bMin = static_cast<Audio::TB##_T>(0); \
        auto bMax = static_cast<Audio::TB##_T>(0); \
        Audio::TA##To##TB(RANGE.getMin(), bMin); \
        Audio::TA##To##TB(RANGE.getMax(), bMax); \
        std::stringstream ss; \
        ss << "    " << #TB << " " << static_cast<int64_t>(bMin) << " " << static_cast<int64_t>(bMax); \
        _print(ss.str()); \
    }

namespace djv
{
    namespace AudioTest
    {
        AudioFuncTest::AudioFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AudioTest::AudioFuncTest", tempPath, context)
        {}
        
        void AudioFuncTest::run()
        {
            _enum();
            _convert();
        }
        
        void AudioFuncTest::_enum()
        {
            for (auto i : Audio::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Type: " + _getText(ss.str()));
            }

            for (auto i : Audio::getTypeEnums())
            {
                std::stringstream ss;
                ss << Audio::toRtAudio(i);
                _print("RtAudio: " + ss.str());
            }
        }
        
        void AudioFuncTest::_convert()
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
        
    } // namespace AudioTest
} // namespace djv

