// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/IOTest.h>

#include <djvAV/IOSystem.h>
#include <djvAV/PPM.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/LogSystem.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/String.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::IO;

namespace djv
{
    namespace AVTest
    {
        IOTest::IOTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest(
                "djv::AVTest::IOTest",
                FileSystem::Path(tempPath, "IOTest"),
                context)
        {}
        
        void IOTest::run()
        {
            _info();
            _videoFrame();
            _videoQueue();
            _audioFrame();
            _audioQueue();
            _inOutPoints();
            _cache();
            _plugin();
            _io();
            _system();
        }
        
        void IOTest::_info()
        {
            {
                const Info info;
                DJV_ASSERT(info.fileName.empty());
                DJV_ASSERT(info.video.empty());
                DJV_ASSERT(info.videoSpeed == Time::fromSpeed(Time::getDefaultSpeed()));
                DJV_ASSERT(info.videoSequence == Frame::Sequence());
                DJV_ASSERT(info.audio == Audio::Info());
                DJV_ASSERT(info.tags.isEmpty());
            }
            
            {
                Info info;
                info.fileName = "render.exr";
                info.video.push_back(Image::Info(1, 2, Image::Type::RGB_U8));
                DJV_ASSERT(info == info);
            }
        }
        
        void IOTest::_videoFrame()
        {
            {
                const VideoFrame frame;
                DJV_ASSERT(0 == frame.frame);
                DJV_ASSERT(!frame.image.get());
            }
            
            {
                const Frame::Number number = 1;
                auto image = Image::Image::create(Image::Info(1, 2, Image::Type::RGB_U8));
                const VideoFrame frame(number, image);
                DJV_ASSERT(number == frame.frame);
                DJV_ASSERT(image == frame.image);
            }
        }
        
        void IOTest::_videoQueue()
        {
            {
                const VideoQueue queue;
                DJV_ASSERT(0 == queue.getMax());
                DJV_ASSERT(queue.isEmpty());
                DJV_ASSERT(0 == queue.getCount());
                DJV_ASSERT(VideoFrame() == queue.getFrame());
                DJV_ASSERT(!queue.isFinished());
            }
            
            {
                VideoQueue queue;
                queue.setMax(1000);
                DJV_ASSERT(1000 == queue.getMax());
                const VideoFrame frame(1, nullptr);
                queue.addFrame(frame);
                queue.addFrame(VideoFrame(2, nullptr));
                queue.addFrame(VideoFrame(3, nullptr));
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
                const AudioFrame frame;
                DJV_ASSERT(!frame.audio.get());
            }
            
            {
                auto audio = Audio::Data::create(Audio::Info(1, Audio::Type::S16, 2, 3));
                const AudioFrame frame(audio);
                DJV_ASSERT(audio == frame.audio);
            }
        }
        
