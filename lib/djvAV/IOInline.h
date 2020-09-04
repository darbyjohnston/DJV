// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            inline bool Info::operator == (const Info& other) const
            {
                return
                    fileName == other.fileName &&
                    videoSpeed == other.videoSpeed &&
                    videoSequence == other.videoSequence &&
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

            inline Core::Math::Range<Core::Frame::Index> InOutPoints::getRange(size_t size) const
            {
                Core::Frame::Index min = 0;
                Core::Frame::Index max = size ? (size - 1) : 0;
                if (_enabled)
                {
                    if (_in != Core::Frame::invalid)
                    {
                        min = _in;
                    }
                    if (_out != Core::Frame::invalid)
                    {
                        max = _out;
                    }
                }
                return Core::Math::Range<Core::Frame::Index>(min, max);
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
