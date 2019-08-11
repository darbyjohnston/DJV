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

#pragma once

#include <djvAV/AudioData.h>
#include <djvAV/Image.h>
#include <djvAV/Tags.h>

#include <djvCore/Cache.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
#include <djvCore/ISystem.h>
#include <djvCore/PicoJSON.h>
#include <djvCore/Speed.h>
#include <djvCore/Time.h>
#include <djvCore/ValueObserver.h>

#include <future>
#include <mutex>
#include <set>
#include <list>

namespace djv
{
    namespace Core
    {
        class LogSystem;
        class ResourceSystem;

    } // namespace Core

    namespace AV
    {
        //! This namespace provides I/O functionality.
        namespace IO
        {
            //! This class provides video I/O information.
            struct VideoInfo
            {
                VideoInfo();
                VideoInfo(
                    const Image::Info &,
                    const Core::Time::Speed & = Core::Time::Speed(),
                    const Core::Frame::Sequence& = Core::Frame::Sequence());

                Image::Info info;
                Core::Time::Speed speed;
                Core::Frame::Sequence sequence;
                std::string codec;

                bool operator == (const VideoInfo &) const;
            };

            //! This class provides audio I/O information.
            struct AudioInfo
            {
                AudioInfo();
                AudioInfo(const Audio::DataInfo &, size_t sampleCount = 0);

                Audio::DataInfo info;
                size_t sampleCount = 0;
                std::string codec;

                bool operator == (const AudioInfo &) const;
            };

            //! This class provides I/O information.
            struct Info
            {
                Info();
                Info(const std::string & fileName, const VideoInfo &);
                Info(const std::string & fileName, const AudioInfo &);
                Info(const std::string & fileName, const VideoInfo &, const AudioInfo &);
                Info(const std::string & fileName, const std::vector<VideoInfo> &, const std::vector<AudioInfo> &);

                std::string fileName;
                std::vector<VideoInfo> video;
                std::vector<AudioInfo> audio;
                Tags tags;

                bool operator == (const Info &) const;
            };

            //! This struct provides a video frame.
            struct VideoFrame
            {
                VideoFrame();
                VideoFrame(Core::Frame::Number, const std::shared_ptr<Image::Image>&);

                Core::Frame::Number           frame = 0;
                std::shared_ptr<Image::Image> image;
            };

            //! This class provides a queue of video frames.
            class VideoQueue
            {
                DJV_NON_COPYABLE(VideoQueue);

            public:
                VideoQueue();

                inline size_t getMax() const;
                void setMax(size_t);

                inline bool hasFrames() const;
                inline size_t getFrameCount() const;
                inline const std::list<VideoFrame>& getFrames() const;
                inline VideoFrame getFrame() const;
                void addFrame(const VideoFrame&);
                VideoFrame popFrame();
                void clearFrames();

                inline bool isFinished() const;
                void setFinished(bool);

            private:
                size_t _max = 0;
                std::list<VideoFrame> _queue;
                bool _finished = false;
            };

            //! This struct provides an audio frame.
            struct AudioFrame
            {
                AudioFrame();
                AudioFrame(const std::shared_ptr<Audio::Data>&);

                std::shared_ptr<Audio::Data> audio;
            };

            //! This class provides a queue of audio frames.
            class AudioQueue
            {
                DJV_NON_COPYABLE(AudioQueue);

            public:
                AudioQueue();

                inline size_t getMax() const;
                void setMax(size_t);

                inline bool hasFrames() const;
                inline size_t getFrameCount() const;
                inline AudioFrame getFrame() const;
                void addFrame(const AudioFrame &);
                AudioFrame popFrame();
                void clearFrames();

                inline bool isFinished() const;
                void setFinished(bool);

            private:
                std::mutex _mutex;
                size_t _max = 0;
                std::list<AudioFrame> _queue;
                bool _finished = false;
            };

            //! This class provides I/O options.
            struct IOOptions
            {
                size_t videoQueueSize = 1;
                //! \todo What is a good default for this value?
                size_t audioQueueSize = 30;
            };

            //! This class provides an interface for I/O.
            class IIO : public std::enable_shared_from_this<IIO>
            {
            protected:
                void _init(
                    const Core::FileSystem::FileInfo&,
                    const IOOptions&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);

            public:
                virtual ~IIO() = 0;

                virtual bool isRunning() const = 0;

                size_t getThreadCount() const;
                void setThreadCount(size_t);

                inline std::mutex& getMutex();
                inline VideoQueue& getVideoQueue();
                inline AudioQueue& getAudioQueue();

