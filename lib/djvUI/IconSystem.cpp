// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/IconSystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IOSystem.h>
#include <djvAV/Image.h>

#include <djvCore/Cache.h>
#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/Timer.h>

#include <atomic>
#include <thread>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t imageCacheMax = 1000;

            struct ImageRequest
            {
                ImageRequest(const std::string& name, uint16_t size) :
                    name(name),
                    size(size)
                {
                    key = 0;
                    Memory::hashCombine(key, name);
                    Memory::hashCombine(key, size);
                }

                ImageRequest(ImageRequest && other) :
                    name   (std::move(other.name)),
                    size   (std::move(other.size)),
                    key    (std::move(other.key)),
                    path   (std::move(other.path)),
                    read   (std::move(other.read)),
                    promise(std::move(other.promise))
                {}

                ~ImageRequest()
                {}

                ImageRequest& operator = (ImageRequest && other)
                {
                    if (this != &other)
                    {
                        name    = std::move(other.name);
                        size    = std::move(other.size);
                        key     = std::move(other.key);
                        path    = std::move(other.path);
                        read    = std::move(other.read);
                        promise = std::move(other.promise);
                    }
                    return *this;
                }

                std::string name;
                uint16_t size;
                size_t key;
                FileSystem::Path path;
                std::shared_ptr<AV::IO::IRead> read;
                std::promise<std::shared_ptr<AV::Image::Image> > promise;
            };

        } // namespace

        struct IconSystem::Private
        {
            FileSystem::Path iconPath;
            std::vector<uint16_t> dpiList;
            std::shared_ptr<AV::IO::System> io;
            std::list<ImageRequest> imageQueue;
            std::condition_variable requestCV;
            std::mutex requestMutex;
            std::list<ImageRequest> newImageRequests;
            std::list<ImageRequest> pendingImageRequests;

            Memory::Cache<size_t, std::shared_ptr<AV::Image::Image> > imageCache;
            std::atomic<float> imageCachePercentage;

            std::shared_ptr<Time::Timer> statsTimer;
            std::thread thread;
            std::atomic<bool> running;

            FileSystem::Path getPath(const std::string& name, uint16_t dpi) const;
            uint16_t findClosestDPI(uint16_t) const;
        };

        void IconSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISystem::_init("djv::UI::IconSystem", context);

            DJV_PRIVATE_PTR();

            addDependency(context->getSystemT<AV::AVSystem>());

            p.iconPath = context->getSystemT<ResourceSystem>()->getPath(FileSystem::ResourcePath::Icons);
                        
            p.io = context->getSystemT<AV::IO::System>();

            p.imageCache.setMax(imageCacheMax);
            p.imageCachePercentage = 0.F;

            p.statsTimer = Time::Timer::create(context);
            p.statsTimer->setRepeating(true);
            p.statsTimer->start(
                Time::getTime(Time::TimerValue::VerySlow),
                [this](const std::chrono::steady_clock::time_point&, const Time::Duration&)
            {
                DJV_PRIVATE_PTR();
                std::stringstream ss;
                {
                    ss << "Image cache: " << p.imageCachePercentage << '%';
                }
                _log(ss.str());
            });

            p.running = true;
            p.thread = std::thread(
                [this]
            {
                DJV_PRIVATE_PTR();
                try
                {
                    // Find the DPI values.
                    for (const auto& i : FileSystem::directoryList(p.iconPath))
                    {
                        const std::string fileName = i.getFileName(Frame::invalid, false);
                        const size_t size = fileName.size();
                        if (size > 3 &&
                            fileName[size - 3] == 'D' &&
                            fileName[size - 2] == 'P' &&
                            fileName[size - 1] == 'I')
                        {
                            p.dpiList.push_back(std::stoi(fileName.substr(0, size - 3)));
                        }
                    }
                    std::sort(p.dpiList.begin(), p.dpiList.end());
                    for (const auto& i : p.dpiList)
                    {
                        std::stringstream ss;
                        ss << "Found DPI: " << i;
                        _log(ss.str());
                    }

                    const auto timeout = Time::getValue(Time::TimerValue::Medium);
                    while (p.running)
                    {
                        {
                            std::unique_lock<std::mutex> lock(p.requestMutex);
                            if (p.requestCV.wait_for(
                                lock,
                                std::chrono::milliseconds(timeout),
                                [this]
                            {
                                DJV_PRIVATE_PTR();
                                return p.imageQueue.size();
                            }))
                            {
                                p.newImageRequests = std::move(p.imageQueue);
                            }
                        }
                        if (p.newImageRequests.size() || p.pendingImageRequests.size())
                        {
                            _handleImageRequests();
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            });
        }

        IconSystem::IconSystem() :
            _p(new Private)
        {}

        IconSystem::~IconSystem()
        {
            DJV_PRIVATE_PTR();
            p.running = false;
            if (p.thread.joinable())
            {
                p.thread.join();
            }
        }

        std::shared_ptr<IconSystem> IconSystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<IconSystem>(new IconSystem);
            out->_init(context);
            return out;
        }

        std::future<std::shared_ptr<AV::Image::Image> > IconSystem::getIcon(const std::string& name, float size)
        {
            DJV_PRIVATE_PTR();
            ImageRequest request(name, static_cast<uint16_t>(Math::clamp(size, 0.F, 65535.F)));
            auto future = request.promise.get_future();
            {
                std::unique_lock<std::mutex> lock(p.requestMutex);
                p.imageQueue.push_back(std::move(request));
            }
            p.requestCV.notify_one();
            return future;
        }

        float IconSystem::getCachePercentage() const
        {
            return _p->imageCachePercentage;
        }

        void IconSystem::_handleImageRequests()
        {
            DJV_PRIVATE_PTR();

            // Process new requests.
            for (auto& i : p.newImageRequests)
            {
                std::shared_ptr<AV::Image::Image> image;
                p.imageCache.get(i.key, image);
                if (!image)
                {
                    try
                    {
                        i.path = p.getPath(i.name, p.findClosestDPI(i.size));
                        i.read = p.io->read(i.path);
                        p.pendingImageRequests.push_back(std::move(i));
                    }
                    catch (const std::exception& e)
                    {
                        try
                        {
                            i.promise.set_exception(std::current_exception());
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                        _log(e.what(), LogLevel::Error);
                    }
                }
                if (image)
                {
                    i.promise.set_value(image);
                }
            }
            p.newImageRequests.clear();

            // Process pending requests.
            auto i = p.pendingImageRequests.begin();
            while (i != p.pendingImageRequests.end())
            {
                std::shared_ptr<AV::Image::Image> image;
                bool finished = false;
                {
                    std::lock_guard<std::mutex> lock(i->read->getMutex());
                    auto& queue = i->read->getVideoQueue();
                    if (!queue.isEmpty())
                    {
                        image = queue.getFrame().image;
                    }
                    else if (queue.isFinished())
                    {
                        finished = true;
                    }
                }
                if (image)
                {
                    p.imageCache.add(i->key, image);
                    p.imageCachePercentage = p.imageCache.getPercentageUsed();
                    i->promise.set_value(image);
                    i = p.pendingImageRequests.erase(i);
                }
                else if (finished)
                {
                    try
                    {
                        std::stringstream ss;
                        ss << "Error loading image" << " '" << i->path << "'.";
                        throw FileSystem::Error(ss.str());
                    }
                    catch (const std::exception& e)
                    {
                        try
                        {
                            i->promise.set_exception(std::current_exception());
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                        _log(e.what(), LogLevel::Error);
                    }
                    i = p.pendingImageRequests.erase(i);
                }
                else
                {
                    ++i;
                }
            }
        }

        FileSystem::Path IconSystem::Private::getPath(const std::string& name, uint16_t dpi) const
        {
            FileSystem::Path out = iconPath;
            {
                std::stringstream ss;
                ss << dpi << "DPI";
                out = FileSystem::Path(out, ss.str());
            }
            {
                std::stringstream ss;
                ss << name << ".png";
                out = FileSystem::Path(out, ss.str());
            }
            return out;
        }

        uint16_t IconSystem::Private::findClosestDPI(uint16_t value) const
        {
            const uint16_t dpi = static_cast<uint16_t>(value / static_cast<float>(Style::iconSizeDefault) * static_cast<float>(AV::dpiDefault));
            std::map<uint16_t, uint16_t> differenceToDPI;
            for (auto i : dpiList)
            {
                differenceToDPI[Math::abs(dpi - i)] = i;
            }
            return differenceToDPI.size() ? differenceToDPI.begin()->second : dpi;
        }

    } // namespace UI
} // namespace djv
