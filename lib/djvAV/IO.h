// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Image.h>
#include <djvImage/Tags.h>

#include <djvAudio/AudioData.h>

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

#include <future>
#include <queue>
#include <mutex>
#include <set>

namespace djv
{
    namespace AV
    {
        //! This namespace provides I/O functionality.
        namespace IO
        {
            //! This class provides I/O information.
            class Info
            {
            public:
                Info();

                std::string              fileName;
                Math::Rational           videoSpeed;
                Math::Frame::Sequence    videoSequence;
                std::vector<Image::Info> video;
                Audio::Info              audio;
                Image::Tags              tags;

                bool operator == (const Info&) const;
            };

            //! This class provides a video frame.
            class VideoFrame
            {
            public:
                VideoFrame();
                VideoFrame(Math::Frame::Number, const std::shared_ptr<Image::Image>&);

                Math::Frame::Number           frame = 0;
                std::shared_ptr<Image::Image> image;

                bool operator == (const VideoFrame&) const;
            };

            //! This class provides a queue of video frames.
            class VideoQueue
            {
                DJV_NON_COPYABLE(VideoQueue);

            public:
                VideoQueue();

                size_t getMax() const;
                void setMax(size_t);

                bool isEmpty() const;
                size_t getCount() const;
                VideoFrame getFrame() const;
                void addFrame(const VideoFrame&);
                VideoFrame popFrame();
                void clearFrames();

                bool isFinished() const;
                void setFinished(bool);

            private:
                size_t _max = 0;
                std::queue<VideoFrame> _queue;
                bool _finished = false;
            };

            //! This class provides an audio frame.
            class AudioFrame
            {
            public:
                AudioFrame();
                explicit AudioFrame(const std::shared_ptr<Audio::Data>&);

                std::shared_ptr<Audio::Data> audio;
                
                bool operator == (const AudioFrame&) const;
            };

            //! This class provides a queue of audio frames.
            class AudioQueue
            {
                DJV_NON_COPYABLE(AudioQueue);

            public:
                AudioQueue();

                size_t getMax() const;
                void setMax(size_t);

                bool isEmpty() const;
                size_t getCount() const;
                AudioFrame getFrame() const;
                void addFrame(const AudioFrame&);
                AudioFrame popFrame();
                void clearFrames();

                bool isFinished() const;
                void setFinished(bool);

            private:
                std::mutex _mutex;
                size_t _max = 0;
                std::queue<AudioFrame> _queue;
                bool _finished = false;
            };

            //! This class provides playback in/out points.
            class InOutPoints
            {
            public:
                InOutPoints();
                InOutPoints(bool, Math::Frame::Index, Math::Frame::Index);

                bool isEnabled() const;
                Math::Frame::Index getIn() const;
                Math::Frame::Index getOut() const;

                Math::Range<Math::Frame::Index> getRange(size_t) const;

                bool operator == (const InOutPoints&) const;

            private:
                bool _enabled = false;
                Math::Frame::Index _in = Math::Frame::invalid;
                Math::Frame::Index _out = Math::Frame::invalid;
            };

            //! This enumeration provides the playback direction for caching.
            enum class Direction
            {
                Forward,
                Reverse
            };

            //! This class provides a frame cache.
            class Cache
            {
            public:
                Cache();
                
                size_t getMax() const;
                size_t getCount() const;
                size_t getTotalByteCount() const;
                Math::Frame::Sequence getFrames() const;
                size_t getReadBehind() const;
                const Math::Frame::Sequence& getSequence() const;
                void setMax(size_t);
                void setSequenceSize(size_t);
                void setInOutPoints(const InOutPoints&);
                void setDirection(Direction);
                void setCurrentFrame(Math::Frame::Index);

                bool contains(Math::Frame::Index) const;
                bool get(Math::Frame::Index, std::shared_ptr<Image::Image>&) const;
                void add(Math::Frame::Index, const std::shared_ptr<Image::Image>&);
                void clear();

            private:
                void _cacheUpdate();

                size_t _max = 0;
                size_t _sequenceSize = 0;
                InOutPoints _inOutPoints;
                Direction _direction = Direction::Forward;
                Math::Frame::Index _currentFrame = 0;
                //! \todo Should this be configurable?
                size_t _readBehind = 10;
                Math::Frame::Sequence _sequence;
                std::map<Math::Frame::Index, std::shared_ptr<Image::Image> > _cache;
            };

        } // namespace IO
    } // namespace AV
} // namespace djv

#include <djvAV/IOInline.h>
