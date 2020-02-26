//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvAV/IO.h>

#include <djvAV/Cineon.h>
#include <djvAV/DPX.h>
#include <djvAV/GLFWSystem.h>
#include <djvAV/IFF.h>
#include <djvAV/PPM.h>
#include <djvAV/RLA.h>
#include <djvAV/SGI.h>
#include <djvAV/Targa.h>

#if defined(FFmpeg_FOUND)
#include <djvAV/FFmpeg.h>
#endif // FFmpeg_FOUND
#if defined(JPEG_FOUND)
#include <djvAV/JPEG.h>
#endif // JPEG_FOUND
#if defined(OPENEXR_FOUND)
#include <djvAV/OpenEXR.h>
#endif // OPENEXR_FOUND
#if defined(PNG_FOUND)
#include <djvAV/PNG.h>
#endif // PNG_FOUND
#if defined(TIFF_FOUND)
#include <djvAV/TIFF.h>
#endif // TIFF_FOUND

#include <djvCore/Context.h>
#include <djvCore/FileSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/Path.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/String.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            void VideoQueue::setMax(size_t value)
            {
                _max = value;
            }

            void VideoQueue::addFrame(const VideoFrame& value)
            {
                _queue.push(value);
            }

            VideoFrame VideoQueue::popFrame()
            {
                VideoFrame out;
                if (_queue.size())
                {
                    out = _queue.front();
                    _queue.pop();
                }
                return out;
            }

            void VideoQueue::clearFrames()
            {
                while (_queue.size())
                {
                    _queue.pop();
                }
            }

            void VideoQueue::setFinished(bool value)
            {
                _finished = value;
            }

            void AudioQueue::setMax(size_t value)
            {
                _max = value;
            }

            void AudioQueue::addFrame(const AudioFrame& value)
            {
                _queue.push(value);
            }

            AudioFrame AudioQueue::popFrame()
            {
                AudioFrame out;
                if (_queue.size())
                {
                    out = _queue.front();
                    _queue.pop();
                }
                return out;
            }

            void AudioQueue::clearFrames()
            {
                while (_queue.size())
                {
                    _queue.pop();
                }
            }

            void AudioQueue::setFinished(bool value)
            {
                _finished = value;
            }

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

            Frame::Sequence Cache::getFrames() const
            {
                Frame::Sequence out;
                std::vector<Frame::Index> frames;
                for (const auto& i : _cache)
                {
                    frames.push_back(i.first);
                }
                const size_t size = frames.size();
                if (size)
                {
                    std::sort(frames.begin(), frames.end());
                    Frame::Number rangeStart = frames[0];
                    Frame::Number prevFrame = frames[0];
                    size_t i = 1;
                    for (; i < size; prevFrame = frames[i], ++i)
                    {
                        if (frames[i] != prevFrame + 1)
                        {
                            out.ranges.push_back(Frame::Range(rangeStart, prevFrame));
                            rangeStart = frames[i];
                        }
                    }
                    if (size > 1)
                    {
                        out.ranges.push_back(Frame::Range(rangeStart, prevFrame));
                    }
                    else
                    {
                        out.ranges.push_back(Frame::Range(rangeStart));
                    }
                }
                return out;
            }

            void Cache::setMax(size_t value)
            {
                if (value == _max)
                    return;
                _max = value;
                _cacheUpdate();
            }

            void Cache::setSequenceSize(size_t value)
            {
                if (value == _sequenceSize)
                    return;
                _sequenceSize = value;
                _cacheUpdate();
            }

            void Cache::setInOutPoints(const InOutPoints& value)
            {
                if (value == _inOutPoints)
                    return;
                _inOutPoints = value;
                _cacheUpdate();
            }

            void Cache::setDirection(Direction value)
            {
                if (value == _direction)
                    return;
                _direction = value;
                _cacheUpdate();
            }

            void Cache::setCurrentFrame(Frame::Index value)
            {
                if (value == _currentFrame)
                    return;
                _currentFrame = value;
                _cacheUpdate();
            }

            void Cache::add(Frame::Index index, const std::shared_ptr<AV::Image::Image>& image)
            {
                _cache[index] = image;
                _cacheUpdate();
            }

            void Cache::_cacheUpdate()
            {
                const auto range = _inOutPoints.getRange(_sequenceSize);
                Frame::Index frame = _currentFrame;
                _sequence = Frame::Sequence();
                switch (_direction)
                {
                case Direction::Forward:
                {
                    for (size_t i = 0; i < _readBehind; ++i)
                    {
                        --frame;
                        if (frame < range.min)
                        {
                            frame = range.max;
                        }
                    }
                    _sequence.ranges.push_back(Frame::Range(frame));
                    const Frame::Index first = frame;
                    for (size_t i = 0; i < _max; ++i)
                    {
                        ++frame;
                        if (first == frame)
                        {
                            break;
                        }
                        if (frame > range.max)
                        {
                            frame = range.min;
                            if (frame != _sequence.ranges.back().max)
                            {
                                _sequence.ranges.push_back(Frame::Range(frame));
                            }
                        }
                        else
                        {
                            _sequence.ranges.back().max = frame;
                        }
                    }
                    break;
                }
                case Direction::Reverse:
                {
                    for (size_t i = 0; i < _readBehind; ++i)
                    {
                        ++frame;
                        if (frame > range.max)
                        {
                            frame = range.min;
                        }
                    }
                    _sequence.ranges.push_back(Frame::Range(frame));
                    const Frame::Index first = frame;
                    for (size_t i = 0; i < _max; ++i)
                    {
                        --frame;
                        if (first == frame)
                        {
                            break;
                        }
                        if (frame < range.min)
                        {
                            frame = range.max;
                            if (frame != _sequence.ranges.back().max)
                            {
                                _sequence.ranges.push_back(Frame::Range(frame));
                            }
                        }
                        else
                        {
                            _sequence.ranges.back().min = frame;
                        }
                    }
                    break;
                }
                default: break;
                }
                auto i = _cache.begin();
                while (i != _cache.end())
                {
                    auto j = i;
                    ++i;
                    if (!_sequence.contains(j->first))
                    {
                        _cache.erase(j);
                    }
                }
            }

            void IRead::_init(
                const FileSystem::FileInfo & fileInfo,
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
                const Info & info,
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
                bool checkExtension(const std::string& value, const std::set<std::string>& extensions)
                {
                    std::string extension = FileSystem::Path(value).getExtension();
                    std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
                    return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
                }

            } // namespace

            bool IPlugin::canRead(const FileSystem::FileInfo& fileInfo) const
            {
                return checkExtension(std::string(fileInfo), _fileExtensions);
            }

            bool IPlugin::canWrite(const FileSystem::FileInfo& fileInfo, const Info&) const
            {
                return checkExtension(std::string(fileInfo), _fileExtensions);
            }

            picojson::value IPlugin::getOptions() const
            {
                return picojson::value(std::string());
            }

            void IPlugin::setOptions(const picojson::value&)
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

            struct System::Private
            {
                std::shared_ptr<TextSystem> textSystem;
                std::shared_ptr<ValueSubject<bool> > optionsChanged;
                std::map<std::string, std::shared_ptr<IPlugin> > plugins;
                std::set<std::string> sequenceExtensions;
            };

            void System::_init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("djv::AV::IO::System", context);

                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<GLFW::System>());

                p.textSystem = context->getSystemT<TextSystem>();

                p.optionsChanged = ValueSubject<bool>::create();

                p.plugins[Cineon::pluginName] = Cineon::Plugin::create(context);
                p.plugins[DPX::pluginName] = DPX::Plugin::create(context);
                p.plugins[IFF::pluginName] = IFF::Plugin::create(context);
                p.plugins[PPM::pluginName] = PPM::Plugin::create(context);
                p.plugins[RLA::pluginName] = RLA::Plugin::create(context);
                p.plugins[SGI::pluginName] = SGI::Plugin::create(context);
                p.plugins[Targa::pluginName] = Targa::Plugin::create(context);
