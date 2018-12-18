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
#include <djvAV/PixelProcess.h>

#include <djvCore/Cache.h>
#include <djvCore/Context.h>
#include <djvCore/OS.h>
#include <djvCore/Timer.h>

#include <GLFW/glfw3.h>

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

            struct InfoRequest
            {
                InfoRequest()
                {}

                InfoRequest(InfoRequest&& other) :
                    path(other.path),
                    read(std::move(other.read)),
                    infoFuture(std::move(other.infoFuture)),
                    promise(std::move(other.promise))
                {}

                InfoRequest& operator = (InfoRequest&& other)
                {
                    if (this != &other)
                    {
                        path = other.path;
                        read = std::move(other.read);
                        infoFuture = std::move(other.infoFuture);
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
                    info(std::move(other.info)),
                    queue(std::move(other.queue)),
                    read(std::move(other.read)),
                    promise(std::move(other.promise))
                {}

                ImageRequest& operator = (ImageRequest&& other)
                {
                    if (this != &other)
                    {
                        path = other.path;
                        info = std::move(other.info);
                        queue = std::move(other.queue);
                        read = std::move(other.read);
                        promise = std::move(other.promise);
                    }
                    return *this;
                }

                Path path;
                std::unique_ptr<Pixel::Info> info;
                std::shared_ptr<IO::Queue> queue;
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

            GLFWwindow * glfwWindow = nullptr;
            std::shared_ptr<Timer> statsTimer;
            std::thread thread;
            std::atomic<bool> running;
        };

        void IconSystem::_init(Context * context)
        {
            ISystem::_init("djv::AV::IconSystem", context);

            DJV_PRIVATE_PTR();
            p.infoCache.setMax(infoCacheMax);
            p.imageCache.setMax(imageCacheMax);

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            if (!OS::getEnv("DJV_OPENGL_DEBUG").empty())
            {
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
            }
            p.glfwWindow = glfwCreateWindow(100, 100, context->getName().c_str(), NULL, NULL);
            if (!p.glfwWindow)
            {
                std::stringstream ss;
                ss << DJV_TEXT("Cannot create GLFW window.");
                throw std::runtime_error(ss.str());
            }

            p.statsTimer = Timer::create(context);
            p.statsTimer->setRepeating(true);
            p.statsTimer->start(
                Timer::getMilliseconds(Timer::Value::VerySlow),
                [this](float)
            {
                DJV_PRIVATE_PTR();
                std::lock_guard<std::mutex> lock(p.cacheMutex);
                std::stringstream s;
                s << "Info cache: " << p.infoCache.getPercentageUsed() << "%\n";
                s << "Image cache: " << p.imageCache.getPercentageUsed() << '%';
                _log(s.str());
            });

            p.running = true;
            p.thread = std::thread(
                [this, context]
            {
                DJV_PRIVATE_PTR();
                GLFWwindow * glfwWindow = nullptr;
                try
                {
                    glfwMakeContextCurrent(p.glfwWindow);
                    glbinding::initialize(glfwGetProcAddress);

                    auto convert = Pixel::Convert::create(context);

                    const auto timeout = Timer::getValue(Timer::Value::Medium);
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
                                return p.infoQueue.size() || p.imageQueue.size();
                            }))
                            {
                                p.newInfoRequests = std::move(p.infoQueue);
                                p.newImageRequests = std::move(p.imageQueue);
                            }
                        }
                        if (p.newInfoRequests.size() || p.pendingInfoRequests.size())
                        {
                            _handleInfoRequests();
                        }
                        if (p.newImageRequests.size() || p.pendingImageRequests.size())
                        {
                            _handleImageRequests(convert);
                        }
                    }
                }
                catch (const std::exception & e)
                {
                    context->log("djv::AV::IconSystem", e.what(), LogLevel::Error);
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
            if (p.glfwWindow)
            {
                glfwDestroyWindow(p.glfwWindow);
            }
        }

        std::shared_ptr<IconSystem> IconSystem::create(Context * context)
        {
            auto out = std::shared_ptr<IconSystem>(new IconSystem);
            out->_init(context);
            return out;
        }

        std::future<Pixel::Info> IconSystem::getInfo(const Path& path)
        {
            DJV_PRIVATE_PTR();
            InfoRequest request;
            request.path = path;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(p.requestMutex);
            p.infoQueue.push_back(std::move(request));
            p.requestCV.notify_one();
            return future;
        }

        std::future<std::shared_ptr<Image> > IconSystem::getImage(const Path& path)
        {
            DJV_PRIVATE_PTR();
            ImageRequest request;
            request.path = path;
            auto future = request.promise.get_future();
            {
                std::unique_lock<std::mutex> lock(p.requestMutex);
                p.imageQueue.push_back(std::move(request));
            }
            p.requestCV.notify_one();
            return future;
        }

        std::future<std::shared_ptr<Image> > IconSystem::getImage(const Path& path, const Pixel::Info& info)
        {
            DJV_PRIVATE_PTR();
            ImageRequest request;
            request.path = path;
            request.info.reset(new Pixel::Info(info));
            auto future = request.promise.get_future();
            {
                std::unique_lock<std::mutex> lock(p.requestMutex);
                p.imageQueue.push_back(std::move(request));
            }
            p.requestCV.notify_one();
            return future;
        }

        void IconSystem::_handleInfoRequests()
        {
            DJV_PRIVATE_PTR();

            // Process new requests.
            for (auto & i : p.newInfoRequests)
            {
                Pixel::Info info;
                bool cached = false;
                {
                    std::lock_guard<std::mutex> lock(p.cacheMutex);
                    if (p.infoCache.contains(i.path))
                    {
                        info = p.infoCache.get(i.path);
                        cached = true;
                    }
                }
                if (cached)
                {
                    i.promise.set_value(info);
                }
                else if (auto io = getContext()->getSystemT<IO::System>().lock())
                {
                    try
                    {
                        i.read = io->read(i.path, nullptr);
                        i.infoFuture = i.read->getInfo();
                        p.pendingInfoRequests.push_back(std::move(i));
                    }
                    catch (const std::exception& e)
                    {
                        try
                        {
                            i.promise.set_exception(std::current_exception());
                        }
                        catch (const std::exception & e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                        _log(e.what(), LogLevel::Error);
                    }
                }
            }
            p.newInfoRequests.clear();

            // Process pending requests.
            auto i = p.pendingInfoRequests.begin();
            while (i != p.pendingInfoRequests.end())
            {
                if (i->infoFuture.valid() &&
                    i->infoFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        Pixel::Info info;
                        const auto ioInfo = i->infoFuture.get();
                        const auto & videoInfo = ioInfo.video;
                        if (videoInfo.size())
                        {
                            info = videoInfo[0].info;
                        }
                        p.infoCache.add(i->path, info);
                        i->promise.set_value(info);
                    }
                    catch (const std::exception & e)
                    {
                        try
                        {
                            i->promise.set_exception(std::current_exception());
                        }
                        catch (const std::exception & e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                        _log(e.what(), LogLevel::Error);
                    }
                    i = p.pendingInfoRequests.erase(i);
                }
                else
                {
                    ++i;
                }
            }
        }

        void IconSystem::_handleImageRequests(const std::shared_ptr<Pixel::Convert> & convert)
        {
            DJV_PRIVATE_PTR();
            // Process new requests.
            for (auto & i : p.newImageRequests)
            {
                std::shared_ptr<Image> image;
                bool cached = false;
                {
                    std::lock_guard<std::mutex> lock(p.cacheMutex);
                    if (p.imageCache.contains(i.path))
                    {
                        image = p.imageCache.get(i.path);
                    }
                }
                if (image)
                {
                    i.promise.set_value(image);
                }
                else if (auto io = getContext()->getSystemT<IO::System>().lock())
                {
                    try
                    {
                        i.queue = IO::Queue::create(1, 0);
                        i.read = io->read(i.path, i.queue);
                        p.pendingImageRequests.push_back(std::move(i));
                    }
                    catch (const std::exception& e)
                    {
                        try
                        {
                            i.promise.set_exception(std::current_exception());
                        }
                        catch (const std::exception & e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                        _log(e.what(), LogLevel::Error);
                    }
                }
            }
            p.newImageRequests.clear();

            // Process pending requests.
            auto i = p.pendingImageRequests.begin();
            while (i != p.pendingImageRequests.end())
            {
                std::shared_ptr<Image> image;
                {
                    std::lock_guard<std::mutex> lock(i->queue->getMutex());
                    if (i->queue->hasVideo())
                    {
                        image = i->queue->getVideo().second;
                    }
                }
                if (image)
                {
                    if (i->info)
                    {
                        auto tmp = Image::create(*i->info);
                        tmp->setTags(image->getTags());
                        convert->process(*image, *i->info, *tmp);
                        image = tmp;
                    }
                    p.imageCache.add(i->path, image);
                    i->promise.set_value(image);
                    i = p.pendingImageRequests.erase(i);
                }
                else
                {
                    ++i;
                }
            }
        }

    } // namespace AV
} // namespace djv
