//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/ThumbnailSystem.h>

#include <djvAV/Image.h>
#include <djvAV/ImageConvert.h>
#include <djvAV/IO.h>

#include <djvCore/Cache.h>
#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>
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
            const size_t infoProcessMax  = 8;
            const size_t imageProcessMax = 8;
            const size_t infoCacheMax    = 1000;
            const size_t imageCacheMax   = 1000;

            struct InfoRequest
            {
                InfoRequest() :
                    uid(createUID())
                {}

                InfoRequest(InfoRequest && other) :
                    uid(other.uid),
                    path(other.path),
                    read(std::move(other.read)),
                    infoFuture(std::move(other.infoFuture)),
                    promise(std::move(other.promise))
                {}

                InfoRequest & operator = (InfoRequest && other)
                {
                    if (this != &other)
                    {
                        uid = other.uid;
                        path = other.path;
                        read = std::move(other.read);
                        infoFuture = std::move(other.infoFuture);
                        promise = std::move(other.promise);
                    }
                    return *this;
                }

                UID uid = 0;
                FileSystem::Path path;
                std::shared_ptr<IO::IRead> read;
                std::future<IO::Info> infoFuture;
                std::promise<IO::Info> promise;
            };

            struct ImageRequest
            {
                ImageRequest() :
                    uid(createUID())
                {}

                ImageRequest(ImageRequest && other) :
                    uid(other.uid),
                    path(other.path),
                    size(std::move(other.size)),
                    type(std::move(other.type)),
                    queue(std::move(other.queue)),
                    read(std::move(other.read)),
                    promise(std::move(other.promise))
                {}

                ImageRequest & operator = (ImageRequest && other)
                {
                    if (this != &other)
                    {
                        uid = other.uid;
                        path = other.path;
                        size = std::move(other.size);
                        type = std::move(other.type);
                        queue = std::move(other.queue);
                        read = std::move(other.read);
                        promise = std::move(other.promise);
                    }
                    return *this;
                }

                UID uid = 0;
                FileSystem::Path path;
                glm::ivec2 size = glm::ivec2(0, 0);
                Image::Type type = Image::Type::None;
                std::shared_ptr<IO::Queue> queue;
                std::shared_ptr<IO::IRead> read;
                std::promise<std::shared_ptr<Image::Image> > promise;
            };

            size_t getInfoCacheKey(const FileSystem::Path & path)
            {
                size_t out = 0;
                Memory::hashCombine(out, path.get());
                return out;
            }

            size_t getImageCacheKey(const FileSystem::Path & path, const glm::ivec2 & size, Image::Type type)
            {
                size_t out = 0;
                Memory::hashCombine(out, path.get());
                Memory::hashCombine(out, size);
                Memory::hashCombine(out, type);
                return out;
            }

        } // namespace

        ThumbnailSystem::InfoFuture::InfoFuture()
        {}
        
        ThumbnailSystem::InfoFuture::InfoFuture(std::future<IO::Info> & future, UID uid) :
            future(std::move(future)),
            uid(uid)
        {}

        ThumbnailSystem::ImageFuture::ImageFuture()
        {}
        
        ThumbnailSystem::ImageFuture::ImageFuture(std::future<std::shared_ptr<Image::Image> > & future, UID uid) :
            future(std::move(future)),
            uid(uid)
        {}
        
        struct ThumbnailSystem::Private
        {
            std::list<InfoRequest> infoRequests;
            std::list<ImageRequest> imageRequests;
            std::condition_variable requestCV;
            std::mutex requestMutex;
            std::list<InfoRequest> pendingInfoRequests;
            std::list<ImageRequest> pendingImageRequests;

            Memory::Cache<size_t, IO::Info> infoCache;
            Memory::Cache<size_t, std::shared_ptr<Image::Image> > imageCache;
            std::mutex cacheMutex;

            GLFWwindow * glfwWindow = nullptr;
            std::shared_ptr<Time::Timer> statsTimer;
            std::thread thread;
            std::atomic<bool> running;
        };

        void ThumbnailSystem::_init(Context * context)
        {
            ISystem::_init("djv::AV::ThumbnailSystem", context);

            DJV_PRIVATE_PTR();

            addDependency(context->getSystemT<IO::System>());

            p.infoCache.setMax(infoCacheMax);
            p.imageCache.setMax(imageCacheMax);

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            if (OS::getIntEnv("DJV_OPENGL_DEBUG") != 0)
            {
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
            }
            p.glfwWindow = glfwCreateWindow(100, 100, context->getName().c_str(), NULL, NULL);
            if (!p.glfwWindow)
            {
                std::stringstream ss;
                ss << DJV_TEXT("The GLFW window cannot be created.");
                throw std::runtime_error(ss.str());
            }

            p.statsTimer = Time::Timer::create(context);
            p.statsTimer->setRepeating(true);
            p.statsTimer->start(
                Time::getMilliseconds(Time::TimerValue::VerySlow),
                [this](float)
            {
                DJV_PRIVATE_PTR();
                std::stringstream s;
                {
                    std::lock_guard<std::mutex> lock(p.cacheMutex);
                    s << "Info cache: " << p.infoCache.getPercentageUsed() << "%\n";
                    s << "Image cache: " << p.imageCache.getPercentageUsed() << '%';
                }
                _log(s.str());
            });

            p.running = true;
            p.thread = std::thread(
                [this, context]
            {
                DJV_PRIVATE_PTR();
                try
                {
                    glfwMakeContextCurrent(p.glfwWindow);
                    glbinding::initialize(glfwGetProcAddress);

                    auto convert = Image::Convert::create(context);

                    const auto timeout = Time::getValue(Time::TimerValue::Medium);
                    while (p.running)
                    {
                        bool infoRequests  = p.pendingInfoRequests.size();
                        bool imageRequests = p.pendingImageRequests.size();
                        {
                            std::unique_lock<std::mutex> lock(p.requestMutex);
                            if (p.requestCV.wait_for(
                                lock,
                                std::chrono::milliseconds(timeout),
                                [this]
                            {
                                DJV_PRIVATE_PTR();
                                return p.infoRequests.size() || p.imageRequests.size();
                            }))
                            {
                                infoRequests  |= p.infoRequests.size () > 0;
                                imageRequests |= p.imageRequests.size() > 0;
                            }
                        }
                        if (infoRequests)
                        {
                            _handleInfoRequests();
                        }
                        if (imageRequests)
                        {
                            _handleImageRequests(convert);
                        }
                    }
                }
                catch (const std::exception & e)
                {
                    if (auto logSystem = context->getSystemT<LogSystem>())
                    {
                        logSystem->log("djv::AV::ThumbnailSystem", e.what(), LogLevel::Error);
                    }
                }
            });
        }

        ThumbnailSystem::ThumbnailSystem() :
            _p(new Private)
        {}

        ThumbnailSystem::~ThumbnailSystem()
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

        std::shared_ptr<ThumbnailSystem> ThumbnailSystem::create(Context * context)
        {
            auto out = std::shared_ptr<ThumbnailSystem>(new ThumbnailSystem);
            out->_init(context);
            return out;
        }

        ThumbnailSystem::InfoFuture ThumbnailSystem::getInfo(const FileSystem::Path & path)
        {
            DJV_PRIVATE_PTR();
            InfoRequest request;
            request.path = path;
            auto future = request.promise.get_future();
            {
                std::unique_lock<std::mutex> lock(p.requestMutex);
                p.infoRequests.push_back(std::move(request));
            }
            p.requestCV.notify_one();
            return InfoFuture(future, request.uid);
        }
        
        void ThumbnailSystem::cancelInfo(UID uid)
        {
            DJV_PRIVATE_PTR();
            {
                std::unique_lock<std::mutex> lock(p.requestMutex);
                const auto i = std::find_if(
                    p.infoRequests.rbegin(),
                    p.infoRequests.rend(),
                    [uid](const InfoRequest & value)
                {
                    return value.uid == uid;
                });
                if (i != p.infoRequests.rend())
                {
                    p.infoRequests.erase(--(i.base()));
                }
            }
        }

        ThumbnailSystem::ImageFuture ThumbnailSystem::getImage(
            const FileSystem::Path & path,
            const glm::ivec2 &       size,
            Image::Type              type)
        {
            DJV_PRIVATE_PTR();
            ImageRequest request;
            request.path = path;
            request.size = size;
            request.type = type;
            auto future = request.promise.get_future();
            {
                std::unique_lock<std::mutex> lock(p.requestMutex);
                p.imageRequests.push_back(std::move(request));
            }
            p.requestCV.notify_one();
            return ImageFuture(future, request.uid);
        }
        
        void ThumbnailSystem::cancelImage(UID uid)
        {
            DJV_PRIVATE_PTR();
            {
                std::unique_lock<std::mutex> lock(p.requestMutex);
                const auto i = std::find_if(
                    p.imageRequests.rbegin(),
                    p.imageRequests.rend(),
                    [uid](const ImageRequest & value)
                {
                    return value.uid == uid;
                });
                if (i != p.imageRequests.rend())
                {
                    p.imageRequests.erase(--(i.base()));
                }
            }
        }

        void ThumbnailSystem::_handleInfoRequests()
        {
            DJV_PRIVATE_PTR();

            // Process new requests.
            while (p.pendingInfoRequests.size() < infoProcessMax)
            {
                InfoRequest i;
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    if (p.infoRequests.size())
                    {
                        i = std::move(p.infoRequests.front());
                        p.infoRequests.pop_front();
                    }
                    else
                    {
                        break;
                    }
                }
                IO::Info info;
                bool cached = false;
                {
                    std::lock_guard<std::mutex> lock(p.cacheMutex);
                    const auto key = getInfoCacheKey(i.path);
                    cached = p.infoCache.get(key, info);
                }
                if (cached)
                {
                    i.promise.set_value(info);
                }
                else if (auto io = getContext()->getSystemT<IO::System>())
                {
                    try
                    {
                        i.read = io->read(i.path, nullptr);
                        i.infoFuture = i.read->getInfo();
                        p.pendingInfoRequests.push_back(std::move(i));
                    }
                    catch (const std::exception & e)
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

            // Process pending requests.
            auto i = p.pendingInfoRequests.begin();
            while (i != p.pendingInfoRequests.end())
            {
                if (i->infoFuture.valid() &&
                    i->infoFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        const auto info = i->infoFuture.get();
                        p.infoCache.add(getInfoCacheKey(i->path), info);
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

        void ThumbnailSystem::_handleImageRequests(const std::shared_ptr<Image::Convert> & convert)
        {
            DJV_PRIVATE_PTR();

            // Process new requests.
            while (p.pendingImageRequests.size() < imageProcessMax)
            {
                ImageRequest i;
                {
                    std::unique_lock<std::mutex> lock(p.requestMutex);
                    if (p.imageRequests.size())
                    {
                        i = std::move(p.imageRequests.front());
                        p.imageRequests.pop_front();
                    }
                    else
                    {
                        break;
                    }
                }
                std::shared_ptr<Image::Image> image;
                {
                    std::lock_guard<std::mutex> lock(p.cacheMutex);
                    const auto key = getImageCacheKey(i.path, i.size, i.type);
                    p.imageCache.get(key, image);
                }
                if (image)
                {
                    i.promise.set_value(image);
                }
                else
                {
                    if (auto io = getContext()->getSystemT<IO::System>())
                    {
                        try
                        {
                            i.queue = IO::Queue::create(1, 0);
                            i.read = io->read(i.path, i.queue);
                            const auto info = i.read->getInfo().get();
                            if (info.video.size() > 0)
                            {
                                p.pendingImageRequests.push_back(std::move(i));
                            }
                            else
                            {
                                i.promise.set_value(nullptr);
                            }
                        }
                        catch (const std::exception & e)
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
            }

            // Process pending requests.
            auto i = p.pendingImageRequests.begin();
            while (i != p.pendingImageRequests.end())
            {
                std::shared_ptr<Image::Image> image;
                {
                    std::lock_guard<std::mutex> lock(i->queue->getMutex());
                    if (i->queue->hasVideo())
                    {
                        image = i->queue->getVideo().second;
                    }
                }
                if (image)
                {
                    if (i->size != image->getSize() || i->type != Image::Type::None)
                    {
                        auto size = i->size;
                        const float aspect = image->getAspectRatio();
                        if (0 == size.x)
                        {
                            size.x = static_cast<int>(size.y * aspect);
                        }
                        else if (0 == size.y && aspect > 0.f)
                        {
                            size.y = static_cast<int>(size.x / aspect);
                        }
                        const auto type = i->type != Image::Type::None ? i->type : image->getType();
                        const auto info = Image::Info(size, type);
                        auto tmp = Image::Image::create(info);
                        tmp->setTags(image->getTags());
                        convert->process(*image, info, *tmp);
                        image = tmp;
                    }
                    p.imageCache.add(getImageCacheKey(i->path, i->size, i->type), image);
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
