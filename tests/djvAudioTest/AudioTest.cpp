// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudioTest/AudioTest.h>

#include <djvAudio/Audio.h>

#include <djvMath/RangeFunc.h>

using namespace djv::Core;
using namespace djv::Audio;

namespace djv
{
    namespace AudioTest
    {
        AudioTest::AudioTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AudioTest::AudioTest", tempPath, context)
        {}
        
        void AudioTest::run()
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
                
    } // namespace AudioTest
} // namespace djv

