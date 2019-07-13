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

#include <djvAV/IO.h>

#include <djvAV/Cineon.h>
#include <djvAV/DPX.h>
#include <djvAV/PPM.h>
#if defined(FFMPEG_FOUND)
#include <djvAV/FFmpeg.h>
#endif // FFMPEG_FOUND
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
#include <djvCore/CoreSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/Path.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            VideoInfo::VideoInfo()
            {}

            VideoInfo::VideoInfo(const Image::Info & info, const Time::Speed & speed, Time::Timestamp duration) :
                info(info),
                speed(speed),
                duration(duration)
            {}

            bool VideoInfo::operator == (const VideoInfo & other) const
            {
                return info == other.info && speed == other.speed && duration == other.duration;
            }

            AudioInfo::AudioInfo()
            {}

            AudioInfo::AudioInfo(const Audio::DataInfo & info, Time::Timestamp duration) :
                info(info),
                duration(duration)
            {}

            bool AudioInfo::operator == (const AudioInfo & other) const
            {
                return info == other.info && duration == other.duration;
            }

            Info::Info()
            {}

            Info::Info(const std::string & fileName, const VideoInfo & video) :
                fileName(fileName)
            {
                this->video.push_back(video);
            }

            Info::Info(const std::string & fileName, const AudioInfo & audio) :
                fileName(fileName)
            {
                this->audio.push_back(audio);
            }

            Info::Info(const std::string & fileName, const VideoInfo & video, const AudioInfo & audio) :
                fileName(fileName)
            {
                this->video.push_back(video);
                this->audio.push_back(audio);
            }

            Info::Info(const std::string & fileName, const std::vector<VideoInfo> & video, const std::vector<AudioInfo> & audio) :
                fileName(fileName),
                video(video),
                audio(audio)
            {}

            bool Info::operator == (const Info & other) const
            {
                return
                    fileName == other.fileName &&
                    video == other.video &&
                    audio == other.audio &&
                    tags == other.tags;
            }

            VideoFrame::VideoFrame()
            {}

            VideoFrame::VideoFrame(Time::Timestamp timestamp, const std::shared_ptr<Image::Image>& image) :
                timestamp(timestamp),
                image(image)
            {}

            VideoQueue::VideoQueue()
            {}

            void VideoQueue::setMax(size_t value)
            {
                _max = value;
            }

            void VideoQueue::addFrame(const VideoFrame& value)
            {
                _queue.push_back(value);
            }

            VideoFrame VideoQueue::popFrame()
            {
                VideoFrame out;
                if (_queue.size())
                {
                    out = _queue.front();
                    _queue.pop_front();
                }
                return out;
            }

            void VideoQueue::clearFrames()
            {
                _queue.clear();
            }

            void VideoQueue::setFinished(bool value)
            {
                _finished = value;
            }

            AudioFrame::AudioFrame()
            {}

            AudioFrame::AudioFrame(Time::Timestamp timestamp, const std::shared_ptr<Audio::Data>& audio) :
                timestamp(timestamp),
                audio(audio)
            {}

            AudioQueue::AudioQueue()
            {}

            void AudioQueue::setMax(size_t value)
            {
                _max = value;
            }

            void AudioQueue::addFrame(const AudioFrame& value)
            {
                _queue.push_back(value);
            }

            AudioFrame AudioQueue::popFrame()
            {
                AudioFrame out;
                if (_queue.size())
                {
                    out = _queue.front();
                    _queue.pop_front();
                }
                return out;
            }

            void AudioQueue::clearFrames()
            {
                _queue.clear();
            }

            void AudioQueue::setFinished(bool value)
            {
                _finished = value;
            }

            void IIO::_init(
                const FileSystem::FileInfo& fileInfo,
                const IOOptions& options,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                _logSystem      = logSystem;
                _resourceSystem = resourceSystem;
                _fileInfo       = fileInfo;
                _videoQueue.setMax(options.videoQueueSize);
                _audioQueue.setMax(options.audioQueueSize);
            }

            IIO::IIO()
            {}

            IIO::~IIO()
            {}

            void IRead::_init(
                const FileSystem::FileInfo & fileInfo,
                const ReadOptions& options,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                IIO::_init(fileInfo, options, resourceSystem, logSystem);
                _options = options;
            }

            IRead::IRead()
            {}

            IRead::~IRead()
            {}

            void IRead::seek(Time::Timestamp)
            {}

            void IWrite::_init(
                const FileSystem::FileInfo& fileInfo,
                const Info & info,
                const WriteOptions& options,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                IIO::_init(fileInfo, options, resourceSystem, logSystem);
                _info = info;
            }

            IWrite::IWrite()
            {}

            IWrite::~IWrite()
            {}

            void IPlugin::_init(
                const std::string & pluginName,
                const std::string & pluginInfo,
                const std::set<std::string> & fileExtensions,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                _logSystem      = logSystem;
                _resourceSystem = resourceSystem;
                _pluginName     = pluginName;
                _pluginInfo     = pluginInfo;
                _fileExtensions = fileExtensions;
            }

            IPlugin::IPlugin()
            {}

            IPlugin::~IPlugin()
            {}

            namespace
            {
                bool checkExtension(const std::string & value, const std::set<std::string> & extensions)
                {
                    std::string extension = FileSystem::Path(value).getExtension();
                    std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
                    return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
                }

            } // namespace

            bool IPlugin::canRead(const FileSystem::FileInfo& fileInfo) const
            {
                return checkExtension(fileInfo, _fileExtensions);
            }

            bool IPlugin::canWrite(const FileSystem::FileInfo& fileInfo, const Info & info) const
            {
                return checkExtension(fileInfo, _fileExtensions);
            }

            picojson::value IPlugin::getOptions() const
            {
                return picojson::value(std::string());
            }

            void IPlugin::setOptions(const picojson::value &)
            {}

            std::shared_ptr<IRead> IPlugin::read(const FileSystem::FileInfo& fileInfo, const ReadOptions&) const
            {
                return nullptr;
            }

            std::shared_ptr<IWrite> IPlugin::write(const FileSystem::FileInfo& fileInfo, const Info&, const WriteOptions&) const
            {
                return nullptr;
            }

            struct System::Private
            {
                std::shared_ptr<ValueSubject<bool> > optionsChanged;
                std::map<std::string, std::shared_ptr<IPlugin> > plugins;
            };

            void System::_init(Context * context)
            {
                ISystem::_init("djv::AV::IO::System", context);

                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<CoreSystem>());

                p.optionsChanged = ValueSubject<bool>::create();

                auto logSystem = context->getSystemT<LogSystem>();
                auto resourceSystem = context->getSystemT<ResourceSystem>();
                p.plugins[Cineon::pluginName] = Cineon::Plugin::create(resourceSystem, logSystem);
                p.plugins[DPX::pluginName] = DPX::Plugin::create(resourceSystem, logSystem);
                p.plugins[PPM::pluginName] = PPM::Plugin::create(resourceSystem, logSystem);
