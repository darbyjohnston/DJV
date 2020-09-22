// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/IO.h>

#include <djvSystem/FileInfo.h>

namespace djv
{
    namespace System
    {
        class Context;
        class LogSystem;
        class ResourceSystem;
        class TextSystem;

    } // namespace System

    namespace AV
    {
        namespace IO
        {
            //! This class provides I/O plugin options.
            struct IOOptions
            {
                size_t videoQueueSize = 1;
                //! \todo What is a good default for this value?
                size_t audioQueueSize = 30;
            };

            //! This class provides the base interface for I/O.
            class IIO : public std::enable_shared_from_this<IIO>
            {
            protected:
                void _init(
                    const System::File::Info&,
                    const IOOptions&,
                    const std::shared_ptr<System::TextSystem>&,
                    const std::shared_ptr<System::ResourceSystem>&,
                    const std::shared_ptr<System::LogSystem>&);

            public:
                virtual ~IIO() = 0;

                virtual bool isRunning() const = 0;

                size_t getThreadCount() const;

                void setThreadCount(size_t);

                std::mutex& getMutex();
                VideoQueue& getVideoQueue();
                AudioQueue& getAudioQueue();

            protected:
                std::shared_ptr<System::LogSystem> _logSystem;
                std::shared_ptr<System::ResourceSystem> _resourceSystem;
                std::shared_ptr<System::TextSystem> _textSystem;
                System::File::Info _fileInfo;
                std::mutex _mutex;
                VideoQueue _videoQueue;
                AudioQueue _audioQueue;
                size_t _threadCount = 4;
            };

            //! This class provides options for reading.
            struct ReadOptions : IOOptions
            {
                ReadOptions();
                
                size_t layer = 0;
                std::string colorSpace;
            };

            //! This class provides an interface for reading.
            class IRead : public IIO
            {
            protected:
                void _init(
                    const System::File::Info&,
                    const ReadOptions&,
                    const std::shared_ptr<System::TextSystem>&,
                    const std::shared_ptr<System::ResourceSystem>&,
                    const std::shared_ptr<System::LogSystem>&);

            public:
                virtual ~IRead() = 0;

                virtual std::future<Info> getInfo() = 0;

                void setPlayback(bool);
                void setLoop(bool);
                void setInOutPoints(const InOutPoints&);

                //! \param value For video files this value represents the
                //! frame number, for audio files it represents the audio sample.
                virtual void seek(int64_t value, Direction) = 0;

                virtual bool hasCache() const;
                bool isCacheEnabled() const;
                size_t getCacheMaxByteCount() const;
                size_t getCacheByteCount();
                Math::Frame::Sequence getCacheSequence();
                Math::Frame::Sequence getCachedFrames();

                void setCacheEnabled(bool);
                void setCacheMaxByteCount(size_t);

            protected:
                ReadOptions _options;
                InOutPoints _inOutPoints;
                Direction _direction = Direction::Forward;
                bool _playback = false;
                bool _loop = false;
                bool _cacheEnabled = false;
                size_t _cacheMaxByteCount = 0;
                size_t _cacheByteCount = 0;
                Math::Frame::Sequence _cacheSequence;
                Math::Frame::Sequence _cachedFrames;
                Cache _cache;
            };

            //! This class provides options for writing.
            struct WriteOptions : IOOptions
            {
                std::string colorSpace;
            };

            //! This class provides an interface for writing.
            class IWrite : public IIO
            {
            protected:
                void _init(
                    const System::File::Info&,
                    const Info&,
                    const WriteOptions&,
                    const std::shared_ptr<System::TextSystem>&,
                    const std::shared_ptr<System::ResourceSystem>&,
                    const std::shared_ptr<System::LogSystem>&);

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
                    const std::string& pluginName,
                    const std::string& pluginInfo,
                    const std::set<std::string>& fileExtensions,
                    const std::shared_ptr<System::Context>&);

            public:
                virtual ~IPlugin() = 0;

                const std::string& getPluginName() const;
                const std::string& getPluginInfo() const;
                const std::set<std::string>& getFileExtensions() const;

                virtual bool canSequence() const;
                virtual bool canRead(const System::File::Info&) const;
                virtual bool canWrite(const System::File::Info&, const Info&) const;

                virtual rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const;

                //! Throws:
                //! - std::exception
                virtual void setOptions(const rapidjson::Value&);

                //! Throws:
                //! - std::exception
                virtual std::shared_ptr<IRead> read(const System::File::Info&, const ReadOptions&) const;

                //! Throws:
                //! - std::exception
                virtual std::shared_ptr<IWrite> write(const System::File::Info&, const Info&, const WriteOptions&) const;

            protected:
                std::weak_ptr<System::Context> _context;
                std::shared_ptr<System::LogSystem> _logSystem;
                std::shared_ptr<System::ResourceSystem> _resourceSystem;
                std::shared_ptr<System::TextSystem> _textSystem;
                std::string _pluginName;
                std::string _pluginInfo;
                std::set<std::string> _fileExtensions;
            };

        } // namespace IO
    } // namespace AV
} // namespace djv

#include <djvAV/IOPluginInline.h>
