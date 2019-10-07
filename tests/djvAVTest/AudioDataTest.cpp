//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
        
        void AudioDataTest::run(const std::vector<std::string>& args)
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
                DJV_ASSERT(data->getData(0));
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
                for (auto j : Audio::getTypeEnums())
                {
                    const Audio::Info info(2, i, 44000, 100);
                    auto data = Audio::Data::create(info);
                    data->zero();
                    auto data2 = Audio::Data::planarInterleave(data);
                    DJV_ASSERT(data->getInfo() == data2->getInfo());
                    auto data3 = Audio::Data::planarDeinterleave(data);
                    DJV_ASSERT(data->getInfo() == data2->getInfo());
                }
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

