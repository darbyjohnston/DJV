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

            inline AudioInfo::AudioInfo(const Audio::Info & info, size_t sampleCount) :
                info(info),
                sampleCount(sampleCount)
            {}

            inline bool AudioInfo::operator == (const AudioInfo & other) const
            {
                return info == other.info && sampleCount == other.sampleCount;
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
                    out += i.second->getDataByteCount();
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
