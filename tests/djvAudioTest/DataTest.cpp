// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudioTest/DataTest.h>

#include <djvAudio/Data.h>

using namespace djv::Core;
using namespace djv::Audio;

namespace djv
{
    namespace AudioTest
    {
        DataTest::DataTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AudioTest::DataTest", tempPath, context)
        {}
        
        void DataTest::run()
        {
            _data();
            _operators();
        }
        
        void DataTest::_data()
        {
            {
                auto data = Audio::Data::create(Audio::Info(), 0);
                DJV_ASSERT(!data->isValid());
            }
            
            {
                const Audio::Info info(2, Audio::Type::S16, 44000);
                auto data = Audio::Data::create(info, 100);
                DJV_ASSERT(info == data->getInfo());
                DJV_ASSERT(info.channelCount == data->getChannelCount());
                DJV_ASSERT(info.type == data->getType());
                DJV_ASSERT(info.sampleRate == data->getSampleRate());
                DJV_ASSERT(100 == data->getSampleCount());
                DJV_ASSERT(data->isValid());
                DJV_ASSERT(info.getByteCount() * 100 == data->getByteCount());
                DJV_ASSERT(data->getData());
            }
        }
                
        void DataTest::_operators()
        {
            {
                const Audio::Info info(2, Audio::Type::S16, 44000);
                auto data = Audio::Data::create(info, 100);
                data->zero();
                auto data2 = Audio::Data::create(info, 100);
                data2->zero();
                DJV_ASSERT(*data == *data2);
                auto data3 = Audio::Data::create(Audio::Info(), 0);
                DJV_ASSERT(*data != *data3);
            }
        }
        
    } // namespace AudioTest
} // namespace djv

