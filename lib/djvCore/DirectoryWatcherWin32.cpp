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

#include <djvCore/DirectoryWatcher.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/Timer.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace djv
{
    namespace Core
    {
        namespace
        {
            //! \todo [1.0 S] Should this be configurable?
            const size_t timeout = 100;

        } // namespace

        struct DirectoryWatcher::Private
        {
            Path path;
            bool changed = false;
            std::condition_variable changedCV;
            std::mutex mutex;
            std::thread thread;
            std::atomic<bool> running = true;
            std::function<void(void)> callback;
            std::shared_ptr<Timer> timer;
        };

        void DirectoryWatcher::_init(const std::shared_ptr<Context>& context)
        {
            std::weak_ptr<Context> weakContext = context;
            _p->thread = std::thread(
                [this, weakContext]
            {
                DJV_PRIVATE_PTR();
                Path path;
                HANDLE changeHandle = 0;
                while (p.running)
                {
                    bool pathChanged = false;
                    {
                        std::lock_guard<std::mutex> lock(p.mutex);
                        if (p.path != path)
                        {
                            path = p.path;
                            pathChanged = true;
                        }
                    }

                    if (pathChanged)
                    {
                        if (changeHandle)
                        {
                            FindCloseChangeNotification(changeHandle);
                        }
                        changeHandle = FindFirstChangeNotification(
                            path.get().c_str(),
                            FALSE,
                            FILE_NOTIFY_CHANGE_FILE_NAME |
                            FILE_NOTIFY_CHANGE_DIR_NAME |
                            FILE_NOTIFY_CHANGE_SIZE |
                            FILE_NOTIFY_CHANGE_LAST_WRITE);
                        if (INVALID_HANDLE_VALUE == changeHandle)
                        {
                            if (auto context = weakContext.lock())
                            {
                                std::stringstream s;
                                s << "Error finding change notification for: " << path << ": " << getLastError();
                                context->log("djv::Core::DirectoryWatcher", s.str(), LogLevel::Error);
                            }
                        }
                    }

                    if (changeHandle && changeHandle != INVALID_HANDLE_VALUE)
                    {
                        const DWORD status = WaitForSingleObject(changeHandle, 0);
                        switch (status)
                        {
                        case WAIT_OBJECT_0:
                            FindNextChangeNotification(changeHandle);
                            {
                                std::lock_guard<std::mutex> lock(p.mutex);
                                p.changed = true;
                            }
                            break;
                        }
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                }

                if (changeHandle)
                {
                    FindCloseChangeNotification(changeHandle);
                }
            });

            _p->timer = Timer::create(context);
            _p->timer->setRepeating(true);
            _p->timer->start(
                std::chrono::milliseconds(timeout),
                [this](float)
            {
                DJV_PRIVATE_PTR();
                bool changed = false;
                {
                    std::unique_lock<std::mutex> lock(p.mutex);
                    if (p.changedCV.wait_for(
                        lock,
                        std::chrono::milliseconds(0),
                        [this]
                    {
                        return p.changed;
                    }))
                    {
                        changed = true;
                        p.changed = false;
                    }
                }
                if (changed && p.callback)
                {
                    p.callback();
                }
            });
        }

        DirectoryWatcher::DirectoryWatcher() :
            _p(new Private)
        {}

        DirectoryWatcher::~DirectoryWatcher()
        {
            _p->running = false;
            _p->thread.join();
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
            DJV_PRIVATE_PTR();
            if (value == p.path)
                return;
            {
                std::lock_guard<std::mutex> lock(p.mutex);
                p.path = value;
                p.changed = true;
            }
            if (p.callback)
            {
                p.callback();
            }
        }

        void DirectoryWatcher::setCallback(const std::function<void(void)>& value)
        {
            _p->callback = value;
        }

    } // namespace Core
} // namespace djv
