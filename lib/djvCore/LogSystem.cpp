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

#include <djvCore/LogSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileIO.h>
#include <djvCore/OS.h>
#include <djvCore/Path.h>
#include <djvCore/Time.h>
#include <djvCore/Timer.h>

#include <atomic>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>

namespace djv
{
    namespace Core
    {
        namespace
        {
            const std::string name = "djv::Core::LogSystem";

            std::map<LogLevel, std::string> labels =
            {
                { LogLevel::Information, std::string() },
                { LogLevel::Warning, "[Warning]" },
                { LogLevel::Error, "[ERROR]" }
            };

            struct Message
            {
                Message()
                {}
                Message(
                    const std::string& prefix,
                    const std::string& text,
                    LogLevel level) :
                    prefix(prefix),
                    text(text),
                    level(level)
                {}
                std::string prefix;
                std::string text;
                LogLevel level = LogLevel::Information;
            };

        } // namespace

        struct LogSystem::Private
        {
            FileSystem::Path path;
            std::atomic<bool> consoleOutput;
            std::list<Message> queue;
            std::condition_variable queueCV;
            std::list<Message> messages;
            std::mutex mutex;
            std::thread thread;
            std::atomic<bool> running;
        };

        void LogSystem::_init(const FileSystem::Path& path, Context * context)
        {
            ISystem::_init(name, context);

            _p->path = path;

            _p->running = true;
            _p->thread = std::thread(
                [this]
            {
                DJV_PRIVATE_PTR();
                p.consoleOutput = OS::getIntEnv("DJV_LOG_CONSOLE") != 0;

                try
                {
                    FileSystem::FileIO io;
                    io.open(p.path, FileSystem::FileIO::Mode::Write);
                }
                catch (const std::exception &)
                {}

                std::stringstream s;
                s << "Path: " << p.path;
                log("djv::Core::LogSystem", s.str());

                const auto timeout = Time::Timer::getValue(Time::Timer::Value::Slow);
                while (p.running)
                {
                    {
                        std::unique_lock<std::mutex> lock(p.mutex);
                        p.queueCV.wait_for(
                            lock,
                            std::chrono::milliseconds(timeout),
                            [this]
                        {
                            return _p->queue.size();
                        });
                        p.messages = std::move(p.queue);
                    }
                    _writeMessages();
                }
                {
                    std::unique_lock<std::mutex> lock(p.mutex);
                    p.messages = std::move(p.queue);
                }
                _writeMessages();
            });

            _logSystemInit = true;
        }

        LogSystem::LogSystem() :
            _p(new Private)
        {}

        LogSystem::~LogSystem()
        {
            _logSystemInit = false;
            _p->running = false;
            if (_p->thread.joinable())
            {
                _p->thread.join();
            }
        }
        
        std::shared_ptr<LogSystem> LogSystem::create(const FileSystem::Path& logFile, Context * context)
        {
            auto out = std::shared_ptr<LogSystem>(new LogSystem);
            out->_init(logFile, context);
            return out;
        }

        void LogSystem::log(const std::string& prefix, const std::string& message, LogLevel level)
        {
            DJV_PRIVATE_PTR();
            {
                std::unique_lock<std::mutex> lock(p.mutex);
                p.queue.push_back(Message(prefix, message, level));
            }
            p.queueCV.notify_one();
        }

        bool LogSystem::hasConsoleOutput() const
        {
            return _p->consoleOutput;
        }

        void LogSystem::setConsoleOutput(bool value)
        {
            _p->consoleOutput = value;
        }

        void LogSystem::_writeMessages()
        {
            DJV_PRIVATE_PTR();
            try
            {
                FileSystem::FileIO io;
                io.open(p.path, FileSystem::FileIO::Mode::Append);
                io.seek(io.getSize());
                for (const auto& message : p.messages)
                {
                    std::string line;
                    std::stringstream s(message.text);
                    while (std::getline(s, line))
                    {
                        std::stringstream s2;
                        const std::time_t t = std::time(nullptr);
                        std::tm tm;
                        Time::localtime(&t, &tm);
                        s2 << std::put_time(&tm, "%c") << " ";
                        s2 << std::setfill(' ');
                        s2 << std::right;
                        s2 << std::setw(32);
                        s2 << message.prefix << " | ";
                        const auto& label = labels[message.level];
                        if (!label.empty())
                        {
                            s2 << label << " ";
                        }
                        s2 << line << std::endl;
                        io.write(s2.str());
                        if (p.consoleOutput)
                        {
                            std::cerr << s2.str();
                        }
                    }
                }
                p.messages.clear();
            }
            catch (const std::exception& e)
            {
                std::cerr << name << ": " << e.what() << std::endl;
            }
        }

    } // namespace Core
} // namespace djv
