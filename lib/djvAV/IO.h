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

#include <djvCore/Error.h>
#include <djvCore/Speed.h>

#include <QObject>
#include <QPointer>

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
                VideoInfo(const PixelDataInfo &, const Core::Speed &, Duration duration);

                inline const PixelDataInfo & getInfo() const;
                inline const Core::Speed & getSpeed() const;
                inline Duration getDuration() const;

                bool operator == (const VideoInfo &) const;

            private:
                PixelDataInfo _info;
                Core::Speed   _speed;
                Duration      _duration = 0;
            };

            class AudioInfo
            {
            public:
                AudioInfo();
                AudioInfo(const AudioDataInfo &, Duration duration);

                inline const AudioDataInfo & getInfo() const;
                inline Duration getDuration() const;

                bool operator == (const AudioInfo &) const;

            private:
                AudioDataInfo _info;
                Duration      _duration   = 0;
            };

            class Info
            {
            public:
                Info();
                Info(const QString & fileName, const VideoInfo &, const AudioInfo &);

                inline const QString & getfileName() const;
                inline const VideoInfo & getVideo() const;
                inline const AudioInfo & getAudio() const;

            private:
                QString   _fileName;
                VideoInfo _video;
                AudioInfo _audio;
            };

            typedef std::pair<Timestamp, std::shared_ptr<PixelData> > VideoFrame;
            typedef std::pair<Timestamp, std::shared_ptr<AudioData> > AudioFrame;

            class Queue
            {
                DJV_NON_COPYABLE(Queue);

                Queue();

            public:
                static std::shared_ptr<Queue> create();

                std::mutex & getMutex();

                size_t getVideoFrameCount() const;
                size_t getAudioFrameCount() const;
                
                bool hasVideoFrames() const;
                bool hasAudioFrames() const;
                
                VideoFrame getFirstVideoFrame() const;

                void addVideoFrame(Timestamp, const std::shared_ptr<PixelData> &);
                void addAudioFrame(Timestamp, const std::shared_ptr<AudioData> &);

                void clear();

            private:
                std::list<VideoFrame> _videoFrames;
                std::list<AudioFrame> _audioFrames;
                std::mutex _mutex;
            };

            class Loader : public QObject
            {
                Q_OBJECT
                DJV_NON_COPYABLE(Loader);

            public:
                Loader(const QString &, const std::shared_ptr<Queue> &, const QPointer<Context> &, QObject * parent = nullptr);
                ~Loader() override;

                std::future<Info> getInfo();

            public Q_SLOTS:
                void seek(Timestamp);

            private:
                Timestamp _decodeVideo(AVPacket *, bool seek = false);
                Timestamp _decodeAudio(AVPacket *, bool seek = false);

                QPointer<Context> _context;

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
