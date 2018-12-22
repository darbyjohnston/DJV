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

#include <djvAV/IO.h>

#include <djvAV/FFmpeg.h>
#include <djvAV/PPM.h>
#if defined(PNG_FOUND)
#include <djvAV/PNG.h>
#endif // PNG_FOUND

#include <djvCore/Context.h>
#include <djvCore/Path.h>
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

            VideoInfo::VideoInfo(const Pixel::Info & info, const Speed & speed, Duration duration) :
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

            AudioInfo::AudioInfo(const Audio::DataInfo & info, Duration duration) :
                info(info),
                duration(duration)
            {}

            bool AudioInfo::operator == (const AudioInfo & other) const
            {
                return info == other.info && duration == other.duration;
            }

            Info::Info()
            {}

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

            void Info::setVideo(const VideoInfo & info)
            {
                video.clear();
                video.push_back(info);
            }

            void Info::setAudio(const AudioInfo & info)
            {
                audio.clear();
                audio.push_back(info);
            }

            bool Info::operator == (const Info & other) const
            {
                return
                    fileName == other.fileName &&
                    video == other.video &&
                    audio == other.audio &&
                    tags == other.tags;
            }

            Queue::Queue()
            {}

            std::shared_ptr<Queue> Queue::create(size_t videoMax, size_t audioMax)
            {
                auto out = std::shared_ptr<Queue>(new Queue);
                out->_videoMax = videoMax;
                out->_audioMax = audioMax;
                return out;
            }

            void Queue::addVideo(Timestamp ts, const std::shared_ptr<Image> & data)
            {
                _video.push_back(std::make_pair(ts, data));
            }

            void Queue::addAudio(Timestamp ts, const std::shared_ptr<Audio::Data> & data)
            {
                _audio.push_back(std::make_pair(ts, data));
            }

            void Queue::popVideo()
            {
                _video.pop_front();
            }

            void Queue::popAudio()
            {
                _audio.pop_front();
            }

            void Queue::clear()
            {
                _video.clear();
                _audio.clear();
            }

            void Queue::setFinished(bool value)
            {
                _finished = value;
            }

            void Queue::setCloseOnFinish(bool value)
            {
                _closeOnFinish = value;
            }

            void IRead::_init(
                const std::string & fileName,
                const std::shared_ptr<Queue> & queue,
                Context * context)
            {
                _context = context;
                _fileName = fileName;
                _queue = queue;
            }

            IRead::IRead()
            {}

            IRead::~IRead()
            {}

            void IRead::seek(Timestamp)
            {}

            void IWrite::_init(
                const std::string & fileName,
                const Info & info,
                const std::shared_ptr<Queue> & queue,
                Context * context)
            {
                _context = context;
                _fileName = fileName;
                _info = info;
                _queue = queue;
            }

            IWrite::IWrite()
            {}

            IWrite::~IWrite()
            {}

            void IPlugin::_init(
                const std::string & pluginName,
                const std::string & pluginInfo,
                const std::set<std::string> & fileExtensions,
                Context * context)
            {
                _context = context;
                _pluginName = pluginName;
                _pluginInfo = pluginInfo;
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

            bool IPlugin::canRead(const std::string & fileInfo) const
            {
                return checkExtension(fileInfo, _fileExtensions);
            }

            bool IPlugin::canWrite(const std::string & fileInfo, const Info & info) const
            {
                return checkExtension(fileInfo, _fileExtensions);
            }

            picojson::value IPlugin::getOptions() const
            {
                return picojson::value();
            }

            void IPlugin::setOptions(const picojson::value &)
            {}

            struct System::Private
            {
                std::map<std::string, std::shared_ptr<IPlugin> > plugins;
            };

            void System::_init(Context * context)
            {
                ISystem::_init("djv::AV::IO::System", context);

                DJV_PRIVATE_PTR();
                p.plugins[FFmpeg::pluginName] = FFmpeg::Plugin::create(context);
                p.plugins[PPM::pluginName] = PPM::Plugin::create(context);
#if defined(PNG_FOUND)
                p.plugins[PNG::pluginName] = PNG::Plugin::create(context);
#endif // PNG_FOUND

                /*
                p.plugins[Cineon::pluginName].reset(new Cineon::Plugin(context));
                p.plugins[DPX::pluginName].reset(new DPX::Plugin(context));
                p.plugins[IFF::pluginName].reset(new IFF::Plugin(context));
                p.plugins[IFL::pluginName].reset(new IFL::Plugin(context));
                p.plugins[LUT::pluginName].reset(new LUT::Plugin(context));
                p.plugins[PIC::pluginName].reset(new PIC::Plugin(context));
                p.plugins[PPM::pluginName].reset(new PPM::Plugin(context));
                p.plugins[RLA::pluginName].reset(new RLA::Plugin(context));
                p.plugins[SGI::pluginName].reset(new SGI::Plugin(context));
                p.plugins[Targa::pluginName].reset(new Targa::Plugin(context));
#if defined(DJV_THIRD_PARTY_OPENEXR)
                p.plugins[EXR::pluginName].reset(new EXR::Plugin(context));
#endif // DJV_THIRD_PARTY_OPENEXR
#if defined(DJV_THIRD_PARTY_JPEG)
                p.plugins[JPEG::pluginName].reset(new JPEG::Plugin(context));
#endif // DJV_THIRD_PARTY_JPEG
#if defined(DJV_THIRD_PARTY_PNG)
                p.plugins[PNG::pluginName].reset(new PNG::Plugin(context));
#endif // DJV_THIRD_PARTY_PNG
#if defined(DJV_THIRD_PARTY_TIFF)
                p.plugins[TIFFPlugin::pluginName].reset(new TIFFPlugin::Plugin(context));
#endif // DJV_THIRD_PARTY_TIFF
                */

                for (const auto & i : p.plugins)
                {
                    std::stringstream s;
                    s << "Plugin: " << i.second->getPluginName() << '\n';
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

            picojson::value System::getOptions(const std::string & pluginName) const
            {
                DJV_PRIVATE_PTR();
                const auto i = _p->plugins.find(pluginName);
                if (i != _p->plugins.end())
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
                }
            }

            bool System::canRead(const std::string & fileName) const
            {
                DJV_PRIVATE_PTR();
                for (const auto & i : p.plugins)
                {
                    if (i.second->canRead(fileName))
                    {
                        return true;
                    }
                }
                return false;
            }

            bool System::canWrite(const std::string & fileName, const Info & info) const
            {
                DJV_PRIVATE_PTR();
                for (const auto & i : p.plugins)
                {
                    if (i.second->canWrite(fileName, info))
                    {
                        return true;
                    }
                }
                return false;
            }

            std::shared_ptr<IRead> System::read(
                const std::string & fileName,
                const std::shared_ptr<Queue> & queue)
            {
                DJV_PRIVATE_PTR();
                for (const auto & i : p.plugins)
                {
                    if (i.second->canRead(fileName))
                    {
                        return i.second->read(fileName, queue);
                    }
                }
                std::stringstream s;
                s << DJV_TEXT("Cannot read") << " '" << fileName << "'.";
                throw std::runtime_error(s.str());
                return nullptr;
            }

            std::shared_ptr<IWrite> System::write(
                const std::string & fileName,
                const Info & info,
                const std::shared_ptr<Queue> & queue)
            {
                DJV_PRIVATE_PTR();
                for (const auto & i : p.plugins)
                {
                    if (i.second->canWrite(fileName, info))
                    {
                        return i.second->write(fileName, info, queue);
                    }
                }
                std::stringstream s;
                s << DJV_TEXT("Cannot write") << " '" << fileName << "'.";
                throw std::runtime_error(s.str());
                return nullptr;
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
