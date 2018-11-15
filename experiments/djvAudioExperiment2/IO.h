//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <Util.h>

#include <djvAV/AudioData.h>
#include <djvAV/FFmpeg.h>
#include <djvAV/PixelData.h>

#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>

#include <QObject>
#include <QPointer>

#include <future>
#include <map>
#include <mutex>
#include <thread>

namespace djv
{
    namespace AudioExperiment2
    {
        class Context;

        struct IOInfo
        {
            Core::FileInfo  fileInfo;
            Util::VideoInfo video;
            Util::AudioInfo audio;
        };

        class IO : public QObject
        {
            Q_OBJECT

        public:
            IO(const Core::FileInfo &, const QPointer<Context> &, QObject * parent = nullptr);
            ~IO() override;

            std::future<IOInfo> ioInfo();

            std::shared_ptr<Util::AVQueue> queue() const;

            void seek(int64_t);

        private:
            int64_t _decodeVideo(AVPacket *, bool seek = false);
            int64_t _decodeAudio(AVPacket *, bool seek = false);

            QPointer<Context> _context;

            std::shared_ptr<Util::AVQueue> _queue;
            std::condition_variable _queueCV;

            IOInfo _ioInfo;
            std::promise<IOInfo> _ioInfoPromise;

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
            int64_t _seek = 0;

            Core::Error _error;
        };

    } // namespace AudioExperiment2
} // namespace djv
