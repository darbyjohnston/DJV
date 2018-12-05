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
#include <djvAV/Image.h>
#include <djvAV/Speed.h>
#include <djvAV/Tags.h>
#include <djvAV/Time.h>

#include <djvCore/Error.h>
#include <djvCore/ISystem.h>
#include <djvCore/PicoJSON.h>

#include <future>
#include <mutex>
#include <set>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            class VideoInfo
            {
            public:
                VideoInfo();
                VideoInfo(const Pixel::Info &, const Speed &, Duration duration);

                inline const Pixel::Info & getInfo() const;
                inline const Speed & getSpeed() const;
                inline Duration getDuration() const;

                void setInfo(const Pixel::Info &);
                void setSpeed(const Speed &);
                void setDuration(Duration);

                bool operator == (const VideoInfo &) const;

            private:
                Pixel::Info _info;
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

                void setInfo(const Audio::DataInfo &);
                void setDuration(Duration);

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
                Info(const std::string & fileName, const std::vector<VideoInfo> &, const std::vector<AudioInfo> &);

                inline const std::string & getFileName() const;
                inline const std::vector<VideoInfo> & getVideo() const;
                inline const std::vector<AudioInfo> & getAudio() const;
                inline const Tags & getTags() const;

                void setFileName(const std::string &);
                void setVideo(const VideoInfo &);
                void setVideo(const std::vector<VideoInfo> &);
                void setAudio(const AudioInfo &);
                void setAudio(const std::vector<AudioInfo> &);
                void setTags(const Tags &);

                bool operator == (const Info &) const;

            private:
                std::string _fileName;
                std::vector<VideoInfo> _video;
                std::vector<AudioInfo> _audio;
                Tags _tags;
            };

            typedef std::pair<Timestamp, std::shared_ptr<Image> > VideoFrame;
            typedef std::pair<Timestamp, std::shared_ptr<Audio::Data> > AudioFrame;

            class ReadQueue : public std::enable_shared_from_this<ReadQueue>
            {
                DJV_NON_COPYABLE(ReadQueue);

            protected:
                ReadQueue();

            public:
                static std::shared_ptr<ReadQueue> create();

                std::mutex & getMutex();

                size_t getVideoFrameCount() const;
                size_t getAudioFrameCount() const;
                bool hasVideoFrames() const;
                bool hasAudioFrames() const;

                void addVideoFrame(Timestamp, const std::shared_ptr<Image> &);
                void addAudioFrame(Timestamp, const std::shared_ptr<Audio::Data> &);

                VideoFrame getVideoFrame() const;
                AudioFrame getAudioFrame() const;
                void popVideoFrame();
                void popAudioFrame();

                void clear();

            private:
                std::list<VideoFrame> _videoFrames;
                std::list<AudioFrame> _audioFrames;
                std::mutex _mutex;
            };

            class IRead : public std::enable_shared_from_this<IRead>
            {
                DJV_NON_COPYABLE(IRead);

            protected:
                void _init(
                    const std::string & fileName,
                    const std::shared_ptr<ReadQueue> &,
                    const std::shared_ptr<Core::Context> &);
                IRead();

            public:
                virtual ~IRead() = 0;

                virtual std::future<Info> getInfo() = 0;

                virtual void seek(Timestamp) = 0;

            protected:
                std::weak_ptr<Core::Context> _context;
                std::string _fileName;
                std::shared_ptr<ReadQueue> _queue;
            };

            class IWrite : public std::enable_shared_from_this<IWrite>
            {
                DJV_NON_COPYABLE(IWrite);

            protected:
                void _init(const std::string &, const Info &, const std::shared_ptr<Core::Context> &);
                IWrite();

            public:                
                virtual ~IWrite() = 0;

            protected:
                std::weak_ptr<Core::Context> _context;
                std::string _fileName;
                Info _info;
            };

            class IPlugin : public std::enable_shared_from_this<IPlugin>
            {
                DJV_NON_COPYABLE(IPlugin);

            protected:
                void _init(
                    const std::string & pluginName,
                    const std::string & pluginInfo,
                    const std::set<std::string> & fileExtensions,
                    const std::shared_ptr<Core::Context> &);
                IPlugin();

            public:
                virtual ~IPlugin() = 0;

                inline const std::string & getPluginName() const;
                inline const std::string & getPluginInfo() const;
                inline const std::set<std::string> & getFileExtensions() const;

                virtual bool canRead(const std::string &) const;
                virtual bool canWrite(const std::string &, const Info &) const;

                virtual picojson::value getOptions() const;

                //! Throws:
                //! - std::exception
                virtual void setOptions(const picojson::value &);

                //! Throws:
                //! - std::exception
                virtual std::shared_ptr<IRead> read(
                    const std::string & fileName,
                    const std::shared_ptr<ReadQueue> &) const { return nullptr; }

                //! Throws:
                //! - std::exception
                virtual std::shared_ptr<IWrite> write(
                    const std::string & fileName,
                    const Info &) const { return nullptr; }

            protected:
                std::weak_ptr<Core::Context> _context;
                std::string _pluginName;
                std::string _pluginInfo;
                std::set<std::string> _fileExtensions;
            };

            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);

            protected:
                void _init(const std::shared_ptr<Core::Context> &);
                System();

            public:
                virtual ~System();

                static std::shared_ptr<System> create(const std::shared_ptr<Core::Context> &);

                virtual picojson::value getOptions(const std::string & pluginName) const;

                //! Throws:
                //! - std::exception
                virtual void setOptions(const std::string & pluginName, const picojson::value &);

                bool canRead(const std::string &) const;
                bool canWrite(const std::string &, const Info &) const;

                //! Throws:
                //! - std::exception
                std::shared_ptr<IRead> read(
                    const std::string & fileName,
                    const std::shared_ptr<ReadQueue> &);

                //! Throws:
                //! - std::exception
                std::shared_ptr<IWrite> write(
                    const std::string & fileName,
                    const Info &);

            protected:
                void _exit() override;

            private:
                struct Private;
                std::unique_ptr<Private> _p;
            };

        } // namespace IO
    } // namespace AV
} // namespace djv

#include <djvAV/IOInline.h>
