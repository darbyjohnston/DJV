// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/AudioTest.h>

#include <djvAV/Audio.h>

using namespace djv::Core;
using namespace djv::AV;

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
    namespace AVTest
    {
        AudioTest::AudioTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::AudioTest", tempPath, context)
        {}
        
        void AudioTest::run()
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
                ss2 << "Type string: " << _getText(ss.str());
                _print(ss2.str());
            }

            for (auto i : Audio::getTypeEnums())
            {
                std::stringstream ss;
                ss << Audio::toRtAudio(i);
                ss << "RtAudio: " << ss.str();
                _print(ss.str());
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

