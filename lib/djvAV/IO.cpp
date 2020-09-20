// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/IO.h>

#include <djvAV/SpeedFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            Info::Info() :
                videoSpeed(fromSpeed(getDefaultSpeed()))
            {}

            VideoFrame::VideoFrame()
            {}

            VideoFrame::VideoFrame(Math::Frame::Number frame, const std::shared_ptr<Image::Image> & image) :
                frame(frame),
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

            AudioFrame::AudioFrame()
            {}

            AudioFrame::AudioFrame(const std::shared_ptr<Audio::Data>& audio) :
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

            InOutPoints::InOutPoints()
            {}

            InOutPoints::InOutPoints(bool enabled, Math::Frame::Index in, Math::Frame::Index out) :
                _enabled(enabled),
                _in(std::min(in, out)),
                _out(std::max(in, out))
            {}

            Cache::Cache()
            {}

            Math::Frame::Sequence Cache::getFrames() const
            {
                Math::Frame::Sequence out;
                std::vector<Math::Frame::Index> frames;
                for (const auto& i : _cache)
                {
                    frames.push_back(i.first);
                }
                const size_t size = frames.size();
                if (size)
                {
                    std::sort(frames.begin(), frames.end());
                    Math::Frame::Number rangeStart = frames[0];
                    Math::Frame::Number prevFrame = frames[0];
                    size_t i = 1;
                    for (; i < size; prevFrame = frames[i], ++i)
                    {
                        if (frames[i] != prevFrame + 1)
                        {
                            out.add(Math::Frame::Range(rangeStart, prevFrame));
                            rangeStart = frames[i];
                        }
                    }
                    if (size > 1)
                    {
                        out.add(Math::Frame::Range(rangeStart, prevFrame));
                    }
                    else
                    {
                        out.add(Math::Frame::Range(rangeStart));
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

            void Cache::setCurrentFrame(Math::Frame::Index value)
            {
                if (value == _currentFrame)
                    return;
                _currentFrame = value;
                _cacheUpdate();
            }

            void Cache::add(Math::Frame::Index index, const std::shared_ptr<Image::Image>& image)
            {
                _cache[index] = image;
                _cacheUpdate();
            }

            void Cache::_cacheUpdate()
            {
                const auto range = _inOutPoints.getRange(_sequenceSize);
                Math::Frame::Index frame = _currentFrame;
                _sequence = Math::Frame::Sequence();
                switch (_direction)
                {
                case Direction::Forward:
                {
                    for (size_t i = 0; i < _readBehind; ++i)
                    {
                        --frame;
                        if (frame < range.getMin())
                        {
                            frame = range.getMax();
                        }
                    }
                    _sequence.add(Math::Frame::Range(frame));
                    const Math::Frame::Index first = frame;
                    for (size_t i = 0; i < _max; ++i)
                    {
                        ++frame;
                        if (first == frame)
                        {
                            break;
                        }
                        if (frame > range.getMax())
                        {
                            frame = range.getMin();
                            if (frame != _sequence.getRanges().back().getMax())
                            {
                                _sequence.add(Math::Frame::Range(frame));
                            }
                        }
                        else
                        {
                            _sequence.add(Math::Frame::Range(frame));
                        }
                    }
                    break;
                }
                case Direction::Reverse:
                {
                    for (size_t i = 0; i < _readBehind; ++i)
                    {
                        ++frame;
                        if (frame > range.getMax())
                        {
                            frame = range.getMin();
                        }
                    }
                    _sequence.add(Math::Frame::Range(frame));
                    const Math::Frame::Index first = frame;
                    for (size_t i = 0; i < _max; ++i)
                    {
                        --frame;
                        if (first == frame)
                        {
                            break;
                        }
                        if (frame < range.getMin())
                        {
                            frame = range.getMax();
                            if (frame != _sequence.getRanges().back().getMin())
                            {
                                _sequence.add(Math::Frame::Range(frame));
                            }
                        }
                        else
                        {
                            _sequence.add(Math::Frame::Range(frame));
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

        } // namespace IO
    } // namespace AV
} // namespace djv
