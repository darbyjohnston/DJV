// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAudioTest/DataFuncTest.h>

#include <djvAudio/Data.h>
#include <djvAudio/DataFunc.h>

using namespace djv::Core;
using namespace djv::Audio;

namespace djv
{
    namespace AudioTest
    {
        DataFuncTest::DataFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AudioTest::DataFuncTest", tempPath, context)
        {}
        
        void DataFuncTest::run()
        {
            {
                const Audio::Info info(2, Audio::Type::S16, 44000);
                auto data = Audio::Data::create(info, 100);
                data->zero();
                DJV_ASSERT(0 == reinterpret_cast<Audio::S16_T*>(data->getData())[0]);
            }
            
            for (auto i : Audio::getTypeEnums())
            {
                for (auto j : Audio::getTypeEnums())
                {
                    const Audio::Info info(2, i, 44000);
                    auto data = Audio::Data::create(info, 100);
                    data->zero();
                    auto data2 = Audio::convert(data, j);
                    DJV_ASSERT(j == data2->getType());
                }
            }
            
            for (auto i : Audio::getTypeEnums())
            {
                const Audio::Info info(2, i, 44000);
                auto data = Audio::Data::create(info, 100);
                data->zero();
                auto data2 = Audio::planarInterleave(data);
                DJV_ASSERT(data->getInfo() == data2->getInfo());
                auto data3 = Audio::planarDeinterleave(data);
                DJV_ASSERT(data->getInfo() == data2->getInfo());
            }
            
            for (auto i : Audio::getTypeEnums())
            {
                const Audio::Info info(2, i, 44000);
                auto data = Audio::Data::create(info, 100);
                auto data2 = Audio::Data::create(info, 100);
                Audio::volume(
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
                Audio::extract(data.data(), data2.data(), 3, 1, 1);
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
                Audio::extract(data.data(), data2.data(), 3, 2, 2);
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
                Audio::extract(data.data(), data2.data(), 3, 4, 3);
                DJV_ASSERT(data2 == std::vector<int>({
                    0, 3, 6,
                    1, 4, 7,
                    2, 5, 8}));
            }
            
            {
                const Audio::Info info(2, Audio::Type::S8, 0);
                auto data = Audio::Data::create(info, 3);
                uint8_t* dataP = data->getData();
                dataP[0] = 0;
                dataP[1] = 1;
                dataP[2] = 2;
                dataP[3] = 3;
                dataP[4] = 4;
                dataP[5] = 5;
                const auto data2 = Audio::planarInterleave(data);
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
                Audio::planarInterleave<int>(data, data2.data(), 1, 3);
                DJV_ASSERT(data2 == std::vector<int>({0, 1, 2}));                
            }
            
            {
                const std::vector<int> dataA = { 0, 1, 2 };
                const std::vector<int> dataB = { 3, 4, 5 };
                const int* data[2] = { dataA.data(), dataB.data() };
                std::vector<int> data2(3 * 2);
                Audio::planarInterleave<int>(data, data2.data(), 2, 3);
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
                Audio::planarInterleave<int>(data, data2.data(), 3, 3);
                DJV_ASSERT(data2 == std::vector<int>({
                    0, 3, 6,
                    1, 4, 7,
                    2, 5, 8}));                
            }
            
            {
                const Audio::Info info(2, Audio::Type::S8, 0);
                auto data = Audio::Data::create(info, 3);
                auto dataP = data->getData();
                dataP[0] = 0;
                dataP[1] = 3;
                dataP[2] = 1;
                dataP[3] = 4;
                dataP[4] = 2;
                dataP[5] = 5;
                const auto data2 = Audio::planarDeinterleave(data);
                auto data2P = data2->getData();
                DJV_ASSERT(0 == data2P[0]);
                DJV_ASSERT(1 == data2P[1]);
                DJV_ASSERT(2 == data2P[2]);
                DJV_ASSERT(3 == data2P[3]);
                DJV_ASSERT(4 == data2P[4]);
                DJV_ASSERT(5 == data2P[5]);
            }
        }
                
    } // namespace AudioTest
} // namespace djv

