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

#include <djvAV/IO.h>

#if defined(DJV_LINUX)
#define __STDC_CONSTANT_MACROS
#endif // DJV_LINUX

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libswscale/swscale.h>

} // extern "C"

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This plugin provides support support for FFmpeg.
            namespace FFmpeg
            {
                static const std::string pluginName = "FFmpeg";
                static const std::set<std::string> fileExtensions = { ".mp4", ".m4v", ".mov" };

                AVRational getTimeBaseQ();

                Audio::Type toAudioType(AVSampleFormat);
                std::string toString(AVSampleFormat);

                std::string getErrorString(int);

                class Read : public IRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    void _init(
                        const std::string & fileName,
                        const std::shared_ptr<ReadQueue> &,
                        const std::shared_ptr<Core::Context> &);
                    Read();

                public:
                    ~Read();

                    static std::shared_ptr<Read> create(
                        const std::string & fileName,
                        const std::shared_ptr<ReadQueue> &,
                        const std::shared_ptr<Core::Context> &);

                    std::future<Info> getInfo() override;

                    virtual void seek(Timestamp) override;

                private:
                    Timestamp _decodeVideo(AVPacket *, bool seek = false);
                    Timestamp _decodeAudio(AVPacket *, bool seek = false);

                    VideoInfo _videoInfo;
                    AudioInfo _audioInfo;
                    std::promise<Info> _infoPromise;

                    std::condition_variable _queueCV;
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

                class Plugin : public IPlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    void _init(const std::shared_ptr<Core::Context>&);

                    Plugin();

                public:
                    static std::shared_ptr<Plugin> create(const std::shared_ptr<Core::Context>&);

                    std::shared_ptr<IRead> read(
                        const std::string & fileName,
                        const std::shared_ptr<ReadQueue> &) const override;
                };

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV
} // namespace djv