#if defined(FFmpeg_FOUND)
                p.plugins[FFmpeg::pluginName] = FFmpeg::Plugin::create(context);
#endif // FFmpeg_FOUND
#if defined(JPEG_FOUND)
                p.plugins[JPEG::pluginName] = JPEG::Plugin::create(context);
#endif // JPEG_FOUND
#if defined(PNG_FOUND)
                p.plugins[PNG::pluginName] = PNG::Plugin::create(context);
#endif // PNG_FOUND
#if defined(OPENEXR_FOUND)
                p.plugins[OpenEXR::pluginName] = OpenEXR::Plugin::create(context);
#endif // OPENEXR_FOUND
#if defined(TIFF_FOUND)
                p.plugins[TIFF::pluginName] = TIFF::Plugin::create(context);
#endif // TIFF_FOUND

                for (const auto & i : p.plugins)
                {
                    if (i.second->canSequence())
                    {
                        const auto& fileExtensions = i.second->getFileExtensions();
                        p.sequenceExtensions.insert(fileExtensions.begin(), fileExtensions.end());
                    }
                
                    std::stringstream ss;
                    ss << "I/O plugin: " << i.second->getPluginName() << '\n';
                    ss << "    Information: " << i.second->getPluginInfo() << '\n';
                    ss << "    File extensions: " << String::joinSet(i.second->getFileExtensions(), ", ") << '\n';
                    _log(ss.str());
                }
            }

            System::System() :
                _p(new Private)
            {}

            System::~System()
            {}

            std::shared_ptr<System> System::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(context);
                return out;
            }

            std::set<std::string> System::getPluginNames() const
            {
                DJV_PRIVATE_PTR();
                std::set<std::string> out;
                for (const auto & i : p.plugins)
                {
                    out.insert(i.second->getPluginName());
                }
                return out;
            }

            std::set<std::string> System::getFileExtensions() const
            {
                DJV_PRIVATE_PTR();
                std::set<std::string> out;
                for (const auto& i : p.plugins)
                {
                    const auto& fileExtensions = i.second->getFileExtensions();
                    out.insert(fileExtensions.begin(), fileExtensions.end());
                }
                return out;
            }

            picojson::value System::getOptions(const std::string & pluginName) const
            {
                DJV_PRIVATE_PTR();
                const auto i = p.plugins.find(pluginName);
                if (i != p.plugins.end())
                {
                    return i->second->getOptions();
                }
                return picojson::value();
            }

            void System::setOptions(const std::string & pluginName, const picojson::value & value)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.plugins.find(pluginName);
                if (i != p.plugins.end())
                {
                    i->second->setOptions(value);
                    p.optionsChanged->setAlways(true);
                }
            }

            std::shared_ptr<IValueSubject<bool> > System::observeOptionsChanged() const
            {
                return _p->optionsChanged;
            }

            const std::set<std::string>& System::getSequenceExtensions() const
            {
                return _p->sequenceExtensions;
            }
                
            bool System::canSequence(const FileSystem::FileInfo& fileInfo) const
            {
                DJV_PRIVATE_PTR();
                return std::find(
                    p.sequenceExtensions.begin(),
                    p.sequenceExtensions.end(),
                    fileInfo.getPath().getExtension()) != p.sequenceExtensions.end();
            }

            bool System::canRead(const FileSystem::FileInfo& fileInfo) const
            {
                DJV_PRIVATE_PTR();
                for (const auto & i : p.plugins)
                {
                    if (i.second->canRead(fileInfo))
                    {
                        return true;
                    }
                }
                return false;
            }

            bool System::canWrite(const FileSystem::FileInfo& fileInfo, const Info & info) const
            {
                DJV_PRIVATE_PTR();
                for (const auto & i : p.plugins)
                {
                    if (i.second->canWrite(fileInfo, info))
                    {
                        return true;
                    }
                }
                return false;
            }

            std::shared_ptr<IRead> System::read(const FileSystem::FileInfo& fileInfo, const ReadOptions& options)
            {
                DJV_PRIVATE_PTR();
                std::shared_ptr<IRead> out;
                for (const auto & i : p.plugins)
                {
                    if (i.second->canRead(fileInfo))
                    {
                        out = i.second->read(fileInfo, options);
                        break;
                    }
                }
                if (!out)
                {
                    std::stringstream ss;
                    ss << p.textSystem->getText(DJV_TEXT("error_the_file"));
                    ss << " '" << fileInfo << "' ";
                    ss << p.textSystem->getText(DJV_TEXT("error_cannot_be_read")) << ".";
                    throw FileSystem::Error(ss.str());
                }
                return out;
            }

            std::shared_ptr<IWrite> System::write(const FileSystem::FileInfo& fileInfo, const Info & info, const WriteOptions& options)
            {
                DJV_PRIVATE_PTR();
                std::shared_ptr<IWrite> out;
                for (const auto & i : p.plugins)
                {
                    if (i.second->canWrite(fileInfo, info))
                    {
                        out = i.second->write(fileInfo, info, options);
                        break;
                    }
                }
                if (!out)
                {
                    std::stringstream ss;
                    ss << p.textSystem->getText(DJV_TEXT("error_the_file"));
                    ss << " '" << fileInfo << "' ";
                    ss << p.textSystem->getText(DJV_TEXT("error_cannot_be_written")) << ".";
                    throw FileSystem::Error(ss.str());
                }
                return out;
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
