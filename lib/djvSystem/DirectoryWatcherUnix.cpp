// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/DirectoryWatcher.h>

#include <djvSystem/Context.h>
#include <djvSystem/Path.h>
#include <djvSystem/Timer.h>

#include <ctime>
#include <mutex>
#include <thread>

#if defined(DJV_PLATFORM_MACOS)
#include <CoreServices/CoreServices.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#else
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>
#endif

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace File
        {
            namespace
            {
#if defined(DJV_PLATFORM_MACOS)
                class Notify
                {
                public:
                    Notify(const Path& path) :
                        _path(path)
                    {
                        _kq = ::kqueue();
                        if  (_kq)
                        {
                            _fd = ::open(_path.get().c_str(), O_EVTONLY);
                            
                            const int vnodeEvents =
                                NOTE_DELETE |
                                NOTE_WRITE  |
                                NOTE_EXTEND |
                                NOTE_ATTRIB |
                                NOTE_LINK   |
                                NOTE_RENAME |
                                NOTE_REVOKE;
                            EV_SET(&_eventsToMonitor[0], _fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, vnodeEvents, 0, 0);
                        }
                    }
                    
                    ~Notify()
                    {
                        if (_fd)
                        {
                            ::close(_fd);
                        }
                    }
                                        
                    ::time_t poll()
                    {
                        struct kevent eventData[1];
                        timespec _timeout;
                        _timeout.tv_sec = 0;
                        _timeout.tv_nsec = getTimerValue(TimerValue::Medium) * 1000000;
                        int eventCount = ::kevent(_kq, _eventsToMonitor, 1, eventData, 1, &_timeout);
                        if (eventCount)
                        {
                            _lastModified = time(nullptr);
                        }
                        return _lastModified;
                    }
                    
                private:
                    Path _path;
                    int _kq = 0;
                    int _fd = 0;
                    struct kevent _eventsToMonitor[1];
                    ::time_t _lastModified = 0;
                };

#else // DJV_PLATFORM_MACOS

                class Notify
                {
                public:
                    explicit Notify(const Path& path) :
                        _path(path)
                    {
                        _fd = ::inotify_init1(IN_NONBLOCK);
                        if (_fd)
                        {
                            _wd = ::inotify_add_watch(_fd, _path.get().c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_ATTRIB);
                        }
                    }

                    Notify(Notify&& other) noexcept :
                        _path(other._path),
                        _fd(other._fd),
                        _wd(other._wd),
                        _lastModified(other._lastModified)
                    {}
                    
                    ~Notify()
                    {
                        if (_fd && _wd)
                        {
                            ::inotify_rm_watch(_fd, _wd);
                            _wd = 0;
                        }
                        if (_fd)
                        {
                            ::close(_fd);
                            _fd = 0;
                        }
                    }

                    Notify& operator = (Notify&& other) noexcept
                    {
                        if (this != &other)
                        {
                            _path = other._path;
                            _fd = other._fd;
                            _wd = other._wd;
                            _lastModified = other._lastModified;
                        }
                        return *this;
                    }
                                        
                    ::time_t poll()
                    {
                        if (_fd && _wd)
                        {
                            static const size_t bufferSize = 1024 * (sizeof(::inotify_event) + 16);
                            char buffer[bufferSize];
                            const int length = ::read(_fd, buffer, bufferSize);
                            int i = 0;
                            while (i < length)
                            {
                                ::inotify_event* event = (::inotify_event*)&buffer[i];
                                if (event->len)
                                {
                                    if (event->mask & IN_CREATE ||
                                        event->mask & IN_DELETE ||
                                        event->mask & IN_MODIFY)
                                    {
                                        _lastModified = time(nullptr);
                                        break;
                                    }
                                }
                                i += sizeof(::inotify_event) + event->len;
                            }
                        }
                        return _lastModified;
                    }
                    
                private:
                    Path _path;
                    int _fd = 0;
                    int _wd = 0;
                    ::time_t _lastModified = 0;
                };
#endif // DJV_PLATFORM_MACOS

            } // namespace
                    
            struct DirectoryWatcher::Private
            {
                Path path;
                bool running = false;
                std::thread thread;
                std::timed_mutex mutex;
                ::time_t lastModified = 0;
                ::time_t threadModified = 0;
                std::shared_ptr<Timer> timer;
                std::function<void(void)> callback;
            };

            void DirectoryWatcher::_init(const std::shared_ptr<Context>& context)
            {
                _p->running = true;
                
                std::weak_ptr<DirectoryWatcher> weak(shared_from_this());
                const auto timeout = getTimerDuration(TimerValue::Medium);
                _p->thread = std::thread(
                    [weak, timeout]
                {
                    Path path;
                    bool pathInit = false;
                    std::unique_ptr<Notify> notify;
                    ::time_t lastModified = 0;                    
                    bool running = true;
                    while (running)
                    {
                        if (auto watcher = weak.lock())
                        {
                            auto & p = *watcher->_p;
                            if (p.mutex.try_lock_for(timeout))
                            {
                                // Synchronize with the main thread.
                                running = p.running;
                                if (path != p.path)
                                {
                                    path = p.path;
                                    pathInit = true;
                                }
                                p.threadModified = lastModified;                                                        
                                p.mutex.unlock();
                            }
                        }
                        else
                        {
                            running = false;
                            break;
                        }

                        if (pathInit)
                        {
                            // Start the notifier with a new path.
                            notify.reset(new Notify(path));
                            pathInit = false;
                        }
                        
                        if (notify)
                        {
                            // Poll for events.
                            lastModified = notify->poll();
                        }
                        
                        std::this_thread::sleep_for(timeout);
                    }
                });

                _p->timer = Timer::create(context);
                _p->timer->setRepeating(true);
                _p->timer->start(
                    timeout,
                    [weak, timeout](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                {
                    if (auto watcher = weak.lock())
                    {
                        auto & p = *watcher->_p;
                        if (p.mutex.try_lock_for(timeout))
                        {
                            if (p.threadModified != p.lastModified)
                            {                            
                                p.lastModified = p.threadModified;
                                
                                if (p.callback)
                                {
                                    p.callback();
                                }
                            }
                            p.mutex.unlock();
                        }
                    }
                });
            }
            
            DirectoryWatcher::DirectoryWatcher() :
                _p(new Private)
            {}

            DirectoryWatcher::~DirectoryWatcher()
            {
                if (_p->running)
                {
                    _p->running = false;
                    _p->thread.join();
                }
            }
            
            std::shared_ptr<DirectoryWatcher> DirectoryWatcher::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<DirectoryWatcher>(new DirectoryWatcher);
                out->_init(context);
                return out;
            }

            const Path& DirectoryWatcher::getPath() const
            {
                return _p->path;
            }

            void DirectoryWatcher::setPath(const Path& value)
            {
                _p->path = value;
            }

            void DirectoryWatcher::setCallback(const std::function<void(void)>& value)
            {
                _p->callback = value;
            }

        } // namespace File
    } // namespace System
} // namespace djv


