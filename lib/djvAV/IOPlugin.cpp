// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/IOPlugin.h>

#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            void IIO::_init(
                const FileSystem::FileInfo& fileInfo,
                const IOOptions& options,
                const std::shared_ptr<TextSystem>& textSystem,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                _logSystem      = logSystem;
                _textSystem     = textSystem;
                _resourceSystem = resourceSystem;
                _fileInfo       = fileInfo;
                _videoQueue.setMax(options.videoQueueSize);
                _audioQueue.setMax(options.audioQueueSize);
            }

            IIO::~IIO()
            {}

            void IIO::setThreadCount(size_t value)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _threadCount = value;
            }

            void IRead::_init(
                const FileSystem::FileInfo& fileInfo,
                const ReadOptions& options,
                const std::shared_ptr<TextSystem>& textSystem,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                IIO::_init(fileInfo, options, textSystem, resourceSystem, logSystem);
                _options = options;
            }

            IRead::~IRead()
            {}

            void IRead::setPlayback(bool value)
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _playback = value;
            }

            void IRead::setLoop(bool value)
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _loop = value;
            }
            
            void IRead::setInOutPoints(const InOutPoints& value)
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _inOutPoints = value;
            }
            
            bool IRead::isCacheEnabled() const
            {
                return _cacheEnabled;
            }

            size_t IRead::getCacheMaxByteCount() const
            {
                return _cacheMaxByteCount;
            }

            size_t IRead::getCacheByteCount()
            {
                std::lock_guard<std::mutex> lock(_mutex);
                return _cacheByteCount;
            }

            Frame::Sequence IRead::getCacheSequence()
            {
                std::lock_guard<std::mutex> lock(_mutex);
                return _cacheSequence;
            }

            Frame::Sequence IRead::getCachedFrames()
            {
                std::lock_guard<std::mutex> lock(_mutex);
                return _cachedFrames;
            }

            void IRead::setCacheEnabled(bool value)
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _cacheEnabled = value;
            }

            void IRead::setCacheMaxByteCount(size_t value)
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _cacheMaxByteCount = value;
            }

            void IWrite::_init(
                const FileSystem::FileInfo& fileInfo,
                const Info& info,
                const WriteOptions& options,
                const std::shared_ptr<TextSystem>& textSystem,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                IIO::_init(fileInfo, options, textSystem, resourceSystem, logSystem);
                _info = info;
            }

            IWrite::~IWrite()
            {}

            void IPlugin::_init(
                const std::string& pluginName,
                const std::string& pluginInfo,
                const std::set<std::string>& fileExtensions,
                const std::shared_ptr<Context>& context)
            {
                _context        = context;
                _logSystem      = context->getSystemT<LogSystem>();
                _resourceSystem = context->getSystemT<ResourceSystem>();
                _textSystem     = context->getSystemT<TextSystem>();
                _pluginName     = pluginName;
                _pluginInfo     = pluginInfo;
                _fileExtensions = fileExtensions;
            }

            IPlugin::~IPlugin()
            {}

            bool IPlugin::canSequence() const
            {
                return false;
            }

            namespace
            {
                bool checkExtension(const FileSystem::FileInfo& fileInfo, const std::set<std::string>& extensions)
                {
                    std::string extension = fileInfo.getPath().getExtension();
                    std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
                    return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
                }

            } // namespace

            bool IPlugin::canRead(const FileSystem::FileInfo& fileInfo) const
            {
                return checkExtension(fileInfo, _fileExtensions);
            }

            bool IPlugin::canWrite(const FileSystem::FileInfo& fileInfo, const Info&) const
            {
                return checkExtension(fileInfo, _fileExtensions);
            }

            rapidjson::Value IPlugin::getOptions(rapidjson::Document::AllocatorType&) const
            {
                return rapidjson::Value();
            }

            void IPlugin::setOptions(const rapidjson::Value&)
            {
                // Default implementation does nothing.
            }

            std::shared_ptr<IRead> IPlugin::read(const FileSystem::FileInfo&, const ReadOptions&) const
            {
                return nullptr;
            }

            std::shared_ptr<IWrite> IPlugin::write(const FileSystem::FileInfo&, const Info&, const WriteOptions&) const
            {
                return nullptr;
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
