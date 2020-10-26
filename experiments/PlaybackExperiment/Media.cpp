// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "Media.h"

#include <djvAV/Time.h>
#include <djvAV/TimeFunc.h>

#include <djvAudio/AudioSystem.h>
#include <djvAudio/DataFunc.h>

#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/TextSystem.h>
#include <djvSystem/Timer.h>
#include <djvSystem/TimerFunc.h>

#include <djvCore/StringFormat.h>
#include <djvCore/StringFunc.h>

using namespace djv;

namespace
{
    const size_t bufferFramesCount = 10;

} // namespace

struct Media::Private
{
    std::weak_ptr<System::Context> context;
    System::File::Info fileInfo;
    IOInfo ioInfo;
    std::shared_ptr<IIO> io;
    bool hasVideo = false;
    bool hasAudio = false;

    std::shared_ptr<Core::Observer::ValueSubject<std::shared_ptr<Image::Data> > > imageSubject;
    std::shared_ptr<Core::Observer::ValueSubject<size_t> > videoQueueSizeSubject;
    std::shared_ptr<Core::Observer::ValueSubject<size_t> > audioQueueSizeSubject;
    std::shared_ptr<Core::Observer::ValueSubject<Playback> > playbackSubject;
    std::shared_ptr<Core::Observer::ValueSubject<Timestamp> > timestampSubject;
    std::shared_ptr<Core::Observer::ValueSubject<float> > fpsSubject;
    std::shared_ptr<Core::Observer::ValueSubject<float> > audioVolumeSubject;
    std::shared_ptr<Core::Observer::ValueSubject<bool> > audioMuteSubject;

    std::shared_ptr<System::Timer> timer;
    size_t fpsCount = 0;
    std::shared_ptr<System::Timer> fpsTimer;

    struct NoAudioPlayback
    {
        Timestamp startTimestamp = 0;
        std::chrono::steady_clock::time_point startTime;
    } noAudio;

    struct AudioPlayback
    {
        std::mutex mutex;
        Timestamp timestamp = 0;
        Timestamp startTimestamp = 0;
        float volume = 1.F;
        bool mute = false;
        std::shared_ptr<djv::Audio::Data> data;
        size_t samplesOffset = 0;
        size_t samplesCount = 0;
    } audio;

    std::unique_ptr<RtAudio> rtAudio;
};

