// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAudioTest/AudioSystemFuncTest.h>

#include <djvAudio/AudioSystemFunc.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::Audio;

namespace djv
{
    namespace AudioTest
    {
        AudioSystemFuncTest::AudioSystemFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AudioTest::AudioSystemFuncTest", tempPath, context)
        {}
        
        void AudioSystemFuncTest::run()
        {
            for (const auto& i : Audio::getDeviceFormatEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Device format: " + _getText(ss.str()));
            }            
        }

    } // namespace AudioTest
} // namespace djv

