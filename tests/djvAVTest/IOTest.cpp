// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/IOTest.h>

#include <djvAV/IO.h>

#include <djvCore/Context.h>
#include <djvCore/String.h>
#include <djvCore/Timer.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        IOTest::IOTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::IOTest", context)
        {}
        
        void IOTest::run()
        {
            _videoInfo();
            _audioInfo();
            _info();
            _videoFrame();
            _videoQueue();
            _audioFrame();
            _audioQueue();
            _cache();
            _io();
            _system();
            _operators();
        }
        
        void IOTest::_videoInfo()
        {
            {
                const AV::IO::VideoInfo info;
                DJV_ASSERT(info.info == Image::Info());
                DJV_ASSERT(info.speed == Time::Speed());
                DJV_ASSERT(info.sequence == Frame::Sequence());
                DJV_ASSERT(info.codec.empty());
            }
            
            {
                const Image::Info imageInfo(1, 2, Image::Type::RGB_U8);
                const Time::Speed speed(Time::FPS::_60);
                const Frame::Sequence sequence(3, 4);
                const AV::IO::VideoInfo info(imageInfo, speed, sequence);
                DJV_ASSERT(info.info == imageInfo);
                DJV_ASSERT(info.speed == speed);
                DJV_ASSERT(info.sequence == sequence);
            }
        }
        
        void IOTest::_audioInfo()
        {
            {
                const AV::IO::AudioInfo info;
                DJV_ASSERT(info.info == Audio::Info());
            }
            
            {
                const Audio::Info audioInfo(1, Audio::Type::S16, 2, 3);
                const size_t sampleCount = 4;
                const AV::IO::AudioInfo info(audioInfo, sampleCount);
                DJV_ASSERT(info.info == audioInfo);
            }
        }
        
        void IOTest::_info()
        {
            {
                const IO::Info info;
                DJV_ASSERT(info.fileName.empty());
                DJV_ASSERT(info.video.empty());
                DJV_ASSERT(info.audio.empty());
                DJV_ASSERT(info.tags.isEmpty());
            }
            
            {
                const std::string fileName = "fileName";
                const IO::VideoInfo videoInfo(Image::Info(1, 2, Image::Type::RGB_U8));
                const std::vector<IO::VideoInfo> videoInfoList = { videoInfo };
                const IO::AudioInfo audioInfo(Audio::Info(1, Audio::Type::S16, 2, 3));
                const std::vector<IO::AudioInfo> audioInfoList = { audioInfo };
                
                const IO::Info info(fileName, videoInfo);
                DJV_ASSERT(fileName == info.fileName);
                DJV_ASSERT(videoInfo == info.video[0]);
                
                const IO::Info info2(fileName, audioInfo);
                DJV_ASSERT(fileName == info2.fileName);
                DJV_ASSERT(audioInfo == info2.audio[0]);
                
                const IO::Info info3(fileName, videoInfo, audioInfo);
                DJV_ASSERT(fileName == info3.fileName);
                DJV_ASSERT(videoInfo == info3.video[0]);
                DJV_ASSERT(audioInfo == info3.audio[0]);
                
                const IO::Info info4(fileName, videoInfoList, audioInfoList);
                DJV_ASSERT(fileName == info4.fileName);
                DJV_ASSERT(videoInfoList == info4.video);
                DJV_ASSERT(audioInfoList == info4.audio);
            }
        }
        
        void IOTest::_videoFrame()
        {
            {
                const IO::VideoFrame frame;
                DJV_ASSERT(0 == frame.frame);
                DJV_ASSERT(!frame.image.get());
            }
            
            {
                const Frame::Number number = 1;
                auto image = Image::Image::create(Image::Info(1, 2, Image::Type::RGB_U8));
                const IO::VideoFrame frame(number, image);
                DJV_ASSERT(number == frame.frame);
                DJV_ASSERT(image == frame.image);
            }
        }
        
        void IOTest::_videoQueue()
        {
            {
                const IO::VideoQueue queue;
                DJV_ASSERT(0 == queue.getMax());
                DJV_ASSERT(queue.isEmpty());
                DJV_ASSERT(0 == queue.getCount());
                DJV_ASSERT(IO::VideoFrame() == queue.getFrame());
                DJV_ASSERT(!queue.isFinished());
            }
            
            {
                IO::VideoQueue queue;
                queue.setMax(1000);
                DJV_ASSERT(1000 == queue.getMax());
                const IO::VideoFrame frame(1, nullptr);
                queue.addFrame(frame);
                queue.addFrame(IO::VideoFrame(2, nullptr));
                queue.addFrame(IO::VideoFrame(3, nullptr));
                DJV_ASSERT(!queue.isEmpty());
                DJV_ASSERT(3 == queue.getCount());
                DJV_ASSERT(frame == queue.getFrame());
                DJV_ASSERT(frame == queue.popFrame());
                queue.clearFrames();
                DJV_ASSERT(queue.isEmpty());
                queue.setFinished(true);
                DJV_ASSERT(queue.isFinished());
            }
        }
        
        void IOTest::_audioFrame()
        {
            {
                const IO::AudioFrame frame;
                DJV_ASSERT(!frame.audio.get());
            }
            
            {
                auto audio = Audio::Data::create(Audio::Info(1, Audio::Type::S16, 2, 3));
                const IO::AudioFrame frame(audio);
                DJV_ASSERT(audio == frame.audio);
            }
        }
        
        void IOTest::_audioQueue()
        {
            {
                const IO::AudioQueue queue;
                DJV_ASSERT(0 == queue.getMax());
                DJV_ASSERT(queue.isEmpty());
                DJV_ASSERT(0 == queue.getCount());
                DJV_ASSERT(IO::AudioFrame() == queue.getFrame());
                DJV_ASSERT(!queue.isFinished());
            }

            {
                IO::AudioQueue queue;
                queue.setMax(1000);
                DJV_ASSERT(1000 == queue.getMax());
                auto audio = Audio::Data::create(Audio::Info(1, Audio::Type::S16, 2, 3));
                const IO::AudioFrame frame(audio);
                queue.addFrame(frame);
                queue.addFrame(IO::AudioFrame(nullptr));
                queue.addFrame(IO::AudioFrame(nullptr));
                DJV_ASSERT(!queue.isEmpty());
                DJV_ASSERT(3 == queue.getCount());
                DJV_ASSERT(frame == queue.getFrame());
                DJV_ASSERT(frame == queue.popFrame());
                queue.clearFrames();
                DJV_ASSERT(queue.isEmpty());
                queue.setFinished(true);
                DJV_ASSERT(queue.isFinished());
            }
        }
        
        void IOTest::_cache()
        {
            {
                const IO::Cache cache;
                DJV_ASSERT(0 == cache.getMax());
                DJV_ASSERT(0 == cache.getTotalByteCount());
                DJV_ASSERT(Frame::Sequence() == cache.getFrames());
                DJV_ASSERT(Frame::Sequence() == cache.getSequence());
                DJV_ASSERT(!cache.contains(0));
                std::shared_ptr<AV::Image::Image> image;
                DJV_ASSERT(!cache.get(0, image));
            }
            
            {
                IO::Cache cache;
                cache.setMax(10);
                DJV_ASSERT(10 == cache.getMax());
                cache.setCurrentFrame(1);
                cache.setCurrentFrame(1);
                cache.setSequenceSize(10);
                for (Frame::Index i = 0; i < 20; ++i)
                {
                    cache.add(i, Image::Image::create(Image::Info(1, 2, Image::Type::RGB_U8)));
                }
                DJV_ASSERT(cache.getCount() > 0);
                for (Frame::Index i = 0; i < 20; i += 3)
                {
                    cache.add(i, Image::Image::create(Image::Info(1, 2, Image::Type::RGB_U8)));
                }
                cache.setDirection(IO::Direction::Reverse);
                cache.setDirection(IO::Direction::Reverse);
                for (Frame::Index i = 20; i >= 0; --i)
                {
                    cache.add(i, Image::Image::create(Image::Info(1, 2, Image::Type::RGB_U8)));
                }
                std::shared_ptr<AV::Image::Image> image;
                for (Frame::Index i = 0; i < 20; ++i)
                {
                    cache.get(i, image);
                }
                {
                    std::stringstream ss;
                    ss << "cache frames: " << cache.getFrames();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "cache sequence: " << cache.getSequence();
                    _print(ss.str());
                }
            }
        }
        
        void IOTest::_io()
        {
            if (auto context = getContext().lock())
            {
                const std::set<std::string> extensions =
                {
                    ".cin",
                    ".dpx",
                    ".ppm",
                    ".png",
                    ".txt"
                };
                const std::vector<Image::Size> sizes =
                {
                    Image::Size(0, 0),
                    Image::Size(1, 1),
                    Image::Size(11, 11),
                    Image::Size(32, 32)
                };
                const std::vector<Image::Type> types = Image::getTypeEnums();
                Tags tags;
                tags.setTag("Description", "This is a description.");
                tags.setTag("Time", "Tue Oct  8 13:18:20 PDT 2019");
                auto io = context->getSystemT<AV::IO::System>();
                for (const auto& extension : extensions)
                {
                    for (const auto& size : sizes)
                    {
                        for (const auto& type : types)
                        {
                            try
                            {
                                const Image::Info imageInfo(size, type);
                                auto image = Image::Image::create(imageInfo);
                                image->setTags(tags);
                                image->zero();
                                
                                std::stringstream ss;
                                ss << "IOTest" << "_" << size.w << "x" << size.h << "_" << type << extension;
                                _print(ss.str());
                                FileSystem::Path path(ss.str());
                                {
                                    IO::Info info;
                                    info.video.push_back(imageInfo);
                                    auto write = io->write(FileSystem::FileInfo(path), info);
                                    {
                                        std::lock_guard<std::mutex> lock(write->getMutex());
                                        auto& writeQueue = write->getVideoQueue();
                                        writeQueue.addFrame(IO::VideoFrame(0, image));
                                        writeQueue.setFinished(true);
                                    }
                                    while (write->isRunning())
                                    {}
                                }

                                {
                                    auto read = io->read(FileSystem::FileInfo(path));
                                    bool running = true;
                                    while (running)
                                    {
                                        bool sleep = false;
                                        {
                                            std::unique_lock<std::mutex> lock(read->getMutex(), std::try_to_lock);
                                            if (lock.owns_lock())
                                            {
                                                auto& readQueue = read->getVideoQueue();
                                                if (!readQueue.isEmpty())
                                                {
                                                    auto frame = readQueue.popFrame();
                                                }
                                                else if (readQueue.isFinished())
                                                {
                                                    running = false;
                                                }
                                                else
                                                {
                                                    sleep = true;
                                                }
                                            }
                                            else
                                            {
                                                sleep = true;
                                            }
                                        }
                                        if (sleep)
                                        {
                                            std::this_thread::sleep_for(Time::getTime(Time::TimerValue::Fast));
                                        }
                                    }
                                }
                            }
                            catch (const std::exception&)
                            {}
                        }
                    }
                }
            }
        }
        
        void IOTest::_system()
        {
            if (auto context = getContext().lock())
            {
                auto io = context->getSystemT<IO::System>();
                const auto pluginNames = io->getPluginNames();
                {
                    std::stringstream ss;
                    ss << "plugins: " << String::joinSet(pluginNames, ", ");
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "file extensions: " << String::joinSet(io->getFileExtensions(), ", ");
                    _print(ss.str());
                }

                auto observer = ValueObserver<bool>::create(
                    io->observeOptionsChanged(),
                    [this](bool value)
                    {
                        std::stringstream ss;
                        ss << "options changed: " << value;
                        _print(ss.str());
                    });
                for (const auto& i : pluginNames)
                {
                    auto json = io->getOptions(i);
                    try
                    {
                        io->setOptions(i, picojson::value());
                    }
                    catch (const std::exception&)
                    {}
                }
                
                {
                    std::stringstream ss;
                    ss << "sequence extensions: " << String::joinSet(io->getSequenceExtensions(), ", ");
                    _print(ss.str());
                }
                const std::vector<std::string> fileNames =
                {
                    "render.1.dpx",
                    "render.1.ppm",
                    "render.mov",
                    "tmp.txt"
                };
                for (const auto& i : fileNames)
                {
                    std::stringstream ss;
                    ss << "can sequence: " << i << " = " << io->canSequence(FileSystem::FileInfo(i));
                    _print(ss.str());
                }
                for (const auto& i : fileNames)
                {
                    std::stringstream ss;
                    ss << "can read: " << i << " = " << io->canRead(FileSystem::FileInfo(i));
                    _print(ss.str());
                }
                for (const auto& i : fileNames)
                {
                    std::stringstream ss;
                    ss << "can write: " << i << " = ";
                    ss << io->canWrite(FileSystem::FileInfo(i), IO::Info());
                    _print(ss.str());
                }
            }
        }
        
        void IOTest::_operators()
        {
            {
                const IO::VideoInfo info(
                    Image::Info(1, 2, Image::Type::RGB_U8),
                    Time::Speed(Time::FPS::_60),
                    Frame::Sequence(3, 4));
                DJV_ASSERT(info == info);
            }

            {
                const IO::AudioInfo info(
                    Audio::Info(1, Audio::Type::S16, 2, 3),
                    4);
                DJV_ASSERT(info == info);
            }
            
            {
                const IO::Info info(
                    "fileName",
                    IO::VideoInfo(Image::Info(1, 2, Image::Type::RGB_U8)),
                    IO::AudioInfo(Audio::Info(1, Audio::Type::S16, 2, 3)));
                DJV_ASSERT(info == info);
            }
            
            {
                const IO::VideoFrame frame(
                    1,
                    Image::Image::create(Image::Info(1, 2, Image::Type::RGB_U8)));
                DJV_ASSERT(frame == frame);
            }
            
            {
                const IO::AudioFrame frame(
                    Audio::Data::create(Audio::Info(1, Audio::Type::S16, 2, 3)));
                DJV_ASSERT(frame == frame);
            }
        }
                
    } // namespace AVTest
} // namespace djv