#if defined(FFMPEG_FOUND)
                p.plugins[FFmpeg::pluginName] = FFmpeg::Plugin::create(resourceSystem, logSystem);
#endif // FFMPEG_FOUND
#if defined(JPEG_FOUND)
                p.plugins[JPEG::pluginName] = JPEG::Plugin::create(resourceSystem, logSystem);
#endif // JPEG_FOUND
#if defined(PNG_FOUND)
                p.plugins[PNG::pluginName] = PNG::Plugin::create(resourceSystem, logSystem);
#endif // PNG_FOUND
#if defined(OPENEXR_FOUND)
                p.plugins[OpenEXR::pluginName] = OpenEXR::Plugin::create(resourceSystem, logSystem);
#endif // OPENEXR_FOUND
#if defined(TIFF_FOUND)
                p.plugins[TIFF::pluginName] = TIFF::Plugin::create(resourceSystem, logSystem);
#endif // TIFF_FOUND

                for (const auto & i : p.plugins)
                {
                    std::stringstream s;
                    s << "I/O plugin: " << i.second->getPluginName() << '\n';
                    s << "    Information: " << i.second->getPluginInfo() << '\n';
                    s << "    File extensions: " << String::joinSet(i.second->getFileExtensions(), ", ") << '\n';
                    _log(s.str());
                }
            }

            System::System() :
                _p(new Private)
            {}

            System::~System()
            {}

            std::shared_ptr<System> System::create(Context * context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(context);
                return out;
            }

            std::vector<std::string> System::getPluginNames() const
            {
                DJV_PRIVATE_PTR();
                std::vector<std::string> out;
                for (const auto & i : p.plugins)
                {
                    out.push_back(i.second->getPluginName());
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
                for (const auto & i : p.plugins)
                {
                    if (i.second->canRead(fileInfo))
                    {
                        return i.second->read(fileInfo, options);
                    }
                }
                std::stringstream s;
                s << "The file" << " '" << fileInfo << "' " << "cannot be read" << ".";
                throw std::runtime_error(s.str());
                return nullptr;
            }

            std::shared_ptr<IWrite> System::write(const FileSystem::FileInfo& fileInfo, const Info & info, const WriteOptions& options)
            {
                DJV_PRIVATE_PTR();
                for (const auto & i : p.plugins)
                {
                    if (i.second->canWrite(fileInfo, info))
                    {
                        return i.second->write(fileInfo, info, options);
                    }
                }
                std::stringstream s;
                s << "The file" << " '" << fileInfo << "' " << "cannot be written" << ".";
                throw std::runtime_error(s.str());
                return nullptr;
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
