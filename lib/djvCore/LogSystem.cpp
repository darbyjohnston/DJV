// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/LogSystem.h>

#include <djvCore/Context.h>
#include <djvCore/ErrorFunc.h>
#include <djvCore/FileIO.h>
#include <djvCore/OSFunc.h>
#include <djvCore/PathFunc.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/TimeFunc.h>
#include <djvCore/Timer.h>

#include <atomic>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <thread>

namespace djv
{
    namespace Core
    {
        namespace
        {
            const std::string name = "djv::Core::LogSystem";

            struct Message
            {
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
            std::vector<std::string> warnings;
            std::shared_ptr<ListSubject<std::string> > warningsSubject;
            std::vector<std::string> errors;
            std::shared_ptr<ListSubject<std::string> > errorsSubject;
            std::list<Message> queue;
            std::condition_variable queueCV;
            std::list<Message> messages;
            std::mutex mutex;
            std::thread thread;
            std::atomic<bool> running;
            std::shared_ptr<Time::Timer> warningsAndErrorsTimer;
        };

        void LogSystem::_init(const std::shared_ptr<Context>& context)
        {
            ISystemBase::_init(name, context);
            DJV_PRIVATE_PTR();

            auto resourceSystem = context->getSystemT<ResourceSystem>();
            addDependency(resourceSystem);
            
            p.path = resourceSystem->getPath(FileSystem::ResourcePath::LogFile);
            p.consoleOutput = false;
            p.warningsSubject = ListSubject<std::string>::create();
            p.errorsSubject = ListSubject<std::string>::create();

            p.running = true;
            p.thread = std::thread(
                [this]
            {
                DJV_PRIVATE_PTR();
                int env = 0;
                if (OS::getIntEnv("DJV_LOG_CONSOLE", env))
                {
                    p.consoleOutput = env != 0;
                }

                try
                {
                    auto io = FileSystem::FileIO::create();
                    io->open(std::string(p.path), FileSystem::FileIO::Mode::Write);
                }
                catch (const std::exception& e)
                {
                    std::cerr << name << ": " << e.what() << std::endl;
                }

                std::stringstream ss;
                ss << "Log file path: " << p.path;
                log("djv::Core::LogSystem", ss.str());

                const auto timeout = Time::getValue(Time::TimerValue::Slow);
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

            p.warningsAndErrorsTimer = Time::Timer::create(context);
            p.warningsAndErrorsTimer->setRepeating(true);
            auto weak = std::weak_ptr<LogSystem>(std::dynamic_pointer_cast<LogSystem>(shared_from_this()));
            p.warningsAndErrorsTimer->start(
                Time::getTime(Time::TimerValue::Medium),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                {
                    if (auto system = weak.lock())
                    {
                        std::vector<std::string> warnings;
                        std::vector<std::string> errors;
                        {
                            std::unique_lock<std::mutex> lock(system->_p->mutex);
                            warnings = std::move(system->_p->warnings);
                            errors = std::move(system->_p->errors);
                        }
                        if (warnings.size())
                        {
                            system->_p->warningsSubject->setAlways(warnings);
                        }
                        if (errors.size())
                        {
                            system->_p->errorsSubject->setAlways(errors);
                        }
                    }
                });
        }

        LogSystem::LogSystem() :
            _p(new Private)
        {}

        LogSystem::~LogSystem()
        {
            DJV_PRIVATE_PTR();
            p.running = false;
            if (p.thread.joinable())
            {
                p.thread.join();
            }
        }
        
        std::shared_ptr<LogSystem> LogSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<LogSystem>(new LogSystem);
            out->_init(context);
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

        std::shared_ptr<Core::IListSubject<std::string> > LogSystem::observeWarnings() const
        {
            return _p->warningsSubject;
        }

        std::shared_ptr<Core::IListSubject<std::string> > LogSystem::observeErrors() const
        {
            return _p->errorsSubject;
        }

        void LogSystem::_writeMessages()
        {
            DJV_PRIVATE_PTR();
            try
            {
                std::vector<std::string> warnings;
                std::vector<std::string> errors;

                auto io = FileSystem::FileIO::create();
                io->open(std::string(p.path), FileSystem::FileIO::Mode::Append);
                io->seek(io->getSize());

                std::map<LogLevel, std::string> labels =
                {
                    { LogLevel::Information, std::string() },
                    { LogLevel::Warning, "[Warning]" },
                    { LogLevel::Error, "[ERROR]" }
                };

                for (const auto& message : p.messages)
                {
                    switch (message.level)
                    {
                    case LogLevel::Warning: warnings.push_back(message.text); break;
                    case LogLevel::Error:   errors.push_back(message.text);   break;
                    default: break;
                    }

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
                        io->write(s2.str());
                        if (p.consoleOutput)
                        {
                            std::cerr << s2.str();
                        }
                    }
                }
                p.messages.clear();
                if (warnings.size() || errors.size())
                {
                    std::unique_lock<std::mutex> lock(p.mutex);
                    for (const auto& i : warnings)
                    {
                        p.warnings.push_back(i);
                    }
                    for (const auto& i : errors)
                    {
                        p.errors.push_back(i);
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << name << ": " << e.what() << std::endl;
            }
        }

    } // namespace Core
} // namespace djv
