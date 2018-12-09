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

#include <QThread>

#include <future>
#include <mutex>
#include <set>
#include <list>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This class provides video I/O information.
            class VideoInfo
            {
            public:
                VideoInfo();
                VideoInfo(const Pixel::Info &, const Speed & = Speed(), Duration duration = 0);

                Pixel::Info info;
                Speed speed;
                Duration duration = 0;

                bool operator == (const VideoInfo &) const;
            };

            //! This class provides audio I/O information.
            class AudioInfo
            {
            public:
                AudioInfo();
                AudioInfo(const Audio::DataInfo &, Duration duration = 0);

                Audio::DataInfo info;
                Duration duration = 0;

                bool operator == (const AudioInfo &) const;
            };

            //! This class provides I/O information.
            class Info
            {
            public:
                Info();
                Info(const std::string & fileName, const VideoInfo &, const AudioInfo &);
                Info(const std::string & fileName, const std::vector<VideoInfo> &, const std::vector<AudioInfo> &);

                std::string fileName;
                std::vector<VideoInfo> video;
                std::vector<AudioInfo> audio;
                Tags tags;

                void setVideo(const VideoInfo &);
                void setAudio(const AudioInfo &);

                bool operator == (const Info &) const;
            };

            //! This typedef provides a video I/O frame.
            typedef std::pair<Timestamp, std::shared_ptr<Image> > VideoFrame;

            //! This typedef provides an audio I/O frame.
            typedef std::pair<Timestamp, std::shared_ptr<Audio::Data> > AudioFrame;

            //! This class provides a queue for video and audio I/O frames.
            class Queue : public std::enable_shared_from_this<Queue>
            {
                DJV_NON_COPYABLE(Queue);

            protected:
                Queue();

            public:
                static std::shared_ptr<Queue> create();

                inline std::mutex & getMutex();

                inline size_t getVideoMax() const;
                inline size_t getAudioMax() const;
                void setVideoMax(size_t);
                void setAudioMax(size_t);

                inline size_t getVideoCount() const;
                inline size_t getAudioCount() const;
                inline bool hasVideo() const;
                inline bool hasAudio() const;

                void addVideo(Timestamp, const std::shared_ptr<Image> &);
                void addAudio(Timestamp, const std::shared_ptr<Audio::Data> &);

                inline VideoFrame getVideo() const;
                inline AudioFrame getAudio() const;
                void popVideo();
                void popAudio();

                void clear();

                inline bool isFinished() const;
                inline bool hasCloseOnFinish() const;
                void setFinished(bool);
                void setCloseOnFinish(bool);

            private:
                std::mutex _mutex;
                size_t _videoMax = 100;
                size_t _audioMax = 100;
                std::list<VideoFrame> _video;
                std::list<AudioFrame> _audio;
                bool _finished = false;
                bool _closeOnFinish = true;
            };

            //! This class provides an interface for reading.
            class IRead : public std::enable_shared_from_this<IRead>
            {
                DJV_NON_COPYABLE(IRead);

            protected:
                void _init(
                    const std::string & fileName,
                    const std::shared_ptr<Queue> &,
                    const std::shared_ptr<Core::Context> &);
                IRead();

            public:
                virtual ~IRead() = 0;

                virtual std::future<Info> getInfo() = 0;

                virtual void seek(Timestamp);

            protected:
                std::weak_ptr<Core::Context> _context;
                std::string _fileName;
                std::shared_ptr<Queue> _queue;
            };

            //! This class provides an interface for writing.
            //!
            //! \todo This class is derived from QThread for compatibility with Qt
            //! OpenGL contexts.
            class IWrite : public QThread, public std::enable_shared_from_this<IWrite>
            {
                DJV_NON_COPYABLE(IWrite);

            protected:
                void _init(
                    const std::string &,
                    const Info &,
                    const std::shared_ptr<Queue> &,
                    const std::shared_ptr<Core::Context> &);
                IWrite();

            public:
                virtual ~IWrite() = 0;

            protected:
                std::weak_ptr<Core::Context> _context;
                std::string _fileName;
                Info _info;
                std::shared_ptr<Queue> _queue;
            };

            //! This class provides an interface for I/O plugins.
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
                    const std::shared_ptr<Queue> &) const { return nullptr; }

                //! Throws:
                //! - std::exception
                virtual std::shared_ptr<IWrite> write(
                    const std::string & fileName,
                    const Info &,
                    const std::shared_ptr<Queue> &) const { return nullptr; }

            protected:
                std::weak_ptr<Core::Context> _context;
                std::string _pluginName;
                std::string _pluginInfo;
                std::set<std::string> _fileExtensions;
            };

            //! This class provides an I/O system.
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
                    const std::shared_ptr<Queue> &);

                //! Throws:
                //! - std::exception
                std::shared_ptr<IWrite> write(
                    const std::string & fileName,
                    const Info &,
                    const std::shared_ptr<Queue> &);

            protected:
                void _exit() override;

            private:
                DJV_PRIVATE();
            };

        } // namespace IO
    } // namespace AV
} // namespace djv

#include <djvAV/IOInline.h>
