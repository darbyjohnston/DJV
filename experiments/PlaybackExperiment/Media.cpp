// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "Media.h"

#include "IOCache.h"

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
    std::shared_ptr<IO::Info> ioInfo;
    std::shared_ptr<IO::IRead> read;
    bool hasVideo = false;
    bool hasAudio = false;

    std::shared_ptr<Core::Observer::ValueSubject<std::shared_ptr<Image::Data> > > imageSubject;
    std::shared_ptr<Core::Observer::ValueSubject<size_t> > videoQueueSizeSubject;
    std::shared_ptr<Core::Observer::ValueSubject<size_t> > audioQueueSizeSubject;
    std::shared_ptr<Core::Observer::ValueSubject<float> > speedSubject;
    std::shared_ptr<Core::Observer::ValueSubject<Playback> > playbackSubject;
    std::shared_ptr<Core::Observer::ValueSubject<IO::FrameInfo> > currentFrameSubject;
    std::shared_ptr<Core::Observer::ValueSubject<float> > audioRateSubject;
    std::shared_ptr<Core::Observer::ValueSubject<float> > audioVolumeSubject;
    std::shared_ptr<Core::Observer::ValueSubject<bool> > audioMuteSubject;

    std::shared_ptr<IO::Cache> cache;

    std::shared_ptr<System::Timer> timer;
    size_t fpsCount = 0;
    std::shared_ptr<System::Timer> fpsTimer;

    struct NoAudioPlayback
    {
        IO::FrameInfo frameInfo;
        IO::Timestamp startTimestamp = 0;
        std::chrono::steady_clock::time_point startTime;
    } noAudio;

    struct AudioPlayback
    {
        std::mutex mutex;
        IO::FrameInfo frameInfo;
        IO::Timestamp startTimestamp = 0;
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
    p.speedSubject = Core::Observer::ValueSubject<float>::create(0.F);
    p.playbackSubject = Core::Observer::ValueSubject<Playback>::create();
    p.currentFrameSubject = Core::Observer::ValueSubject<IO::FrameInfo>::create();
    p.audioRateSubject = Core::Observer::ValueSubject<float>::create(0.F);
    p.audioVolumeSubject = Core::Observer::ValueSubject<float>::create(1.F);
    p.audioMuteSubject = Core::Observer::ValueSubject<bool>::create(false);

    auto ioSystem = context->getSystemT<IO::IOSystem>();
    p.read = ioSystem->read(fileInfo);
    if (p.read)
    {
        p.ioInfo = p.read->getInfo().get();
        if (p.ioInfo)
        {
            if (p.ioInfo->video.size())
            {
                p.hasVideo = true;
            }
            if (p.ioInfo->audio.isValid())
            {
                try
                {
                    p.rtAudio.reset(new RtAudio);
                    RtAudio::StreamParameters rtParameters;
                    auto audioSystem = context->getSystemT<Audio::AudioSystem>();
                    rtParameters.deviceId = audioSystem->getDefaultOutputDevice();
                    rtParameters.nChannels = p.ioInfo->audio.channelCount;
                    unsigned int rtBufferFrames = bufferFramesCount;
                    p.rtAudio->openStream(
                        &rtParameters,
                        nullptr,
                        Audio::toRtAudio(p.ioInfo->audio.type),
                        p.ioInfo->audio.sampleRate,
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
    }

    p.cache = IO::Cache::create(context);

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

const std::shared_ptr<IO::Info>& Media::getIOInfo() const
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

std::shared_ptr<Core::Observer::IValueSubject<float> > Media::observeSpeed() const
{
    return _p->speedSubject;
}

std::shared_ptr<Core::Observer::IValueSubject<Playback> > Media::observePlayback() const
{
    return _p->playbackSubject;
}

std::shared_ptr<Core::Observer::IValueSubject<IO::FrameInfo> > Media::observeCurrentFrame() const
{
    return _p->currentFrameSubject;
}

std::shared_ptr<Core::Observer::IValueSubject<float> > Media::observeAudioRate() const
{
    return _p->audioRateSubject;
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
    if (p.read && p.playbackSubject->setIfChanged(value))
    {
        p.read->setPlaybackDirection(Playback::Forward == value ? PlaybackDirection::Forward : PlaybackDirection::Reverse);
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
                    p.audio.startTimestamp = p.audio.frameInfo.timestamp;
                }
                _startAudio();
            }
            else
            {
                p.noAudio.startTime = std::chrono::steady_clock::now();
                p.noAudio.startTimestamp = p.noAudio.frameInfo.timestamp;
            }
            auto weak = std::weak_ptr<Media>(shared_from_this());
            p.fpsTimer->start(
                std::chrono::seconds(1),
                [weak](const std::chrono::steady_clock::time_point&, const Core::Time::Duration& duration)
                {
                    if (auto media = weak.lock())
                    {
                        media->_p->speedSubject->setIfChanged(media->_p->fpsCount / std::chrono::duration<float>(duration).count());
                        media->_p->fpsCount = 0;
                    }
                });
            break;
        }
        default: break;
        }
    }
}

void Media::seek(IO::Timestamp value)
{
    DJV_PRIVATE_PTR();
    if (p.read)
    {
        p.read->seek(value);
    }
}

void Media::frame(Frame value)
{
    DJV_PRIVATE_PTR();
    Math::IntRational speed;
    int64_t frameIndex = -1;
    const int64_t frame = AV::Time::scale(1, speed.swap(), IO::timebase);
    if (!p.ioInfo->video.empty())
    {
        speed = p.ioInfo->videoSpeed;
        switch (value)
        {
        case Frame::Start:
            frameIndex = 0;
            break;
        case Frame::End:
            frameIndex = p.ioInfo->videoDuration - frame;
            break;
        case Frame::Next:
            ++frameIndex;
            if (frameIndex >= p.ioInfo->videoDuration - frame)
            {
                frameIndex = 0;
            }
            break;
        case Frame::Prev:
            --frameIndex;
            if (frameIndex < 0)
            {
                frameIndex = p.ioInfo->videoDuration - frame;
            }
            break;
        default: break;
        }
    }
    else if (p.ioInfo->audio.isValid())
    {
        speed = Math::IntRational(p.ioInfo->audio.sampleRate, 1);
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

    IO::FrameInfo frameInfo = p.currentFrameSubject->get();
    IO::VideoFrame videoFrame;
    bool valid = false;
    size_t videoQueueSize = 0;
    size_t audioQueueSize = 0;
    {
        std::lock_guard<std::mutex> lock(p.audio.mutex);
        IO::AudioFrame audioFrame;
        bool finished = false;
        const bool stopped = Playback::Stop == p.playbackSubject->get();
        {
            std::lock_guard<std::mutex> lock(p.read->getMutex());

            // Get an audio frame from the queue if playback is stopped and
            // this is a seek frame.
            auto& audioQueue = p.read->getAudioQueue();
            if (stopped && !audioQueue.isEmpty() && IO::SeekFrame::True == audioQueue.getFrame().seekFrame)
            {
                audioFrame = audioQueue.popFrame();
                p.audio.frameInfo = audioFrame.info;
                p.audio.startTimestamp = audioFrame.info.timestamp;
                if (!p.hasVideo)
                {
                    frameInfo = audioFrame.info;
                }
            }
            audioQueueSize = audioQueue.getCount();

            // Get video frames from the queue.
            auto& videoQueue = p.read->getVideoQueue();
            finished = videoQueue.isFinished() && videoQueue.isEmpty();
            while (!videoQueue.isEmpty() &&
                (videoQueue.getFrame().info.timestamp <= p.audio.frameInfo.timestamp || (stopped && IO::SeekFrame::True == videoQueue.getFrame().seekFrame)))
            {
                videoFrame = videoQueue.popFrame();
                frameInfo = videoFrame.info;
                valid = true;
                ++(p.fpsCount);
            }
            videoQueueSize = videoQueue.getCount();
        }
    }
    if (valid)
    {
        p.imageSubject->setIfChanged(videoFrame.data);
        //std::cout << "Media video: " << videoFrame.timestamp << std::endl;
    }
    p.currentFrameSubject->setIfChanged(frameInfo);
    p.videoQueueSizeSubject->setAlways(videoQueueSize);
    p.audioQueueSizeSubject->setAlways(audioQueueSize);
}

void Media::_tickNoAudio()
{
    DJV_PRIVATE_PTR();

    switch (p.playbackSubject->get())
    {
    case Playback::Stop:
        p.noAudio.frameInfo = p.currentFrameSubject->get();
        break;
    case Playback::Forward:
    case Playback::Reverse:
    {
        const auto now = std::chrono::steady_clock::now();
        const double playbackTime = std::chrono::duration<float>(now - p.noAudio.startTime).count();
        const auto rate = p.ioInfo->videoSpeed.swap();
        p.noAudio.frameInfo.timestamp = p.noAudio.startTimestamp + playbackTime * IO::timebase.getDen();
    }
    default: break;
    }

    IO::VideoFrame frame;
    bool valid = false;
    size_t queueSize = 0;
    bool finished = false;
    {
        std::lock_guard<std::mutex> lock(p.read->getMutex());

        p.read->getAudioQueue().clearFrames();

        auto& videoQueue = p.read->getVideoQueue();
        finished = videoQueue.isFinished() && videoQueue.isEmpty();
        while (!videoQueue.isEmpty() &&
            (videoQueue.getFrame().info.timestamp <= p.noAudio.frameInfo.timestamp || IO::SeekFrame::True == videoQueue.getFrame().seekFrame))
        {
            frame = videoQueue.popFrame();
            valid = true;
            ++(p.fpsCount);
            if (IO::SeekFrame::True == frame.seekFrame)
            {
                break;
            }
        }
        queueSize = videoQueue.getCount();
    }
    if (IO::SeekFrame::True == frame.seekFrame)
    {
        p.noAudio.startTime = std::chrono::steady_clock::now();
        p.noAudio.startTimestamp = frame.info.timestamp;
    }
    if (valid)
    {
        p.imageSubject->setIfChanged(frame.data);
        p.currentFrameSubject->setIfChanged(frame.info);
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

    // Get audio frames from the queue.
    size_t outputSampleCount = static_cast<size_t>(nFrames);
    size_t sampleCount = 0;
    if (p.audio.samplesOffset > 0)
    {
        sampleCount += p.audio.data->getSampleCount() - p.audio.samplesOffset;
    }
    std::vector<IO::AudioFrame> frames;
    size_t queueSize = 0;
    {
        std::lock_guard<std::mutex> lock(p.read->getMutex());
        auto& queue = p.read->getAudioQueue();
        while (!queue.isEmpty() && sampleCount < outputSampleCount)
        {
            auto frame = queue.popFrame();
            frames.push_back(frame);
            if (IO::SeekFrame::True == frame.seekFrame)
            {
                p.audio.startTimestamp = frame.info.timestamp;
                p.audio.samplesOffset = 0;
                p.audio.samplesCount = 0;
                sampleCount = 0;
            }
            sampleCount += frame.data->getSampleCount();
        }
        queueSize = queue.getCount();
    }

    // Use the remaining data from the frame.
    const auto& info = p.ioInfo->audio;
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
        p.audio.frameInfo = i.info;
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
