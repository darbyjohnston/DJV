// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/ThumbnailSystem.h>

#include <djvAV/IOSystem.h>

#include <djvGL/ImageConvert.h>

#include <djvImage/Image.h>

#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/ResourceSystem.h>
#include <djvSystem/TextSystem.h>
#include <djvSystem/TimerFunc.h>

#include <djvCore/Cache.h>
#include <djvCore/OSFunc.h>
#include <djvCore/UIDFunc.h>

#define GLFW_INCLUDE_NONE
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
            //! \todo Should this be configurable?
            const size_t infoProcessMax  = 4;
            const size_t imageProcessMax = 4;
            const size_t infoCacheMax    = 1000;
            const size_t imageCacheMax   = 1000;

            struct InfoRequest
            {
                InfoRequest() :
                    uid(createUID())
                {}

                InfoRequest(InfoRequest&& other) noexcept :
                    uid(other.uid),
                    fileInfo(other.fileInfo),
                    read(std::move(other.read)),
                    infoFuture(std::move(other.infoFuture)),
                    promise(std::move(other.promise))
                {}

                ~InfoRequest()
                {}

                InfoRequest& operator = (InfoRequest&& other) noexcept
                {
                    if (this != &other)
                    {
                        uid = other.uid;
                        fileInfo = other.fileInfo;
                        read = std::move(other.read);
                        infoFuture = std::move(other.infoFuture);
                        promise = std::move(other.promise);
                    }
                    return *this;
                }

                UID uid = 0;
                System::File::Info fileInfo;
                std::shared_ptr<IO::IRead> read;
                std::future<IO::Info> infoFuture;
                std::promise<IO::Info> promise;
            };

            struct ImageRequest
            {
                ImageRequest() :
                    uid(createUID())
                {}

                ImageRequest(ImageRequest&& other) noexcept :
                    uid(other.uid),
                    fileInfo(other.fileInfo),
                    size(std::move(other.size)),
                    type(std::move(other.type)),
                    read(std::move(other.read)),
                    promise(std::move(other.promise))
                {}

                ~ImageRequest()
                {}

                ImageRequest& operator = (ImageRequest&& other) noexcept
                {
                    if (this != &other)
                    {
                        uid = other.uid;
                        fileInfo = other.fileInfo;
                        size = std::move(other.size);
                        type = std::move(other.type);
                        read = std::move(other.read);
                        promise = std::move(other.promise);
                    }
                    return *this;
                }

                UID uid = 0;
                System::File::Info fileInfo;
                Image::Size size;
                Image::Type type = Image::Type::None;
                std::shared_ptr<IO::IRead> read;
                std::promise<std::shared_ptr<Image::Image> > promise;
            };

            size_t getInfoCacheKey(const System::File::Info& fileInfo)
            {
                size_t out = 0;
                Memory::hashCombine(out, fileInfo.getFileName());
                return out;
            }

            size_t getImageCacheKey(const System::File::Info& fileInfo, const Image::Size& size, Image::Type type)
            {
                size_t out = 0;
                Memory::hashCombine(out, fileInfo.getFileName());
                Memory::hashCombine(out, size.w);
                Memory::hashCombine(out, size.h);
                Memory::hashCombine(out, type);
                return out;
            }

        } // namespace
        
        ThumbnailSystem::InfoFuture::InfoFuture()
        {}
        
        ThumbnailSystem::InfoFuture::InfoFuture(std::future<IO::Info>& future, UID uid) :
            future(std::move(future)),
            uid(uid)
        {}
        
        ThumbnailSystem::ImageFuture::ImageFuture()
        {}
        
        ThumbnailSystem::ImageFuture::ImageFuture(std::future<std::shared_ptr<Image::Image> >& future, UID uid) :
            future(std::move(future)),
            uid(uid)
        {}

        ThumbnailError::ThumbnailError(const std::string& what) :
            std::runtime_error(what)
        {}
        
        struct ThumbnailSystem::Private
        {
            std::shared_ptr<System::TextSystem> textSystem;
            std::shared_ptr<IO::IOSystem> io;

            std::list<InfoRequest> infoRequests;
            std::list<ImageRequest> imageRequests;
            std::condition_variable requestCV;
            std::mutex requestMutex;
            std::list<InfoRequest> pendingInfoRequests;
            std::list<ImageRequest> pendingImageRequests;

            Memory::Cache<size_t, IO::Info> infoCache;
            std::atomic<float> infoCachePercentage;
            Memory::Cache<size_t, std::shared_ptr<Image::Image> > imageCache;
            std::atomic<float> imageCachePercentage;
            std::atomic<bool> clearCache;
            std::shared_ptr<Observer::ValueObserver<bool> > ioOptionsObserver;

            GLFWwindow * glfwWindow = nullptr;
            std::shared_ptr<System::Timer> statsTimer;
            std::thread thread;
            std::atomic<bool> running;
        };

        void ThumbnailSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init("djv::AV::ThumbnailSystem", context);

            DJV_PRIVATE_PTR();

            p.textSystem = context->getSystemT<System::TextSystem>();
            p.io = context->getSystemT<IO::IOSystem>();
            addDependency(p.io);

            p.infoCache.setMax(infoCacheMax);
            p.infoCachePercentage = 0.F;
            p.imageCache.setMax(imageCacheMax);
            p.imageCachePercentage = 0.F;
            p.clearCache = false;

