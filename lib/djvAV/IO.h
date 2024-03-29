// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Data.h>

#include <djvAudio/Data.h>

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

#include <future>
#include <queue>
#include <set>

namespace djv
{
    namespace AV
    {
        //! Input/output.
        namespace IO
        {
            //! I/O information.
            class Info
            {
            public:
                Info();

                std::string              fileName;
                Math::IntRational        videoSpeed;
                Math::Frame::Sequence    videoSequence;
                std::vector<Image::Info> video;
                Audio::Info              audio;
                size_t                   audioSampleCount;
                Image::Tags              tags;

                bool operator == (const Info&) const;
            };

            //! Video frame.
            class VideoFrame
            {
            public:
                VideoFrame();
                VideoFrame(Math::Frame::Number, const std::shared_ptr<Image::Data>&);

                Math::Frame::Number          frame = 0;
                std::shared_ptr<Image::Data> data;

                bool operator == (const VideoFrame&) const;
            };

            //! Video frame queue.
            class VideoQueue
            {
                DJV_NON_COPYABLE(VideoQueue);

            public:
                VideoQueue();

                //! \name Size
                ///@{
                
                size_t getMax() const;

                void setMax(size_t);

                ///@}

                //! \name Frames
                ///@{
                
                bool isEmpty() const;
                size_t getCount() const;
                VideoFrame getFrame() const;

                void addFrame(const VideoFrame&);
                VideoFrame popFrame();
                void clearFrames();

                ///@}

                //! \name Finished
                ///@{

                bool isFinished() const;

                void setFinished(bool);

                ///@}

            private:
                size_t _max = 0;
                std::queue<VideoFrame> _queue;
                bool _finished = false;
            };

            //! Audio frame.
            class AudioFrame
            {
            public:
                AudioFrame();
                explicit AudioFrame(const std::shared_ptr<Audio::Data>&);

                std::shared_ptr<Audio::Data> data;
                
                bool operator == (const AudioFrame&) const;
            };

            //! Audio frame queue.
            class AudioQueue
            {
                DJV_NON_COPYABLE(AudioQueue);

            public:
                AudioQueue();

                //! \name Size
                ///@{

                size_t getMax() const;

                void setMax(size_t);

                ///@}

                //! \name Frames
                ///@{

                bool isEmpty() const;
                size_t getCount() const;
                AudioFrame getFrame() const;

                void addFrame(const AudioFrame&);
                AudioFrame popFrame();
                void clearFrames();

                ///@}

                //! \name Finished
                ///@{

                bool isFinished() const;
                
                void setFinished(bool);

                ///@}

            private:
                size_t _max = 0;
                std::queue<AudioFrame> _queue;
                bool _finished = false;
            };

            //! Playback in/out points.
            class InOutPoints
            {
            public:
                InOutPoints();
                InOutPoints(bool, Math::Frame::Index, Math::Frame::Index);

                //! \name Information
                ///@{

                bool isEnabled() const;

                ///@}

                //! \name Range
                ///@{

                Math::Frame::Index getIn() const;
                Math::Frame::Index getOut() const;
                Math::Range<Math::Frame::Index> getRange(size_t) const;

                ///@}

                bool operator == (const InOutPoints&) const;

            private:
                bool _enabled = false;
                Math::Frame::Index _in = Math::Frame::invalid;
                Math::Frame::Index _out = Math::Frame::invalid;
            };

            //! Playback direction for caching.
            enum class Direction
            {
                Forward,
                Reverse
            };

            //! Frame cache.
            class Cache
            {
            public:
                Cache();
                
                //! \name Size
                ///@{

                size_t getMax() const;
                size_t getCount() const;
                size_t getTotalByteCount() const;

                void setMax(size_t);

                ///@}

                //! \name Frames
                ///@{

                Math::Frame::Sequence getFrames() const;
                size_t getReadBehind() const;
                const Math::Frame::Sequence& getSequence() const;

                void setSequenceSize(size_t);
                void setInOutPoints(const InOutPoints&);
                void setDirection(Direction);
                void setCurrentFrame(Math::Frame::Index);

                bool contains(Math::Frame::Index) const;
                bool get(Math::Frame::Index, std::shared_ptr<Image::Data>&) const;

                void add(Math::Frame::Index, const std::shared_ptr<Image::Data>&);
                void clear();

                ///@}

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
                std::map<Math::Frame::Index, std::shared_ptr<Image::Data> > _cache;
            };

        } // namespace IO
    } // namespace AV
} // namespace djv

#include <djvAV/IOInline.h>
