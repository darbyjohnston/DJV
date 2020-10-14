// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "Media.h"

#include <djvAV/Time.h>
#include <djvAV/TimeFunc.h>

#include <djvAudio/AudioDataFunc.h>
#include <djvAudio/AudioSystem.h>

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
    const size_t audioBufferFrameCount = 256;

} // namespace

struct Media::Private
{
    std::weak_ptr<System::Context> context;

    System::File::Info fileInfo;
    std::shared_ptr<IIO> read;
    std::shared_ptr<Core::Observer::ValueSubject<IOInfo> > info;
    std::shared_ptr<Core::Observer::ValueSubject<std::shared_ptr<Image::Image> > > image;
    std::shared_ptr<Core::Observer::ValueSubject<size_t> > videoQueueSize;
    std::shared_ptr<Core::Observer::ValueSubject<size_t> > audioQueueSize;
    std::shared_ptr<Core::Observer::ValueSubject<Playback> > playback;
    std::shared_ptr<Core::Observer::ValueSubject<Timestamp> > timestamp;
    std::shared_ptr<Core::Observer::ValueSubject<float> > fps;
    std::shared_ptr<Core::Observer::ValueSubject<float> > audioVolume;
    std::shared_ptr<Core::Observer::ValueSubject<bool> > audioMute;

    size_t fpsCount = 0;
    float playbackTime = 0.F;
    Timestamp playbackStart = 0;
    std::chrono::steady_clock::time_point playbackStartTime;
    std::shared_ptr<System::Timer> timer;
    std::shared_ptr<System::Timer> fpsTimer;

    std::unique_ptr<RtAudio> rtAudio;
    std::shared_ptr<Audio::Data> audioData;
    size_t audioDataSamplesOffset = 0;
    size_t audioDataSamplesCount = 0;
};

