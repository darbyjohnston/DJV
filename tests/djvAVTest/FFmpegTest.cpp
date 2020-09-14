// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/FFmpegTest.h>

#include <djvAV/FFmpeg.h>

#include <djvCore/ErrorFunc.h>

#include <libavutil/error.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::IO;

namespace djv
{
    namespace AVTest
    {
        FFmpegTest::FFmpegTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::FFmpegTest", tempPath, context)
        {}
        
        void FFmpegTest::run()
        {
            _convert();
            _serialize();
        }
        
        void FFmpegTest::_convert()
        {
            for (const auto& i : {
                AV_SAMPLE_FMT_NONE,
                AV_SAMPLE_FMT_S16,
                AV_SAMPLE_FMT_S32,
                AV_SAMPLE_FMT_FLT,
                AV_SAMPLE_FMT_DBL,
                AV_SAMPLE_FMT_S16P,
                AV_SAMPLE_FMT_S32P,
                AV_SAMPLE_FMT_FLTP,
                AV_SAMPLE_FMT_DBLP })
            {
                std::stringstream ss;
                ss << FFmpeg::toAudioType(i);
                std::stringstream ss2;
                ss2 << FFmpeg::toString(i);
                _print(_getText(ss.str()) + ": " + _getText(ss2.str()));
            }

            for (const auto i : {
                AV_SAMPLE_FMT_NONE,
                AV_SAMPLE_FMT_S16,
                AV_SAMPLE_FMT_S32,
                AV_SAMPLE_FMT_FLT,
                AV_SAMPLE_FMT_DBL,
                AV_SAMPLE_FMT_S16P,
                AV_SAMPLE_FMT_S32P,
                AV_SAMPLE_FMT_FLTP,
                AV_SAMPLE_FMT_DBLP })
            {
                const Audio::Info info(2, FFmpeg::toAudioType(i), 0, 1);
                auto out = Audio::Data::create(info);
                std::vector<uint8_t> in(info.getByteCount());
                uint8_t* p = in.data();
                FFmpeg::extractAudio(&p, i, 2, out);
            }

            for (const auto i : {
                AV_SAMPLE_FMT_NONE,
                AV_SAMPLE_FMT_S16,
                AV_SAMPLE_FMT_S32,
                AV_SAMPLE_FMT_FLT,
                AV_SAMPLE_FMT_DBL,
                AV_SAMPLE_FMT_S16P,
                AV_SAMPLE_FMT_S32P,
                AV_SAMPLE_FMT_FLTP,
                AV_SAMPLE_FMT_DBLP })
            {
                const Audio::Info info(2, FFmpeg::toAudioType(i), 0, 1);
                auto out = Audio::Data::create(info);
                std::vector<uint8_t> in(info.getByteCount() * 2);
                uint8_t* p = in.data();
                FFmpeg::extractAudio(&p, i, 4, out);
            }
            
            for (const auto i : {
                AVERROR_EOF,
                AVERROR_EXIT,
                AVERROR_EXTERNAL })
            {
                _print("Error: " + FFmpeg::getErrorString(i));
            }
        }
        
        void FFmpegTest::_serialize()
        {
            {
                FFmpeg::Options options;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(options, allocator);
                FFmpeg::Options options2;
                fromJSON(json, options2);
                DJV_ASSERT(options == options2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                FFmpeg::Options options;
                fromJSON(json, options);
                DJV_ASSERT(options == options);
            }
            catch (const std::exception& e)
            {
                _print(Error::format(e.what()));
            }
        }
        
    } // namespace AVTest
} // namespace djv