        void IOTest::_audioQueue()
        {
            {
                const AudioQueue queue;
                DJV_ASSERT(0 == queue.getMax());
                DJV_ASSERT(queue.isEmpty());
                DJV_ASSERT(0 == queue.getCount());
                DJV_ASSERT(AudioFrame() == queue.getFrame());
                DJV_ASSERT(!queue.isFinished());
            }

            {
                AudioQueue queue;
                queue.setMax(1000);
                DJV_ASSERT(1000 == queue.getMax());
                auto audio = Audio::Data::create(Audio::Info(1, Audio::Type::S16, 2, 3));
                const AudioFrame frame(audio);
                queue.addFrame(frame);
                queue.addFrame(AudioFrame(nullptr));
                queue.addFrame(AudioFrame(nullptr));
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
        
        void IOTest::_inOutPoints()
        {
            {
                const InOutPoints inOutPoints;
                DJV_ASSERT(!inOutPoints.isEnabled());
                DJV_ASSERT(Frame::invalid == inOutPoints.getIn());
                DJV_ASSERT(Frame::invalid == inOutPoints.getOut());
                DJV_ASSERT(Math::Range<Frame::Index>(0, 0) == inOutPoints.getRange(1));
            }
            
            {
                const InOutPoints inOutPoints(true, 1, 2);
                DJV_ASSERT(inOutPoints.isEnabled());
                DJV_ASSERT(1 == inOutPoints.getIn());
                DJV_ASSERT(2 == inOutPoints.getOut());
                DJV_ASSERT(Math::Range<Frame::Index>(1, 2) == inOutPoints.getRange(4));
            }
            
            {
                const InOutPoints inOutPoints(true, 1, 2);
                DJV_ASSERT(inOutPoints == inOutPoints);
            }
        }
        
        void IOTest::_cache()
        {
            {
                const Cache cache;
                DJV_ASSERT(0 == cache.getMax());
                DJV_ASSERT(0 == cache.getTotalByteCount());
                DJV_ASSERT(Frame::Sequence() == cache.getFrames());
                DJV_ASSERT(Frame::Sequence() == cache.getSequence());
                DJV_ASSERT(!cache.contains(0));
                std::shared_ptr<Image::Image> image;
                DJV_ASSERT(!cache.get(0, image));
            }
            
            {
                Cache cache;
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
                cache.setDirection(Direction::Reverse);
                cache.setDirection(Direction::Reverse);
                for (Frame::Index i = 20; i >= 0; --i)
                {
                    cache.add(i, Image::Image::create(Image::Info(1, 2, Image::Type::RGB_U8)));
                }
                std::shared_ptr<Image::Image> image;
                for (Frame::Index i = 0; i < 20; ++i)
                {
                    cache.get(i, image);
                }
                {
                    std::stringstream ss;
                    ss << "Cache frames: " << cache.getFrames();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Cache sequence: " << cache.getSequence();
                    _print(ss.str());
                }
            }
        }
        
        void IOTest::_plugin()
        {
            if (auto context = getContext().lock())
            {
                auto plugin = PPM::Plugin::create(context);
                _print("Plugin name: " + plugin->getPluginName());
                _print("Plugin info: " + _getText(plugin->getPluginInfo()));
                _print("Plugin file extensions: " + String::joinSet(plugin->getFileExtensions(), ", "));
                DJV_ASSERT(plugin->canSequence());
                DJV_ASSERT(plugin->canRead(FileSystem::FileInfo("image.ppm")));
                Info info;
                info.video.push_back(Image::Info(64, 64, Image::Type::RGB_U8));
                DJV_ASSERT(plugin->canWrite(FileSystem::FileInfo("image.ppm"), info));
            }
            
            if (auto context = getContext().lock())
            {
                const ReadOptions options;
                auto read = PPM::Read::create(
                    FileSystem::FileInfo(),
                    options,
                    context->getSystemT<TextSystem>(),
                    context->getSystemT<ResourceSystem>(),
                    context->getSystemT<LogSystem>());
                DJV_ASSERT(4 == read->getThreadCount());
                read->setThreadCount(1);
                DJV_ASSERT(1 == read->getThreadCount());                
                read->setPlayback(true);
                read->setLoop(true);
                read->setInOutPoints(InOutPoints(true, 1, 2));
            }

            if (auto context = getContext().lock())
            {
                const ReadOptions options;
                auto read = PPM::Read::create(
                    FileSystem::FileInfo(),
                    options,
                    context->getSystemT<TextSystem>(),
                    context->getSystemT<ResourceSystem>(),
                    context->getSystemT<LogSystem>());
                DJV_ASSERT(!read->hasCache());
                DJV_ASSERT(!read->isCacheEnabled());
                {
                    std::stringstream ss;
                    ss << read->getCacheMaxByteCount();
                    _print("Cache max byte count: " + ss.str());
                }
                {
                    std::stringstream ss;
                    ss << read->getCacheByteCount();
                    _print("Cache byte count: " + ss.str());
                }
                {
                    std::stringstream ss;
                    ss << read->getCacheSequence();
                    _print("Cache sequence: " + ss.str());
                }
                {
                    std::stringstream ss;
                    ss << read->getCachedFrames();
                    _print("Cached frames: " + ss.str());
                }
                read->setCacheEnabled(true);
                read->setCacheMaxByteCount(0);
            }
        }
        
        void IOTest::_io()
        {
            if (auto context = getContext().lock())
            {
                struct PluginInfo
                {
                    std::string extension;
                    std::vector<rapidjson::Value> options;
                };
                std::map<std::string, PluginInfo> pluginInfo;
                pluginInfo["Cineon"].extension = ".cineon";
                pluginInfo["DPX"].extension = ".dpx";
                pluginInfo["PNG"].extension = ".png";
                pluginInfo["PPM"].extension = ".ppm";
                
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                PPM::Options ppmOptions;
                ppmOptions.data = PPM::Data::ASCII;
                pluginInfo["PPM"].options.push_back(toJSON(ppmOptions, allocator));
                
                const std::vector<Image::Size> sizes =
                {
                    Image::Size(0, 0),
                    Image::Size(1, 1),
                    Image::Size(11, 11),
                    Image::Size(32, 32)
                };
                const std::vector<Image::Type> types = Image::getTypeEnums();
                
                Tags tags;
                tags.set("Description", "This is a description.");
                tags.set("Time", "Tue Oct 8 13:18:20 PDT 2019");
                
                auto io = context->getSystemT<System>();
                for (const auto& i : pluginInfo)
                {
                    for (const auto& size : sizes)
                    {
                        for (const auto& type : types)
                        {
                            _io(i.first, i.second.extension, size, type, tags, io);
                        }
                    }
                }
                for (const auto& i : pluginInfo)
                {
                    for (const auto& size : sizes)
                    {
                        for (const auto& type : types)
                        {
                            for (const auto& options : i.second.options)
                            {
                                io->setOptions(i.first, options);
                                _io(i.first, i.second.extension, size, type, tags, io);
                            }
                        }
                    }
                }
            }
        }
        
        void IOTest::_io(
            const std::string& name,
            const std::string& extension,
            const Image::Size size,
            Image::Type type,
            const Tags& tags,
            const std::shared_ptr<System>& io)
        {
            try
            {
                const Image::Info imageInfo(size, type);
                auto image = Image::Image::create(imageInfo);
                image->setTags(tags);
                image->zero();
                
                std::stringstream ss;
                ss << size.w << "x" << size.h << "_" << type << extension;
                _print(ss.str());
                FileSystem::Path path(getTempPath(), ss.str());
                {
                    Info info;
                    info.video.push_back(imageInfo);
                    auto write = io->write(FileSystem::FileInfo(path), info);
                    {
                        std::lock_guard<std::mutex> lock(write->getMutex());
                        auto& writeQueue = write->getVideoQueue();
                        writeQueue.addFrame(VideoFrame(0, image));
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
            catch (const std::exception& e)
            {
                std::cout << Error::format(e) << std::endl;
            }
        }
        
        void IOTest::_system()
        {
            if (auto context = getContext().lock())
            {
                auto io = context->getSystemT<System>();
                const auto pluginNames = io->getPluginNames();
                {
                    std::stringstream ss;
                    ss << "Plugins: " << String::joinSet(pluginNames, ", ");
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "File extensions: " << String::joinSet(io->getFileExtensions(), ", ");
                    _print(ss.str());
                }

                auto observer = ValueObserver<bool>::create(
                    io->observeOptionsChanged(),
                    [this](bool value)
                    {
                        std::stringstream ss;
                        ss << "Options changed: " << value;
                        _print(ss.str());
                    });
                for (const auto& i : pluginNames)
                {
                    rapidjson::Document document;
                    auto& allocator = document.GetAllocator();
                    auto json = io->getOptions(i, allocator);
                    try
                    {
                        io->setOptions(i, rapidjson::Value());
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << Error::format(e) << std::endl;
                    }
                }
                
                {
                    std::stringstream ss;
                    ss << "Sequence extensions: " << String::joinSet(io->getSequenceExtensions(), ", ");
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
                    ss << "Can sequence: " << i << " = " << io->canSequence(FileSystem::FileInfo(i));
                    _print(ss.str());
                }
                for (const auto& i : fileNames)
                {
                    std::stringstream ss;
                    ss << "Can read: " << i << " = " << io->canRead(FileSystem::FileInfo(i));
                    _print(ss.str());
                }
                for (const auto& i : fileNames)
                {
                    std::stringstream ss;
                    ss << "Can write: " << i << " = ";
                    ss << io->canWrite(FileSystem::FileInfo(i), Info());
                    _print(ss.str());
                }
            }
        }
                
    } // namespace AVTest
} // namespace djv

