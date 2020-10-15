// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudioTest/InfoTest.h>

#include <djvAudio/Info.h>

using namespace djv::Core;
using namespace djv::Audio;

namespace djv
{
    namespace AudioTest
    {
        InfoTest::InfoTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AudioTest::InfoTest", tempPath, context)
        {}
        
        void InfoTest::run()
        {
            _info();
            _operators();
        }

        void InfoTest::_info()
        {
            {
                const Audio::Info info;
                DJV_ASSERT(Audio::defaultName == info.name);
                DJV_ASSERT(0 == info.channelCount);
                DJV_ASSERT(Audio::Type::None == info.type);
                DJV_ASSERT(0 == info.sampleRate);
                DJV_ASSERT(!info.isValid());
            }

            {
                const Audio::Info info(2, Audio::Type::S16, 44000);
                DJV_ASSERT(Audio::defaultName == info.name);
                DJV_ASSERT(2 == info.channelCount);
                DJV_ASSERT(Audio::Type::S16 == info.type);
                DJV_ASSERT(44000 == info.sampleRate);
                DJV_ASSERT(info.isValid());
            }
        }
                        
        void InfoTest::_operators()
        {
            {
                const Audio::Info info(2, Audio::Type::S16, 44000);
                DJV_ASSERT(info == info);
                DJV_ASSERT(info != Audio::Info());
            }
        }
        
    } // namespace AudioTest
} // namespace djv

