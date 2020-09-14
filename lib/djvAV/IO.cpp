// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/IO.h>

#include <djvCore/SpeedFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            Info::Info() :
                videoSpeed(Time::fromSpeed(Time::getDefaultSpeed()))
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
                            out.add(Frame::Range(rangeStart, prevFrame));
                            rangeStart = frames[i];
                        }
                    }
                    if (size > 1)
                    {
                        out.add(Frame::Range(rangeStart, prevFrame));
                    }
                    else
                    {
                        out.add(Frame::Range(rangeStart));
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
                        if (frame < range.getMin())
                        {
                            frame = range.getMax();
                        }
                    }
                    _sequence.add(Frame::Range(frame));
                    const Frame::Index first = frame;
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
                                _sequence.add(Frame::Range(frame));
                            }
                        }
                        else
                        {
                            _sequence.add(Frame::Range(frame));
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
                    _sequence.add(Frame::Range(frame));
                    const Frame::Index first = frame;
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
                                _sequence.add(Frame::Range(frame));
                            }
                        }
                        else
                        {
                            _sequence.add(Frame::Range(frame));
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
