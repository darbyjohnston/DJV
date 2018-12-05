//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAV/IconSystem.h>

#include <djvAV/Image.h>
#include <djvAV/IO.h>

#include <djvCore/Cache.h>
#include <djvCore/Context.h>
#include <djvCore/Timer.h>

#include <atomic>
#include <mutex>
#include <thread>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace
        {
            //! \todo [1.0 S] Should this be configurable?
            const size_t infoCacheMax = 10000;
            const size_t imageCacheMax = 10000;
            const size_t timeout = 10;
            const size_t statsTimeout = 10000;

            struct InfoRequest
            {
                InfoRequest()
                {}

                InfoRequest(InfoRequest&& other) :
                    path(other.path),
                    promise(std::move(other.promise))
                {}

                InfoRequest& operator = (InfoRequest&& other)
                {
                    if (this != &other)
                    {
                        path = other.path;
                        promise = std::move(other.promise);
                    }
                    return *this;
                }

                Path path;
                std::shared_ptr<IO::IRead> read;
                std::future<IO::Info> infoFuture;
                std::promise<Pixel::Info> promise;
            };

            struct ImageRequest
            {
                ImageRequest()
                {}

                ImageRequest(ImageRequest&& other) :
                    path(other.path),
                    promise(std::move(other.promise))
                {}

                ImageRequest& operator = (ImageRequest&& other)
                {
                    if (this != &other)
                    {
                        path = other.path;
                        promise = std::move(other.promise);
                    }
                    return *this;
                }

                Path path;
                std::shared_ptr<IO::ReadQueue> queue;
                std::shared_ptr<IO::IRead> read;
                std::promise<std::shared_ptr<Image> > promise;
            };

        } // namespace

        struct IconSystem::Private
        {
            std::list<InfoRequest> infoQueue;
            std::list<ImageRequest> imageQueue;
            std::condition_variable requestCV;
            std::mutex requestMutex;
            std::list<InfoRequest> newInfoRequests;
            std::list<InfoRequest> pendingInfoRequests;
            std::list<ImageRequest> newImageRequests;
            std::list<ImageRequest> pendingImageRequests;

            Cache<Path, Pixel::Info> infoCache;
            Cache<Path, std::shared_ptr<Image> > imageCache;
            std::mutex cacheMutex;

            std::shared_ptr<Timer> statsTimer;

            std::thread thread;
            std::atomic<bool> running;
        };

        void IconSystem::_init(const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::AV::IconSystem", context);

            _p->infoCache.setMax(infoCacheMax);
            _p->imageCache.setMax(imageCacheMax);

            _p->statsTimer = Timer::create(context);
            _p->statsTimer->setRepeating(true);
            _p->statsTimer->start(
                std::chrono::milliseconds(statsTimeout),
                [this](float)
            {
                std::lock_guard<std::mutex> lock(_p->cacheMutex);
                std::stringstream s;
                s << "Info cache: " << _p->infoCache.getPercentageUsed() << "%\n";
                s << "Image cache: " << _p->imageCache.getPercentageUsed() << '%';
                _log(s.str());
            });

            _p->running = true;
            _p->thread = std::thread(
                [this]
            {
                while (_p->running)
                {
                    {
                        std::unique_lock<std::mutex> lock(_p->requestMutex);
                        if (_p->requestCV.wait_for(
                            lock,
                            std::chrono::milliseconds(timeout),
                            [this]
                        {
                            return _p->infoQueue.size() || _p->imageQueue.size();
                        }))
                        {
                            _p->newInfoRequests = std::move(_p->infoQueue);
                            _p->newImageRequests = std::move(_p->imageQueue);
                        }
                    }
                    _handleInfoRequests();
                    _handleImageRequests();
                }
            });
        }

        IconSystem::IconSystem() :
            _p(new Private)
        {}

        IconSystem::~IconSystem()
        {}

        std::shared_ptr<IconSystem> IconSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<IconSystem>(new IconSystem);
            out->_init(context);
            return out;
        }

        std::future<Pixel::Info> IconSystem::getInfo(const Path& path)
        {
            InfoRequest request;
            request.path = path;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(_p->requestMutex);
            _p->infoQueue.push_back(std::move(request));
            _p->requestCV.notify_one();
            return future;
        }

        std::future<std::shared_ptr<Image> > IconSystem::getImage(const Path& path)
        {
            ImageRequest request;
            request.path = path;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(_p->requestMutex);
            _p->imageQueue.push_back(std::move(request));
            _p->requestCV.notify_one();
            return future;
        }

        void IconSystem::_exit()
        {
            ISystem::_exit();
            {
                std::unique_lock<std::mutex> lock(_p->requestMutex);
                _p->infoQueue.clear();
                _p->imageQueue.clear();
            }
            _p->running = false;
            _p->thread.join();
        }

        void IconSystem::_handleInfoRequests()
        {
            if (auto context = getContext().lock())
            {
                // Process new requests.
                for (auto & i : _p->newInfoRequests)
                {
                    Pixel::Info info;
                    bool cached = false;
                    {
                        std::lock_guard<std::mutex> lock(_p->cacheMutex);
                        if (_p->infoCache.contains(i.path))
                        {
                            info = _p->infoCache.get(i.path);
                            cached = true;
                        }
                    }
                    if (cached)
                    {
                        i.promise.set_value(info);
                    }
                    else if (auto io = context->getSystemT<IO::System>())
                    {
                        try
                        {
                            i.read = io->read(i.path, nullptr);
                            i.infoFuture = i.read->getInfo();
                            _p->pendingInfoRequests.push_back(std::move(i));
                        }
                        catch (const std::exception& e)
                        {
                            //! \todo Error handling?
                            _log(e.what());
                        }
                    }
                }
                _p->newInfoRequests.clear();

                // Process pending requests.
                auto i = _p->pendingInfoRequests.begin();
                while (i != _p->pendingInfoRequests.end())
                {
                    if (i->infoFuture.valid() &&
                        i->infoFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        Pixel::Info info;
                        auto ioInfo = i->infoFuture.get();
                        auto videoInfo = ioInfo.getVideo();
                        if (videoInfo.size())
                        {
                            info = videoInfo[0].getInfo();
                        }
                        _p->infoCache.add(i->path, info);
                        i->promise.set_value(info);
                        i = _p->pendingInfoRequests.erase(i);
                    }
                    else
                    {
                        ++i;
                    }
                }
            }
        }

        void IconSystem::_handleImageRequests()
        {
            if (auto context = getContext().lock())
            {
                // Process new requests.
                for (auto & i : _p->newImageRequests)
                {
                    std::shared_ptr<Image> image;
                    bool cached = false;
                    {
                        std::lock_guard<std::mutex> lock(_p->cacheMutex);
                        if (_p->imageCache.contains(i.path))
                        {
                            image = _p->imageCache.get(i.path);
                        }
                    }
                    if (image)
                    {
                        i.promise.set_value(image);
                    }
                    else if (auto io = context->getSystemT<IO::System>())
                    {
                        try
                        {
                            i.queue = IO::ReadQueue::create();
                            i.read = io->read(i.path, i.queue);
                            _p->pendingImageRequests.push_back(std::move(i));
                        }
                        catch (const std::exception& e)
                        {
                            //! \todo Error handling?
                            _log(e.what());
                        }
                    }
                }
                _p->newImageRequests.clear();

                // Process pending requests.
                auto i = _p->pendingImageRequests.begin();
                while (i != _p->pendingImageRequests.end())
                {
                    std::shared_ptr<Image> image;
                    {
                        std::lock_guard<std::mutex> lock(i->queue->getMutex());
                        if (i->queue->hasVideoFrames())
                        {
                            image = i->queue->getVideoFrame().second;
                        }
                    }
                    if (image)
                    {
                        _p->imageCache.add(i->path, image);
                        i->promise.set_value(image);
                        i = _p->pendingImageRequests.erase(i);
                    }
                    else
                    {
                        ++i;
                    }
                }
            }
        }

    } // namespace AV
} // namespace djv