            protected:
                std::shared_ptr<Core::LogSystem> _logSystem;
                std::shared_ptr<Core::ResourceSystem> _resourceSystem;
                Core::FileSystem::FileInfo _fileInfo;
                std::mutex _mutex;
                VideoQueue _videoQueue;
                AudioQueue _audioQueue;
                size_t _threadCount = 4;
            };

            //! This class provides options for reading.
            struct ReadOptions : IOOptions
            {
                size_t layer = 0;
            };

            //! This enumeration provides the playback direction for caching.
            enum class Playback
            {
                Forward,
                Reverse
            };

            //! This class provides an interface for reading.
            class IRead : public IIO
            {
            protected:
                void _init(
                    const Core::FileSystem::FileInfo&,
                    const ReadOptions&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);

            public:
                virtual ~IRead() = 0;

                virtual std::future<Info> getInfo() = 0;

                //! \param value For video files this value represents the
                //! frame number, for audio files it represents the audio sample.
                virtual void seek(int64_t value, Playback) = 0;

                virtual bool hasCache() const { return false; }
                bool isCacheEnabled() const;
                void setCacheEnabled(bool);
                size_t getCacheMax() const;
                void setCacheMax(size_t);
                std::vector<Core::Frame::Range> getCachedFrames();

            protected:
                typedef Core::Memory::Cache<Core::Frame::Number, std::shared_ptr<Image::Image> > MemoryCache;
                static std::vector<Core::Frame::Range> _getCachedFrames(const MemoryCache& cache);

                ReadOptions _options;
                Playback _playback = Playback::Forward;
                bool _cacheEnabled = false;
                size_t _cacheMax = 0;
                std::vector<Core::Frame::Range> _cachedFrames;
                MemoryCache _cache;
            };

            //! This class provides options for writing.
            struct WriteOptions : IOOptions
            {};

            //! This class provides an interface for writing.
            class IWrite : public IIO
            {
            protected:
                void _init(
                    const Core::FileSystem::FileInfo &,
                    const Info &,
                    const WriteOptions&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);

            public:
                virtual ~IWrite() = 0;

            protected:
                Info _info;
                WriteOptions _options;
            };

            //! This class provides an interface for I/O plugins.
            class IPlugin : public std::enable_shared_from_this<IPlugin>
            {
            protected:
                void _init(
                    const std::string & pluginName,
                    const std::string & pluginInfo,
                    const std::set<std::string> & fileExtensions,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);

            public:
                virtual ~IPlugin() = 0;

                inline const std::string & getPluginName() const;
                inline const std::string & getPluginInfo() const;
                inline const std::set<std::string> & getFileExtensions() const;

                virtual bool canSequence() const;
                virtual bool canRead(const Core::FileSystem::FileInfo&) const;
                virtual bool canWrite(const Core::FileSystem::FileInfo&, const Info &) const;

                virtual picojson::value getOptions() const;

                //! Throws:
                //! - std::exception
                virtual void setOptions(const picojson::value &);

                //! Throws:
                //! - std::exception
                virtual std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&, const ReadOptions&) const;

                //! Throws:
                //! - std::exception
                virtual std::shared_ptr<IWrite> write(const Core::FileSystem::FileInfo&, const Info&, const WriteOptions&) const;

            protected:
                std::shared_ptr<Core::LogSystem> _logSystem;
                std::shared_ptr<Core::ResourceSystem> _resourceSystem;
                std::string _pluginName;
                std::string _pluginInfo;
                std::set<std::string> _fileExtensions;
            };

            //! This class provides an I/O system.
            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);

            protected:
                void _init(Core::Context *);
                System();

            public:
                virtual ~System();

                static std::shared_ptr<System> create(Core::Context *);

                std::vector<std::string> getPluginNames() const;

                picojson::value getOptions(const std::string & pluginName) const;

                //! Throws:
                //! - std::exception
                void setOptions(const std::string & pluginName, const picojson::value &);

                std::shared_ptr<Core::IValueSubject<bool> > observeOptionsChanged() const;

                const std::vector<std::string>& getSequenceExtensions() const;
                bool canSequence(const Core::FileSystem::FileInfo&) const;
                bool canRead(const Core::FileSystem::FileInfo&) const;
                bool canWrite(const Core::FileSystem::FileInfo&, const Info &) const;

                //! Throws:
                //! - std::exception
                std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&, const ReadOptions& = ReadOptions());

                //! Throws:
                //! - std::exception
                std::shared_ptr<IWrite> write(const Core::FileSystem::FileInfo&, const Info &, const WriteOptions& = WriteOptions());

            private:
                DJV_PRIVATE();
            };

        } // namespace IO
    } // namespace AV
} // namespace djv

#include <djvAV/IOInline.h>
