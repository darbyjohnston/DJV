//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#pragma once

#include <djvAV/AudioData.h>
#include <djvAV/FFmpeg.h>
#include <djvAV/PixelData.h>
#include <djvAV/Speed.h>

#include <djvCore/Error.h>

#include <condition_variable>
#include <future>
#include <mutex>

namespace djv
{
    namespace AV
    {
        class Context;

        namespace IO
        {
            typedef int64_t Timestamp;
            typedef int64_t Duration;

            class VideoInfo
            {
            public:
                VideoInfo();
                VideoInfo(const Image::PixelDataInfo &, const Speed &, Duration duration);

                inline const Image::PixelDataInfo & getInfo() const;
                inline const Speed & getSpeed() const;
                inline Duration getDuration() const;

                bool operator == (const VideoInfo &) const;

            private:
                Image::PixelDataInfo _info;
                Speed _speed;
                Duration _duration = 0;
            };

            class AudioInfo
            {
            public:
                AudioInfo();
                AudioInfo(const Audio::DataInfo &, Duration duration);

                inline const Audio::DataInfo & getInfo() const;
                inline Duration getDuration() const;

                bool operator == (const AudioInfo &) const;

            private:
                Audio::DataInfo _info;
                Duration _duration   = 0;
            };

            class Info
            {
            public:
                Info();
                Info(const std::string & fileName, const VideoInfo &, const AudioInfo &);

                inline const std::string & getFileName() const;
                inline const VideoInfo & getVideo() const;
                inline const AudioInfo & getAudio() const;

            private:
                std::string _fileName;
                VideoInfo _video;
                AudioInfo _audio;
            };

            typedef std::pair<Timestamp, std::shared_ptr<Image::PixelData> > VideoFrame;
            typedef std::pair<Timestamp, std::shared_ptr<Audio::Data> > AudioFrame;

            class Queue : public std::enable_shared_from_this<Queue>
            {
                DJV_NON_COPYABLE(Queue);

            protected:
                Queue();

            public:
                static std::shared_ptr<Queue> create();

                std::mutex & getMutex();

                size_t getVideoFrameCount() const;
                size_t getAudioFrameCount() const;
                
                bool hasVideoFrames() const;
                bool hasAudioFrames() const;
                
                VideoFrame getFirstVideoFrame() const;

                void addVideoFrame(Timestamp, const std::shared_ptr<Image::PixelData> &);
                void addAudioFrame(Timestamp, const std::shared_ptr<Audio::Data> &);

                void clear();

            private:
                std::list<VideoFrame> _videoFrames;
                std::list<AudioFrame> _audioFrames;
                std::mutex _mutex;
            };

            class Loader : public std::enable_shared_from_this<Loader>
            {
                DJV_NON_COPYABLE(Loader);

            protected:
                void _init(const std::string & fileName, const std::shared_ptr<Queue> &, const std::shared_ptr<Context> &);
                Loader();

            public:
                ~Loader();

                static std::shared_ptr<Loader> create(const std::string & fileName, const std::shared_ptr<Queue> &, const std::shared_ptr<Context> &);

                std::future<Info> getInfo();

                void seek(Timestamp);

            private:
                Timestamp _decodeVideo(AVPacket *, bool seek = false);
                Timestamp _decodeAudio(AVPacket *, bool seek = false);

                std::weak_ptr<Context> _context;

                std::shared_ptr<Queue> _queue;
                std::condition_variable _queueCV;

                Info _info;
                std::promise<Info> _infoPromise;

                std::thread _thread;
                std::atomic<bool> _running;

                AVFormatContext * _avFormatContext = nullptr;
                int _avVideoStream = -1;
                int _avAudioStream = -1;
                std::map<int, AVCodecParameters *> _avCodecParameters;
                std::map<int, AVCodecContext *> _avCodecContext;
                AVFrame * _avFrame = nullptr;
                AVFrame * _avFrameRgb = nullptr;
                SwsContext * _swsContext = nullptr;
                Timestamp _seek = -1;

                bool _hasError = false;
                Core::Error _error;
            };

            class Util
            {
            public:
                virtual ~Util() = 0;

                static double timestampToSeconds(Timestamp);
                static Timestamp secondsToTimestamp(double);
            };

        } // namespace IO
    } // namespace AV
} // namespace djv

#include <djvAV/IOInline.h>
