// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AudioData.h>
#include <djvAV/Image.h>
#include <djvAV/Tags.h>

#include <djvCore/FrameNumber.h>
#include <djvCore/Rational.h>

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
                Core::Math::Rational     videoSpeed;
                Core::Frame::Sequence    videoSequence;
                std::vector<Image::Info> video;
                Audio::Info              audio;
                Tags                     tags;

                bool operator == (const Info&) const;
            };

            //! This class provides a video frame.
            class VideoFrame
            {
            public:
                VideoFrame();
                VideoFrame(Core::Frame::Number, const std::shared_ptr<Image::Image>&);

                Core::Frame::Number           frame = 0;
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
                InOutPoints(bool, Core::Frame::Index, Core::Frame::Index);

                bool isEnabled() const;
                Core::Frame::Index getIn() const;
                Core::Frame::Index getOut() const;

                Core::Math::Range<Core::Frame::Index> getRange(size_t) const;

                bool operator == (const InOutPoints&) const;

            private:
                bool _enabled = false;
                Core::Frame::Index _in = Core::Frame::invalid;
                Core::Frame::Index _out = Core::Frame::invalid;
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
                Core::Frame::Sequence getFrames() const;
                size_t getReadBehind() const;
                const Core::Frame::Sequence& getSequence() const;
                void setMax(size_t);
                void setSequenceSize(size_t);
                void setInOutPoints(const InOutPoints&);
                void setDirection(Direction);
                void setCurrentFrame(Core::Frame::Index);

                bool contains(Core::Frame::Index) const;
                bool get(Core::Frame::Index, std::shared_ptr<AV::Image::Image>&) const;
                void add(Core::Frame::Index, const std::shared_ptr<AV::Image::Image>&);
                void clear();

            private:
                void _cacheUpdate();

                size_t _max = 0;
                size_t _sequenceSize = 0;
                InOutPoints _inOutPoints;
                Direction _direction = Direction::Forward;
                Core::Frame::Index _currentFrame = 0;
                //! \todo Should this be configurable?
                size_t _readBehind = 10;
                Core::Frame::Sequence _sequence;
                std::map<Core::Frame::Index, std::shared_ptr<AV::Image::Image> > _cache;
            };

        } // namespace IO
    } // namespace AV
} // namespace djv

#include <djvAV/IOInline.h>