void Media::_init(
    const System::File::Info& fileInfo,
    const std::shared_ptr<System::Context>& context)
{
    DJV_PRIVATE_PTR();
    p.context = context;
    p.fileInfo = fileInfo;
    p.imageSubject = Core::Observer::ValueSubject<std::shared_ptr<Image::Data> >::create();
    p.videoQueueSizeSubject = Core::Observer::ValueSubject<size_t>::create();
    p.audioQueueSizeSubject = Core::Observer::ValueSubject<size_t>::create();
    p.playbackSubject = Core::Observer::ValueSubject<Playback>::create();
    p.timestampSubject = Core::Observer::ValueSubject<Timestamp>::create(0);
    p.fpsSubject = Core::Observer::ValueSubject<float>::create(0.F);
    p.audioVolumeSubject = Core::Observer::ValueSubject<float>::create(1.F);
    p.audioMuteSubject = Core::Observer::ValueSubject<bool>::create(false);

    auto ioSystem = context->getSystemT<IOSystem>();
    p.io = ioSystem->read(fileInfo);
    if (p.io)
    {
        p.ioInfo = p.io->getInfo().get();
        if (p.ioInfo.video.size())
        {
            p.hasVideo = true;
        }
        if (p.ioInfo.audio.isValid())
        {
            try
            {
                p.rtAudio.reset(new RtAudio);
                RtAudio::StreamParameters rtParameters;
                auto audioSystem = context->getSystemT<Audio::AudioSystem>();
                rtParameters.deviceId = audioSystem->getDefaultOutputDevice();
                rtParameters.nChannels = p.ioInfo.audio.channelCount;
                unsigned int rtBufferFrames = bufferFramesCount;
                p.rtAudio->openStream(
                    &rtParameters,
                    nullptr,
                    Audio::toRtAudio(p.ioInfo.audio.type),
                    p.ioInfo.audio.sampleRate,
                    &rtBufferFrames,
                    _rtAudioCallback,
                    this,
                    nullptr,
                    _rtAudioErrorCallback);
                p.hasAudio = true;
            }
            catch (const std::exception& e)
            {
                _error(DJV_TEXT("Audio cannot be initialized"), e.what());
            }
        }
    }

    p.timer = System::Timer::create(context);
    p.timer->setRepeating(true);
    auto weak = std::weak_ptr<Media>(shared_from_this());
    p.timer->start(
        System::getTimerDuration(System::TimerValue::Fast),
        [weak](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
        {
            if (auto media = weak.lock())
            {
                media->_tick();
            }
        });

    p.fpsTimer = System::Timer::create(context);
    p.fpsTimer->setRepeating(true);
}

Media::Media() :
    _p(new Private)
{}

Media::~Media()
{}

std::shared_ptr<Media> Media::create(
    const System::File::Info& fileInfo,
    const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<Media>(new Media);
    out->_init(fileInfo, context);
    return out;
}

const System::File::Info& Media::getFileInfo() const
{
    return _p->fileInfo;
}

std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<Image::Data> > > Media::observeImage() const
{
    return _p->imageSubject;
}

const IOInfo& Media::getIOInfo() const
{
    return _p->ioInfo;
}

std::shared_ptr<Core::Observer::IValueSubject<size_t> > Media::observeVideoQueueSize() const
{
    return _p->videoQueueSizeSubject;
}

std::shared_ptr<Core::Observer::IValueSubject<size_t> > Media::observeAudioQueueSize() const
{
    return _p->audioQueueSizeSubject;
}

std::shared_ptr<Core::Observer::IValueSubject<Playback> > Media::observePlayback() const
{
    return _p->playbackSubject;
}

std::shared_ptr<Core::Observer::IValueSubject<Timestamp> > Media::observeTimestamp() const
{
    return _p->timestampSubject;
}

std::shared_ptr<Core::Observer::IValueSubject<float> > Media::observeFPS() const
{
    return _p->fpsSubject;
}

std::shared_ptr<Core::Observer::IValueSubject<float> > Media::observeAudioVolume() const
{
return _p->audioVolumeSubject;
}

std::shared_ptr<Core::Observer::IValueSubject<bool> > Media::observeAudioMute() const
{
    return _p->audioMuteSubject;
}

void Media::setPlayback(Playback value)
{
    DJV_PRIVATE_PTR();
    if (p.io && p.playbackSubject->setIfChanged(value))
    {
        p.io->setPlaybackDirection(Playback::Forward == value ? PlaybackDirection::Forward : PlaybackDirection::Reverse);
        switch (value)
        {
        case Playback::Stop:
        {
            _stopAudio();
            p.fpsTimer->stop();
            break;
        }
        case Playback::Forward:
        case Playback::Reverse:
        {
            if (_hasAudioPlayback())
            {
                {
                    std::lock_guard<std::mutex> lock(p.audio.mutex);
                    p.audio.startTimestamp = p.timestampSubject->get();
                }
                _startAudio();
            }
            else
            {
                p.noAudio.startTime = std::chrono::steady_clock::now();
                p.noAudio.startTimestamp = p.timestampSubject->get();
            }
            auto weak = std::weak_ptr<Media>(shared_from_this());
            p.fpsTimer->start(
                std::chrono::seconds(1),
                [weak](const std::chrono::steady_clock::time_point&, const Core::Time::Duration& duration)
                {
                    if (auto media = weak.lock())
                    {
                        media->_p->fpsSubject->setIfChanged(media->_p->fpsCount / std::chrono::duration<float>(duration).count());
                        media->_p->fpsCount = 0;
                    }
                });
            break;
        }
        default: break;
        }
    }
}

void Media::seek(Math::Frame::Index value)
{
    DJV_PRIVATE_PTR();
    if (p.io)
    {
        p.io->seek(value);
    }
}

void Media::frame(Frame value)
{
    DJV_PRIVATE_PTR();
    Math::Rational speed;
    Math::Frame::Sequence sequence;
    if (!p.ioInfo.video.empty())
    {
        speed = p.ioInfo.videoSpeed;
        sequence = p.ioInfo.videoSequence;
    }
    else if (p.ioInfo.audio.isValid())
    {
        speed = Math::Rational(p.ioInfo.audio.sampleRate, 1);
        sequence = Math::Frame::Sequence(0, p.ioInfo.audioSampleCount > 0 ? (p.ioInfo.audioSampleCount - 1) : 0);
    }
    switch (value)
    {
    case Frame::Start:
        p.io->seek(0);
        break;
    case Frame::End:
        if (speed.isValid())
        {
            int64_t t = 0;
            t = AV::Time::scale(sequence.getLastIndex(), speed.swap(), timebase);
            p.io->seek(t);
        }
        break;
    case Frame::Next:
        if (speed.isValid())
        {
            int64_t t = AV::Time::scale(p.timestampSubject->get(), timebase, speed.swap());
            ++t;
            if (t > sequence.getLastIndex())
            {
                t = 0;
            }
            p.io->seek(AV::Time::scale(t, speed.swap(), timebase));
        }
        break;
    case Frame::Prev:
        if (speed.isValid())
        {
            int64_t t = AV::Time::scale(p.timestampSubject->get(), timebase, speed.swap());
            --t;
            if (t < 0)
            {
                t = sequence.getLastIndex();
            }
            p.io->seek(AV::Time::scale(t, speed.swap(), timebase));
        }
        break;
    default: break;
    }
}

void Media::_tick()
{
    if (_hasAudioPlayback())
    {
        _tickAudio();
    }
    else
    {
        _tickNoAudio();
    }
}

void Media::_tickAudio()
{
    DJV_PRIVATE_PTR();

    Timestamp timestamp = 0;
    VideoFrame videoFrame;
    bool valid = false;
    size_t videoQueueSize = 0;
    size_t audioQueueSize = 0;
    {
        std::lock_guard<std::mutex> lock(p.audio.mutex);
        timestamp = p.audio.timestamp;
        AudioFrame audioFrame;
        bool finished = false;
        const bool stopped = Playback::Stop == p.playbackSubject->get();
        {
            std::lock_guard<std::mutex> lock(p.io->getMutex());

            auto& audioQueue = p.io->getAudioQueue();
            if (stopped && !audioQueue.isEmpty() && SeekFrame::True == audioQueue.getFrame().seekFrame)
            {
                audioFrame = audioQueue.popFrame();
                timestamp = audioFrame.timestamp;
            }
            audioQueueSize = audioQueue.getCount();

            auto& videoQueue = p.io->getVideoQueue();
            finished = videoQueue.isFinished() && videoQueue.isEmpty();
            while (!videoQueue.isEmpty() &&
                (videoQueue.getFrame().timestamp <= timestamp || (stopped && SeekFrame::True == videoQueue.getFrame().seekFrame)))
            {
                videoFrame = videoQueue.popFrame();
                valid = true;
                ++(p.fpsCount);
            }
            videoQueueSize = videoQueue.getCount();
        }
        if (stopped && SeekFrame::True == audioFrame.seekFrame)
        {
            p.audio.timestamp = timestamp;
            p.audio.startTimestamp = timestamp;
        }
    }
    if (valid)
    {
        p.imageSubject->setIfChanged(videoFrame.data);
        //std::cout << "Media video: " << videoFrame.timestamp << std::endl;
    }
    p.timestampSubject->setIfChanged(timestamp);
    p.videoQueueSizeSubject->setAlways(videoQueueSize);
    p.audioQueueSizeSubject->setAlways(audioQueueSize);
}

void Media::_tickNoAudio()
{
    DJV_PRIVATE_PTR();

    Timestamp currentTimestamp = 0;
    switch (p.playbackSubject->get())
    {
    case Playback::Stop:
        currentTimestamp = p.timestampSubject->get();
        break;
    case Playback::Forward:
    case Playback::Reverse:
    {
        const auto now = std::chrono::steady_clock::now();
        const double playbackTime = std::chrono::duration<float>(now - p.noAudio.startTime).count();
        const auto rate = timebase.swap();
        currentTimestamp = p.noAudio.startTimestamp + playbackTime * rate.getNum() / rate.getDen();
    }
    default: break;
    }

    VideoFrame frame;
    bool valid = false;
    size_t queueSize = 0;
    bool finished = false;
    {
        std::lock_guard<std::mutex> lock(p.io->getMutex());

        p.io->getAudioQueue().clearFrames();

        auto& videoQueue = p.io->getVideoQueue();
        finished = videoQueue.isFinished() && videoQueue.isEmpty();
        while (!videoQueue.isEmpty() &&
            (videoQueue.getFrame().timestamp <= currentTimestamp || SeekFrame::True == videoQueue.getFrame().seekFrame))
        {
            frame = videoQueue.popFrame();
            valid = true;
            ++(p.fpsCount);
            if (SeekFrame::True == frame.seekFrame)
            {
                break;
            }
        }
        queueSize = videoQueue.getCount();
    }
    if (SeekFrame::True == frame.seekFrame)
    {
        p.noAudio.startTime = std::chrono::steady_clock::now();
        p.noAudio.startTimestamp = frame.timestamp;
    }
    if (valid)
    {
        p.imageSubject->setIfChanged(frame.data);
        p.timestampSubject->setIfChanged(frame.timestamp);
    }
    p.videoQueueSizeSubject->setAlways(queueSize);
}

bool Media::_hasAudioPlayback() const
{
    DJV_PRIVATE_PTR();
    return
        p.hasAudio &&
        p.playbackSubject->get() != Playback::Reverse;
}

void Media::_startAudio()
{
    DJV_PRIVATE_PTR();
    if (auto context = p.context.lock())
    {
        try
        {
            p.rtAudio->startStream();
        }
        catch (const std::exception& e)
        {
            _error(DJV_TEXT("Cannot start audio"), e.what());
        }
    }
}

void Media::_stopAudio()
{
    DJV_PRIVATE_PTR();
    if (auto context = p.context.lock())
    {
        if (p.hasAudio && p.rtAudio->isStreamRunning())
        {
            try
            {
                p.rtAudio->abortStream();
                p.rtAudio->setStreamTime(0.0);
            }
            catch (const std::exception& e)
            {
                _error(DJV_TEXT("Cannot stop audio"), e.what());
            }
        }
    }
}

int Media::_audioCallback(
    void* outputBuffer,
    void* inputBuffer,
    unsigned int nFrames,
    double streamTime,
    RtAudioStreamStatus status)
{
    DJV_PRIVATE_PTR();
    std::lock_guard<std::mutex> lock(p.audio.mutex);

    // Get audio frames from the read queue.
    size_t outputSampleCount = static_cast<size_t>(nFrames);
    size_t sampleCount = 0;
    if (p.audio.samplesOffset > 0)
    {
        sampleCount += p.audio.data->getSampleCount() - p.audio.samplesOffset;
    }
    std::vector<AudioFrame> frames;
    size_t queueSize = 0;
    {
        std::lock_guard<std::mutex> lock(p.io->getMutex());
        auto& queue = p.io->getAudioQueue();
        while (!queue.isEmpty() && sampleCount < outputSampleCount)
        {
            auto frame = queue.getFrame();
            frames.push_back(frame);
            queue.popFrame();
            if (SeekFrame::True == frame.seekFrame)
            {
                p.audio.startTimestamp = frame.timestamp;
                p.audio.samplesOffset = 0;
                p.audio.samplesCount = 0;
                sampleCount = 0;
            }
            sampleCount += frame.data->getSampleCount();
        }
        queueSize = queue.getCount();
    }

    // Use the remaining data from the frame.
    const auto& info = p.ioInfo.audio;
    const size_t sampleByteCount = info.channelCount * Audio::getByteCount(info.type);
    const float volume = !p.audio.mute ? p.audio.volume : 0.F;
    uint8_t* data = reinterpret_cast<uint8_t*>(outputBuffer);
    if (p.audio.data && p.audio.samplesOffset > 0)
    {
        const size_t size = std::min(
            p.audio.data->getSampleCount() - p.audio.samplesOffset,
            outputSampleCount);
        //memcpy(
        //    p,
        //    p.audioData->getData() + dataSamplesOffset * sampleByteCount,
        //    size * sampleByteCount);
        Audio::volume(
            p.audio.data->getData() + p.audio.samplesOffset * sampleByteCount,
            data,
            volume,
            size,
            info.channelCount,
            info.type);
        data += size * sampleByteCount;
        p.audio.samplesOffset += size;
        p.audio.samplesCount += size;
        outputSampleCount -= size;
        if (p.audio.samplesOffset >= p.audio.data->getSampleCount())
        {
            p.audio.samplesOffset = 0;
        }
    }

    // Process the frames.
    for (const auto& i : frames)
    {
        p.audio.timestamp = i.timestamp;
        p.audio.data = i.data;
        size_t size = std::min(i.data->getSampleCount(), outputSampleCount);
        //memcpy(
        //    p,
        //    i.audio->getData(),
        //    size * sampleByteCount);
        Audio::volume(
            i.data->getData(),
            data,
            volume,
            size,
            info.channelCount,
            info.type);
        data += size * sampleByteCount;
        p.audio.samplesOffset = size;
        p.audio.samplesCount += size;
        outputSampleCount -= size;
    }

    const size_t zero = (nFrames * sampleByteCount) - (data - reinterpret_cast<uint8_t*>(outputBuffer));
    if (zero)
    {
        //! \todo Is this the correct way to clear the audio data?
        memset(data, 0, zero);
    }

    return 0;
}

int Media::_rtAudioCallback(
    void* outputBuffer,
    void* inputBuffer,
    unsigned int nFrames,
    double streamTime,
    RtAudioStreamStatus status,
    void* userData)
{
    Media* media = reinterpret_cast<Media*>(userData);
    return media->_audioCallback(outputBuffer, inputBuffer, nFrames, streamTime, status);
}

void Media::_rtAudioErrorCallback(
    RtAudioError::Type type,
    const std::string& errorText)
{
    //std::cout << errorText << std::endl;
}

void Media::_error(const std::string& message, const std::string& what)
{
    DJV_PRIVATE_PTR();
    if (auto context = p.context.lock())
    {
        std::vector<std::string> messages;
        auto textSystem = context->getSystemT<System::TextSystem>();
        const std::string message = Core::String::Format("{0}: {1}: {2}").
            arg(p.fileInfo.getFileName()).
            arg(textSystem->getText(message)).
            arg(what);
        auto logSystem = context->getSystemT<System::LogSystem>();
        logSystem->log("djv::ViewApp::Media", message, System::LogLevel::Error);
    }
}
