// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/AudioDataTest.h>

#include <djvAV/AudioData.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        AudioDataTest::AudioDataTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::AudioDataTest", context)
        {}
        
        void AudioDataTest::run()
        {
            _info();
            _data();
            _util();
            _operators();
        }

        void AudioDataTest::_info()
        {
            {
                const Audio::Info info;
                DJV_ASSERT(0 == info.channelCount);
                DJV_ASSERT(Audio::Type::None == info.type);
                DJV_ASSERT(0 == info.sampleRate);
                DJV_ASSERT(0 == info.sampleCount);
                DJV_ASSERT(!info.isValid());
            }

            {
                const Audio::Info info(2, Audio::Type::S16, 44000, 100);
                DJV_ASSERT(2 == info.channelCount);
                DJV_ASSERT(Audio::Type::S16 == info.type);
                DJV_ASSERT(44000 == info.sampleRate);
                DJV_ASSERT(100 == info.sampleCount);
                DJV_ASSERT(info.isValid());
            }
        }
        
        void AudioDataTest::_data()
        {
            {
                auto data = Audio::Data::create(Audio::Info());
                DJV_ASSERT(!data->isValid());
            }
            
            {
                const Audio::Info info(2, Audio::Type::S16, 44000, 100);
                auto data = Audio::Data::create(info);
                DJV_ASSERT(info == data->getInfo());
                DJV_ASSERT(info.channelCount == data->getChannelCount());
                DJV_ASSERT(info.type == data->getType());
                DJV_ASSERT(info.sampleRate == data->getSampleRate());
                DJV_ASSERT(info.sampleCount == data->getSampleCount());
                DJV_ASSERT(data->isValid());
                DJV_ASSERT(info.getByteCount() == data->getByteCount());
                DJV_ASSERT(data->getData());
            }
        }
        
        void AudioDataTest::_util()
        {
            {
                const Audio::Info info(2, Audio::Type::S16, 44000, 100);
                auto data = Audio::Data::create(info);
                data->zero();
                DJV_ASSERT(0 == reinterpret_cast<Audio::S16_T*>(data->getData())[0]);
            }
            
            for (auto i : Audio::getTypeEnums())
            {
                for (auto j : Audio::getTypeEnums())
                {
                    const Audio::Info info(2, i, 44000, 100);
                    auto data = Audio::Data::create(info);
                    data->zero();
                    auto data2 = Audio::Data::convert(data, j);
                    DJV_ASSERT(j == data2->getType());
                }
            }
            
            for (auto i : Audio::getTypeEnums())
            {
                const Audio::Info info(2, i, 44000, 100);
                auto data = Audio::Data::create(info);
                data->zero();
                auto data2 = Audio::Data::planarInterleave(data);
                DJV_ASSERT(data->getInfo() == data2->getInfo());
                auto data3 = Audio::Data::planarDeinterleave(data);
                DJV_ASSERT(data->getInfo() == data2->getInfo());
            }
            
            for (auto i : Audio::getTypeEnums())
            {
                const Audio::Info info(2, i, 44000, 100);
                auto data = Audio::Data::create(info);
                auto data2 = Audio::Data::create(info);
                Audio::Data::volume(
                    data->getData(),
                    data2->getData(),
                    .5F,
                    data->getSampleCount(),
                    info.channelCount,
                    info.type);
            }
        }
        
        void AudioDataTest::_operators()
        {
            {
                const Audio::Info info(2, Audio::Type::S16, 44000, 100);
                DJV_ASSERT(info == info);
                DJV_ASSERT(info != Audio::Info());
            }
            
            {
                const Audio::Info info(2, Audio::Type::S16, 44000, 100);
                auto data = Audio::Data::create(info);
                data->zero();
                auto data2 = Audio::Data::create(info);
                data2->zero();
                DJV_ASSERT(*data == *data2);
                auto data3 = Audio::Data::create(Audio::Info());
                DJV_ASSERT(*data != *data3);
            }
        }
        
    } // namespace AVTest
} // namespace djv

