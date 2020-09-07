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
        AudioDataTest::AudioDataTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::AudioDataTest", tempPath, context)
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
                DJV_ASSERT(Audio::defaultName == info.name);
                DJV_ASSERT(0 == info.channelCount);
                DJV_ASSERT(Audio::Type::None == info.type);
                DJV_ASSERT(0 == info.sampleRate);
                DJV_ASSERT(0 == info.sampleCount);
                DJV_ASSERT(!info.isValid());
            }

            {
                const Audio::Info info(2, Audio::Type::S16, 44000, 100);
                DJV_ASSERT(Audio::defaultName == info.name);
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
            
            {
                const std::vector<int8_t> data = {
                    0,
                    1,
                    2
                };
                std::vector<int8_t> data2(3);
                Audio::Data::extract(data.data(), data2.data(), 3, 1, 1);
                DJV_ASSERT(data2 == std::vector<int8_t>({
                    0,
                    1,
                    2}));
            }
            
            {
                const std::vector<int> data = {
                    0, 3,
                    1, 4,
                    2, 5
                };
                std::vector<int> data2(3 * 2);
                Audio::Data::extract(data.data(), data2.data(), 3, 2, 2);
                DJV_ASSERT(data2 == std::vector<int>({
                    0, 3,
                    1, 4,
                    2, 5}));
            }
            
            {
                const std::vector<int> data = {
                    0, 3, 6, 9,
                    1, 4, 7, 10,
                    2, 5, 8, 11
                };
                std::vector<int> data2(3 * 3);
                Audio::Data::extract(data.data(), data2.data(), 3, 4, 3);
                DJV_ASSERT(data2 == std::vector<int>({
                    0, 3, 6,
                    1, 4, 7,
                    2, 5, 8}));
            }
            
            {
                const Audio::Info info(2, Audio::Type::S8, 0, 3);
                auto data = Audio::Data::create(info);
                uint8_t* dataP = data->getData();
                dataP[0] = 0;
                dataP[1] = 1;
                dataP[2] = 2;
                dataP[3] = 3;
                dataP[4] = 4;
                dataP[5] = 5;
                const auto data2 = Audio::Data::planarInterleave(data);
                const uint8_t* data2P = data2->getData();
                DJV_ASSERT(0 == data2P[0]);
                DJV_ASSERT(3 == data2P[1]);
                DJV_ASSERT(1 == data2P[2]);
                DJV_ASSERT(4 == data2P[3]);
                DJV_ASSERT(2 == data2P[4]);
                DJV_ASSERT(5 == data2P[5]);
            }
            
            {
                const std::vector<int> dataA = { 0, 1, 2 };
                const int* data[1] = { dataA.data() };
                std::vector<int> data2(3);
                Audio::Data::planarInterleave<int>(data, data2.data(), 1, 3);
                DJV_ASSERT(data2 == std::vector<int>({0, 1, 2}));                
            }
            
            {
                const std::vector<int> dataA = { 0, 1, 2 };
                const std::vector<int> dataB = { 3, 4, 5 };
                const int* data[2] = { dataA.data(), dataB.data() };
                std::vector<int> data2(3 * 2);
                Audio::Data::planarInterleave<int>(data, data2.data(), 2, 3);
                DJV_ASSERT(data2 == std::vector<int>({
                    0, 3,
                    1, 4,
                    2, 5}));                
            }
            
            {
                const std::vector<int> dataA = { 0, 1, 2 };
                const std::vector<int> dataB = { 3, 4, 5 };
                const std::vector<int> dataC = { 6, 7, 8 };
                const int* data[3] = { dataA.data(), dataB.data(), dataC.data() };
                std::vector<int> data2(3 * 3);
                Audio::Data::planarInterleave<int>(data, data2.data(), 3, 3);
                DJV_ASSERT(data2 == std::vector<int>({
                    0, 3, 6,
                    1, 4, 7,
                    2, 5, 8}));                
            }
            
            {
                const Audio::Info info(2, Audio::Type::S8, 0, 3);
                auto data = Audio::Data::create(info);
                auto dataP = data->getData();
                dataP[0] = 0;
                dataP[1] = 3;
                dataP[2] = 1;
                dataP[3] = 4;
                dataP[4] = 2;
                dataP[5] = 5;
                const auto data2 = Audio::Data::planarDeinterleave(data);
                auto data2P = data2->getData();
                DJV_ASSERT(0 == data2P[0]);
                DJV_ASSERT(1 == data2P[1]);
                DJV_ASSERT(2 == data2P[2]);
                DJV_ASSERT(3 == data2P[3]);
                DJV_ASSERT(4 == data2P[4]);
                DJV_ASSERT(5 == data2P[5]);
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