void Media::_init(
    const System::File::Info& fileInfo,
    const std::shared_ptr<System::Context>& context)
{
    DJV_PRIVATE_PTR();
    p.context = context;
    p.fileInfo = fileInfo;
    p.info = Core::Observer::ValueSubject<IOInfo>::create();
    p.image = Core::Observer::ValueSubject<std::shared_ptr<Image::Image> >::create();
    p.videoQueueSize = Core::Observer::ValueSubject<size_t>::create();
    p.audioQueueSize = Core::Observer::ValueSubject<size_t>::create();
    p.playback = Core::Observer::ValueSubject<Playback>::create();
    p.timestamp = Core::Observer::ValueSubject<Timestamp>::create(0);
    p.fps = Core::Observer::ValueSubject<float>::create(0.F);
    p.audioVolume = Core::Observer::ValueSubject<float>::create(1.F);
    p.audioMute = Core::Observer::ValueSubject<bool>::create(false);

    auto ioSystem = context->getSystemT<IOSystem>();
    p.read = ioSystem->read(fileInfo);
    if (p.read)
    {
        const auto& info = p.read->getInfo().get();
        p.info->setIfChanged(info);

        if (info.audio.isValid())
        {
            try
            {
                p.rtAudio.reset(new RtAudio);

                RtAudio::StreamParameters rtParameters;
                auto audioSystem = context->getSystemT<Audio::AudioSystem>();
                rtParameters.deviceId = audioSystem->getDefaultOutputDevice();
                rtParameters.nChannels = info.audio.channelCount;
                unsigned int rtBufferFrames = audioBufferFrameCount;

                p.rtAudio->openStream(
                    &rtParameters,
                    nullptr,
                    Audio::toRtAudio(info.audio.type),
                    info.audio.sampleRate,
                    &rtBufferFrames,
                    _rtAudioCallback,
                    this,
                    nullptr,
                    _rtAudioErrorCallback);
            }
            catch (const std::exception& e)
            {
                _log(DJV_TEXT("Audio cannot be initialized"), e.what());
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

std::shared_ptr<Core::Observer::IValueSubject<IOInfo> > Media::observeInfo() const
{
    return _p->info;
}

std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<Image::Image> > > Media::observeCurrentImage() const
{
    return _p->image;
}

std::shared_ptr<Core::Observer::IValueSubject<size_t> > Media::observeVideoQueueSize() const
{
    return _p->videoQueueSize;
}

std::shared_ptr<Core::Observer::IValueSubject<size_t> > Media::observeAudioQueueSize() const
{
    return _p->audioQueueSize;
}

std::shared_ptr<Core::Observer::IValueSubject<Playback> > Media::observePlayback() const
{
    return _p->playback;
}

std::shared_ptr<Core::Observer::IValueSubject<Timestamp> > Media::observeTimestamp() const
{
    return _p->timestamp;
}

std::shared_ptr<Core::Observer::IValueSubject<float> > Media::observeFPS() const
{
    return _p->fps;
}

void Media::setPlayback(Playback value)
{
    DJV_PRIVATE_PTR();
    if (p.read && p.playback->setIfChanged(value))
    {
        p.read->setPlaybackDirection(Playback::Forward == value ? PlaybackDirection::Forward : PlaybackDirection::Reverse);
        switch (value)
        {
        case Playback::Stop:
            p.playbackTime = 0.F;
            p.fpsTimer->stop();
            _stopAudio();
            break;
        case Playback::Forward:
        case Playback::Reverse:
        {
            p.playbackStart = p.timestamp->get();
            p.playbackStartTime = std::chrono::steady_clock::now();
            auto weak = std::weak_ptr<Media>(shared_from_this());
            p.fpsTimer->start(
                std::chrono::seconds(1),
                [weak](const std::chrono::steady_clock::time_point&, const Core::Time::Duration& duration)
                {
                    if (auto media = weak.lock())
                    {
                        media->_p->fps->setIfChanged(media->_p->fpsCount / std::chrono::duration<float>(duration).count());
                        media->_p->fpsCount = 0;
                    }
                });
            p.audioData.reset();
            p.audioDataSamplesOffset = 0;
            p.audioDataSamplesCount = 0;
            if (_hasAudioSyncPlayback())
            {
                _startAudio();
            }
            break;
        }
        default: break;
        }
    }
}

void Media::seek(Math::Frame::Index value)
{
    DJV_PRIVATE_PTR();
    if (p.read)
    {
        p.read->seek(value);
    }
}

bool Media::_hasVideo() const
{
    DJV_PRIVATE_PTR();
    return !p.info->get().video.empty();
}

bool Media::_hasAudio() const
{
    DJV_PRIVATE_PTR();
    return
        p.rtAudio &&
        p.info->get().audio.isValid();
}

bool Media::_hasAudioSyncPlayback() const
{
    DJV_PRIVATE_PTR();
    return
        _hasAudio() &&
        Playback::Forward == p.playback->get();
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
            _log(DJV_TEXT("Cannot start audio"), e.what());
        }
    }
}

void Media::_stopAudio()
{
    DJV_PRIVATE_PTR();
    if (auto context = p.context.lock())
    {
        if (_hasAudio() && p.rtAudio->isStreamRunning())
        {
            try
            {
                p.rtAudio->abortStream();
                p.rtAudio->setStreamTime(0.0);
            }
            catch (const std::exception& e)
            {
                _log(DJV_TEXT("Cannot stop audio"), e.what());
            }
        }
    }
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
    const auto& info = media->_p->info->get().audio;

    size_t outputSampleCount = static_cast<size_t>(nFrames);
    size_t sampleCount = 0;
    const size_t sampleByteCount = info.channelCount * Audio::getByteCount(info.type);
    const float volume = !media->_p->audioMute->get() ? media->_p->audioVolume->get() : 0.F;

    if (media->_p->audioData)
    {
        sampleCount += media->_p->audioData->getSampleCount() - media->_p->audioDataSamplesOffset;
    }

    // Get audio frames from the read queue.
    std::vector<AudioFrame> frames;
    {
        std::lock_guard<std::mutex> lock(media->_p->read->getMutex());
        auto& queue = media->_p->read->getAudioQueue();
        while (!queue.isEmpty() && sampleCount < outputSampleCount)
        {
            auto frame = queue.getFrame();
            frames.push_back(frame);
            queue.popFrame();
            sampleCount += frame.audio->getSampleCount();
        }
    }

    // Use the remaining data from the frame.
    uint8_t* p = reinterpret_cast<uint8_t*>(outputBuffer);
    if (media->_p->audioData)
    {
        const size_t size = std::min(
            media->_p->audioData->getSampleCount() - media->_p->audioDataSamplesOffset,
            outputSampleCount);
        //memcpy(
        //    p,
        //    media->_p->audioData->getData() + media->_p->audioDataSamplesOffset * sampleByteCount,
        //    size * sampleByteCount);
        Audio::volume(
            media->_p->audioData->getData() + media->_p->audioDataSamplesOffset * sampleByteCount,
            p,
            volume,
            size,
            info.channelCount,
            info.type);
        p += size * sampleByteCount;
        media->_p->audioDataSamplesOffset += size;
        media->_p->audioDataSamplesCount += size;
        outputSampleCount -= size;
        if (media->_p->audioDataSamplesOffset >= media->_p->audioData->getSampleCount())
        {
            media->_p->audioData.reset();
            media->_p->audioDataSamplesOffset = 0;
        }
    }

    // Process the frames.
    for (const auto& i : frames)
    {
        media->_p->audioData = i.audio;
        size_t size = std::min(i.audio->getSampleCount(), outputSampleCount);
        //memcpy(
        //    p,
        //    i.audio->getData(),
        //    size * sampleByteCount);
        Audio::volume(
            i.audio->getData(),
            p,
            volume,
            size,
            info.channelCount,
            info.type);
        p += size * sampleByteCount;
        media->_p->audioDataSamplesOffset = size;
        media->_p->audioDataSamplesCount += size;
        outputSampleCount -= size;
    }

    const size_t zero = (nFrames * sampleByteCount) - (p - reinterpret_cast<uint8_t*>(outputBuffer));
    if (zero)
    {
        //! \todo Is this the correct way to clear the audio data?
        memset(p, 0, zero);
    }

    return 0;
}

void Media::_rtAudioErrorCallback(
    RtAudioError::Type type,
    const std::string& errorText)
{}

Timestamp Media::_getTimestamp() const
{
    DJV_PRIVATE_PTR();
    Timestamp out = 0;
    const auto rate = timebase.swap();
    if (_hasAudioSyncPlayback())
    {
        out = p.playbackStart +
            AV::Time::scale(
                p.audioDataSamplesCount,
                Math::Rational(1, static_cast<int>(p.info->get().audio.sampleRate)),
                timebase);
    }
    else
    {
        out = p.playbackStart + p.playbackTime * rate.getNum() / rate.getDen();
    }
    return out;
}

void Media::_tick()
{
    DJV_PRIVATE_PTR();
    if (p.read)
    {
        switch (p.playback->get())
        {
        case Playback::Forward:
        case Playback::Reverse:
        {
            const auto now = std::chrono::steady_clock::now();
            p.playbackTime = std::chrono::duration<float>(now - p.playbackStartTime).count();
            break;
        }
        default: break;
        }
        _videoTick();
        _audioTick();
    }
}

void Media::_videoTick()
{
    DJV_PRIVATE_PTR();

    const Timestamp ts = _getTimestamp();
    VideoFrame frame;
    bool valid = false;
    size_t queueSize = 0;
    bool finished = false;
    {
        std::lock_guard<std::mutex> lock(p.read->getMutex());
        auto& queue = p.read->getVideoQueue();
        const bool seek = queue.isSeek();
        finished = queue.isFinished() && queue.isEmpty();
        while (!queue.isEmpty() && (queue.getFrame().timestamp <= ts || seek))
        {
            frame = queue.popFrame();
            //std::cout << "Media video frame: " << frame.timestamp << " / " << ts << " / " << seek << std::endl;
            valid = true;
            if (seek)
            {
                p.playbackTime = 0.F;
                p.playbackStart = frame.timestamp;
                p.playbackStartTime = std::chrono::steady_clock::now();
                p.audioData.reset();
                p.audioDataSamplesOffset = 0;
                p.audioDataSamplesCount = 0;
            }
            p.fpsCount++;
        }
        if (valid)
        {
            queue.setSeek(false);
            //std::cout << std::endl;
        }
        queueSize = queue.getCount();
    }

    if (valid)
    {
        p.image->setIfChanged(frame.image);
        p.timestamp->setIfChanged(frame.timestamp);
        p.videoQueueSize->setAlways(queueSize);
    }
}

void Media::_audioTick()
{
    DJV_PRIVATE_PTR();

    const Timestamp ts = _getTimestamp();
    AudioFrame frame;
    bool valid = false;
    size_t queueSize = 0;
    bool finished = false;
    const bool hasVideo = _hasVideo();
    {
        std::lock_guard<std::mutex> lock(p.read->getMutex());
        auto& queue = p.read->getAudioQueue();
        const bool seek = queue.isSeek();
        finished = queue.isFinished() && queue.isEmpty();
        while (!queue.isEmpty() && (queue.getFrame().timestamp <= ts || seek))
        {
            frame = queue.popFrame();
            //std::cout << "Media audio frame: " << frame.timestamp << " / " << ts << " / " << seek << std::endl;
            valid = true;
            if (seek && !hasVideo)
            {
                p.playbackTime = 0.F;
                p.playbackStart = frame.timestamp;
                p.playbackStartTime = std::chrono::steady_clock::now();
                p.audioData.reset();
                p.audioDataSamplesOffset = 0;
                p.audioDataSamplesCount = 0;
            }
        }
        if (valid)
        {
            queue.setSeek(false);
        }
        queueSize = queue.getCount();
    }

    if (valid)
    {
        p.audioQueueSize->setAlways(queueSize);
    }
}

void Media::_log(const std::string& message, const std::string& what)
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
