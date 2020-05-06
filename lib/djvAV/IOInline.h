// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            inline VideoInfo::VideoInfo()
            {}

            inline VideoInfo::VideoInfo(const Image::Info & info, const Core::Time::Speed & speed, const Core::Frame::Sequence& sequence) :
                info(info),
                speed(speed),
                sequence(sequence)
            {}

            inline bool VideoInfo::operator == (const VideoInfo & other) const
            {
                return info == other.info && speed == other.speed && sequence == other.sequence;
            }

            inline AudioInfo::AudioInfo()
            {}

            inline AudioInfo::AudioInfo(const Audio::Info & info) :
                info(info)
            {}

            inline bool AudioInfo::operator == (const AudioInfo & other) const
            {
                return info == other.info;
            }

            inline Info::Info()
            {}

            inline Info::Info(const std::string & fileName, const VideoInfo & video) :
                fileName(fileName)
            {
                this->video.push_back(video);
            }

            inline Info::Info(const std::string & fileName, const AudioInfo & audio) :
                fileName(fileName)
            {
                this->audio.push_back(audio);
            }

            inline Info::Info(const std::string & fileName, const VideoInfo & video, const AudioInfo & audio) :
                fileName(fileName)
            {
                this->video.push_back(video);
                this->audio.push_back(audio);
            }

            inline Info::Info(const std::string & fileName, const std::vector<VideoInfo> & video, const std::vector<AudioInfo> & audio) :
                fileName(fileName),
                video(video),
                audio(audio)
            {}

            inline bool Info::operator == (const Info & other) const
            {
                return
                    fileName == other.fileName &&
                    video == other.video &&
                    audio == other.audio &&
                    tags == other.tags;
            }

            inline VideoFrame::VideoFrame()
            {}

            inline VideoFrame::VideoFrame(Core::Frame::Number frame, const std::shared_ptr<Image::Image>& image) :
                frame(frame),
                image(image)
            {}
            
            inline bool VideoFrame::operator == (const VideoFrame& other) const
            {
                return frame == other.frame && image == other.image;
            }

            inline VideoQueue::VideoQueue()
            {}

            inline size_t VideoQueue::getMax() const
            {
                return _max;
            }

            inline bool VideoQueue::isEmpty() const
            {
                return 0 == _queue.size();
            }

            inline size_t VideoQueue::getCount() const
            {
                return _queue.size();
            }

            inline VideoFrame VideoQueue::getFrame() const
            {
                return _queue.size() ? _queue.front() : VideoFrame();
            }

            inline bool VideoQueue::isFinished() const
            {
                return _finished;
            }

            inline AudioFrame::AudioFrame()
            {}

            inline AudioFrame::AudioFrame(const std::shared_ptr<Audio::Data>& audio) :
                audio(audio)
            {}
            
            inline bool AudioFrame::operator == (const AudioFrame& other) const
            {
                return audio == other.audio;
            }

            inline AudioQueue::AudioQueue()
            {}

            inline size_t AudioQueue::getMax() const
            {
                return _max;
            }

            inline bool AudioQueue::isEmpty() const
            {
                return 0 == _queue.size();
            }

            inline size_t AudioQueue::getCount() const
            {
                return _queue.size();
            }

            inline bool AudioQueue::isFinished() const
            {
                return _finished;
            }

            inline AudioFrame AudioQueue::getFrame() const
            {
                return _queue.size() ? _queue.front() : AudioFrame();
            }

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

            inline InOutPoints::InOutPoints()
            {}

            inline InOutPoints::InOutPoints(bool enabled, Core::Frame::Index in, Core::Frame::Index out) :
                _enabled(enabled),
                _in(std::min(in, out)),
                _out(std::max(in, out))
            {}

            inline bool InOutPoints::isEnabled() const
            {
                return _enabled;
            }

            inline Core::Frame::Index InOutPoints::getIn() const
            {
                return _in;
            }

            inline Core::Frame::Index InOutPoints::getOut() const
            {
                return _out;
            }

            inline Core::Range::Range<Core::Frame::Index> InOutPoints::getRange(size_t size) const
            {
                Core::Range::Range<Core::Frame::Index> out;
                out.min = 0;
                out.max = size ? (size - 1) : 0;
                if (_enabled)
                {
                    if (_in != Core::Frame::invalid)
                    {
                        out.min = _in;
                    }
                    if (_out != Core::Frame::invalid)
                    {
                        out.max = _out;
                    }
                }
                return out;
            }

            inline bool InOutPoints::operator == (const InOutPoints& other) const
            {
                return _enabled == other._enabled &&
                    _in == other._in &&
                    _out == other._out;
            }

            inline Cache::Cache()
            {}
            
            inline size_t Cache::getMax() const
            {
                return _max;
            }
            
            inline size_t Cache::getCount() const
            {
                return _cache.size();
            }

            inline size_t Cache::getTotalByteCount() const
            {
                size_t out = 0;
                for (const auto& i : _cache)
                {
                    if (i.second)
                    {
                        out += i.second->getDataByteCount();
                    }
                }
                return out;
            }

            inline const std::string & IPlugin::getPluginName() const
            {
                return _pluginName;
            }

            inline const std::string & IPlugin::getPluginInfo() const
            {
                return _pluginInfo;
            }

            inline const std::set<std::string> & IPlugin::getFileExtensions() const
            {
                return _fileExtensions;
            }

            inline size_t Cache::getReadBehind() const
            {
                return _readBehind;
            }

            inline const Core::Frame::Sequence& Cache::getSequence() const
            {
                return _sequence;
            }

            inline bool Cache::contains(Core::Frame::Index value) const
            {
                return _cache.find(value) != _cache.end();
            }

            inline bool Cache::get(Core::Frame::Index index, std::shared_ptr<AV::Image::Image>& out) const
            {
                const auto i = _cache.find(index);
                const bool found = i != _cache.end();
                if (found)
                {
                    out = i->second;
                }
                return found;
            }

            inline void Cache::clear()
            {
                _cache.clear();
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
