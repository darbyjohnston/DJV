// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/AudioSystemTest.h>

#include <djvAV/AudioSystemFunc.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        AudioSystemTest::AudioSystemTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::AudioSystemTest", tempPath, context)
        {}
        
        void AudioSystemTest::run()
        {
            for (const auto& i : Audio::getDeviceFormatEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Device format: " + _getText(ss.str()));
            }            
        }

    } // namespace AVTest
} // namespace djv

