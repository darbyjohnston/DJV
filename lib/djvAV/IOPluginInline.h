// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            inline size_t IIO::getThreadCount() const
            {
                return _threadCount;
            }

            inline std::mutex& IIO::getMutex()
            {
                return _mutex;
            }

            inline VideoQueue& IIO::getVideoQueue()
            {
                return _videoQueue;
            }

            inline AudioQueue& IIO::getAudioQueue()
            {
                return  _audioQueue;
            }

            inline bool IRead::hasCache() const
            {
                return false;
            }

            inline bool IRead::isCacheEnabled() const
            {
                return _cacheEnabled;
            }

            inline size_t IRead::getCacheMaxByteCount() const
            {
                return _cacheMaxByteCount;
            }

            inline const std::string& IPlugin::getPluginName() const
            {
                return _pluginName;
            }

            inline const std::string& IPlugin::getPluginInfo() const
            {
                return _pluginInfo;
            }

            inline const std::set<std::string>& IPlugin::getFileExtensions() const
            {
                return _fileExtensions;
            }

            inline bool IPlugin::canSequence() const
            {
                return false;
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