#if defined(DJV_GL_ES2)
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else // DJV_GL_ES2
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // DJV_GL_ES2
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            int env = 0;
            if (OS::getIntEnv("DJV_GL_DEBUG", env) && env != 0)
            {
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
            }
            p.glfwWindow = glfwCreateWindow(100, 100, context->getName().c_str(), NULL, NULL);
            if (!p.glfwWindow)
            {
                throw ThumbnailError(p.textSystem->getText(DJV_TEXT("error_glfw_window_creation")));
            }

            p.statsTimer = System::Timer::create(context);
            p.statsTimer->setRepeating(true);
            p.statsTimer->start(
                System::getTimerDuration(System::TimerValue::VerySlow),
                [this](const std::chrono::steady_clock::time_point&, const Time::Duration&)
            {
                DJV_PRIVATE_PTR();
                std::stringstream ss;
                {
                    ss << "Info cache: " << p.infoCachePercentage << "%\n";
                    ss << "Image cache: " << p.imageCachePercentage << '%';
                }
                _log(ss.str());
            });

            auto logSystem = context->getSystemT<System::LogSystem>();
            auto resourceSystem = context->getSystemT<System::ResourceSystem>();
            p.running = true;
            p.thread = std::thread(
                [this, resourceSystem, logSystem]
            {
                DJV_PRIVATE_PTR();
                try
                {
                    glfwMakeContextCurrent(p.glfwWindow);
#if defined(DJV_GL_ES2)
                    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
#else // DJV_GL_ES2
                    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
#endif // DJV_GL_ES2
                    {
                        throw ThumbnailError(p.textSystem->getText(DJV_TEXT("error_glad_init")));
                    }

                    auto convert = GL::ImageConvert::create(p.textSystem, resourceSystem);

                    const auto timeout = System::getTimerValue(System::TimerValue::Medium);
                    while (p.running)
                    {
                        if (p.clearCache)
                        {
                            p.clearCache = false;
                            p.infoCache.clear();
                            p.infoCachePercentage = 0.F;
                            p.imageCache.clear();
                            p.imageCachePercentage = 0.F;
                        }

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
                catch (const std::exception& e)
                {
                    logSystem->log("djv::AV::ThumbnailSystem", e.what(), System::LogLevel::Error);
                }
            });

            auto weak = std::weak_ptr<ThumbnailSystem>(std::dynamic_pointer_cast<ThumbnailSystem>(shared_from_this()));
            p.ioOptionsObserver = Observer::ValueObserver<bool>::create(
                p.io->observeOptionsChanged(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->clearCache();
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

        std::shared_ptr<ThumbnailSystem> ThumbnailSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<ThumbnailSystem>();
            if (!out)
            {
                out = std::shared_ptr<ThumbnailSystem>(new ThumbnailSystem);
                out->_init(context);
            }
            return out;
        }

        ThumbnailSystem::InfoFuture ThumbnailSystem::getInfo(const System::File::Info& fileInfo)
        {
            DJV_PRIVATE_PTR();
            InfoRequest request;
            request.fileInfo = fileInfo;
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
                    [uid](const InfoRequest& value)
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
            const System::File::Info& fileInfo,
            const Image::Size&        size,
            Image::Type               type)
        {
            DJV_PRIVATE_PTR();
            ImageRequest request;
            request.fileInfo = fileInfo;
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
                    [uid](const ImageRequest& value)
                {
                    return value.uid == uid;
                });
                if (i != p.imageRequests.rend())
                {
                    p.imageRequests.erase(--(i.base()));
                }
            }
        }

        float ThumbnailSystem::getInfoCachePercentage() const
        {
            return _p->infoCachePercentage;
        }

        float ThumbnailSystem::getImageCachePercentage() const
        {
            return _p->imageCachePercentage;
        }

        void ThumbnailSystem::clearCache()
        {
            _p->clearCache = true;
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
                const auto key = getInfoCacheKey(i.fileInfo);
                IO::Info info;
                const bool cached = p.infoCache.get(key, info);
                if (cached)
                {
                    i.promise.set_value(info);
                }
                else
                {
                    try
                    {
                        i.read = p.io->read(i.fileInfo);
                        i.infoFuture = i.read->getInfo();
                        p.pendingInfoRequests.push_back(std::move(i));
                    }
                    catch (const std::exception&)
                    {
                        try
                        {
                            i.promise.set_exception(std::current_exception());
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), System::LogLevel::Error);
                        }
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
                    p.infoCache.add(getInfoCacheKey(i->fileInfo), info);
                    p.infoCachePercentage = p.infoCache.getPercentageUsed();
                    i->promise.set_value(info);
                    }
                    catch (const std::exception&)
                    {
                        try
                        {
                            i->promise.set_exception(std::current_exception());
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), System::LogLevel::Error);
                        }
                    }
                    i = p.pendingInfoRequests.erase(i);
                }
                else
                {
                    ++i;
                }
            }
        }

        void ThumbnailSystem::_handleImageRequests(const std::shared_ptr<GL::ImageConvert>& convert)
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
                const auto key = getImageCacheKey(i.fileInfo, i.size, i.type);
                std::shared_ptr<Image::Image> image;
                p.imageCache.get(key, image);
                if (image)
                {
                    i.promise.set_value(image);
                }
                else
                {
                    try
                    {
                        i.read = p.io->read(i.fileInfo);
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
                    catch (const std::exception&)
                    {
                        try
                        {
                            i.promise.set_exception(std::current_exception());
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), System::LogLevel::Error);
                        }
                    }
                }
            }

            // Process pending requests.
            auto i = p.pendingImageRequests.begin();
            while (i != p.pendingImageRequests.end())
            {
                std::shared_ptr<Image::Image> image;
                bool finished = false;
                {
                    std::lock_guard<std::mutex> lock(i->read->getMutex());
                    auto& queue = i->read->getVideoQueue();
                    if (!queue.isEmpty())
                    {
                        image = queue.getFrame().image;
                    }
                    if (queue.isFinished())
                    {
                        finished = true;
                    }
                }
                if (image)
                {
                    try
                    {
                        Image::Size imageSize = image->getSize();
                        imageSize.w *= image->getInfo().pixelAspectRatio;
                        if (i->size != imageSize || i->type != Image::Type::None)
                        {
                            Image::Size size = i->size;
                            const float aspect = size.h != 0 ? (size.w / static_cast<float>(size.h)) : 1.F;
                            const float imageAspect = imageSize.h != 0 ? (imageSize.w / static_cast<float>(imageSize.h)) : 1.F;
                            if (imageAspect < aspect)
                            {
                                size.w = static_cast<uint16_t>(size.h * imageAspect);
                            }
                            else
                            {
                                size.h = static_cast<int>(size.w / imageAspect);
                            }
                            const auto type = i->type != Image::Type::None ? i->type : image->getType();
                            auto info = Image::Info(size, type);
#if defined(DJV_GL_ES2)
                            info.type = Image::Type::RGBA_U8;
#endif // DJV_GL_ES2
                            auto tmp = Image::Image::create(info);
                            tmp->setPluginName(image->getPluginName());
                            tmp->setTags(image->getTags());
                            convert->process(*image, info, *tmp);
                            image = tmp;
                        }
                        p.imageCache.add(getImageCacheKey(i->fileInfo, i->size, i->type), image);
                        p.imageCachePercentage = p.imageCache.getPercentageUsed();
                        i->promise.set_value(image);
                    }
                    catch (const std::exception&)
                    {
                        try
                        {
                            i->promise.set_exception(std::current_exception());
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), System::LogLevel::Error);
                        }
                    }
                }
                else if (finished)
                {
                    i->promise.set_value(nullptr);
                }
                if (image || finished)
                {
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
