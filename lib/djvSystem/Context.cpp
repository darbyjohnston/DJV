// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/Context.h>

#include <djvSystem/CoreSystem.h>
#include <djvSystem/FileIOFunc.h>
#include <djvSystem/IObject.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/PathFunc.h>
#include <djvSystem/ResourceSystem.h>
#include <djvSystem/TextSystem.h>
#include <djvSystem/TimerFunc.h>

#include <djvCore/MemoryFunc.h>
#include <djvCore/OSFunc.h>
#include <djvCore/Time.h>

#include <iostream>
#include <thread>

#if defined(DJV_PLATFORM_WINDOWS)
#include <fcntl.h>
#endif // DJV_PLATFORM_WINDOWS

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t statsRate       = 60;
            const size_t fpsSamplesCount = 10;

            void addSample(std::list<float>& list, float sample)
            {
                list.push_front(sample);
                while (list.size() > fpsSamplesCount)
                {
                    list.pop_back();
                }
            }
            
            float averageSamples(const std::list<float>& list)
            {
                float out = 0.F;
                for (const auto& i : list)
                {
                    out += i;
                }
                return out / static_cast<float>(list.size());
            }

            struct TickTimes
            {
                Time::Duration total = Time::Duration::zero();
                std::vector<std::pair<std::string, Time::Duration> > times;

                void add(const std::string& name, const Time::Duration& diff)
                {
                    times.push_back(std::make_pair(name, diff));
                    total += diff;
                }

                void sort()
                {
                    std::sort(
                        times.begin(),
                        times.end(),
                        [](const std::pair<std::string, Time::Duration>& a,
                            const std::pair<std::string, Time::Duration>& b)
                        {
                            return a.second > b.second;
                        });
                }

                void print()
                {
                    if (times.size() > 0)
                    {
                        std::cout << "System tick time: " <<
                            times[0].first << ", " <<
                            times[0].second.count() << std::endl;
                    }
                    for (const auto& i : times)
                    {
                        std::cout << i.first << ": " << i.second.count() << std::endl;
                    }
                    std::cout << "total: " << total.count() << std::endl << std::endl;
                }
            };

        } // namespace

        void Context::_init(const std::string& argv0)
        {
            _name = File::Path(argv0).getBaseName();

#if defined(DJV_PLATFORM_WINDOWS)
            _set_fmode(_O_BINARY);
#endif // DJV_PLATFORM_WINDOWS

            _timerSystem = TimerSystem::create(shared_from_this());
            _resourceSystem = ResourceSystem::create(argv0, shared_from_this());
            _logSystem = LogSystem::create(shared_from_this());
            _textSystem = TextSystem::create(shared_from_this());
            CoreSystem::create(argv0, shared_from_this());

            _logInfo(argv0);

            _fpsTimer = Timer::create(shared_from_this());
            _fpsTimer->setRepeating(true);
            auto weak = std::weak_ptr<Context>(shared_from_this());
            _fpsTimer->start(
                getTimerDuration(TimerValue::VerySlow),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
            {
                if (auto context = weak.lock())
                {
                    std::stringstream ss;
                    ss << "FPS: " << context->_fpsAverage;
                    context->_logSystem->log("djv::System::Context", ss.str());
                }
            });
            
            OS::getIntEnv("DJV_DEBUG", _debugEnv);
        }
        
        Context::Context()
        {}

        Context::~Context()
        {}

        std::shared_ptr<Context> Context::create(const std::string& argv0)
        {
            auto out = std::shared_ptr<Context>(new Context);
            out->_init(argv0);
            return out;
        }
        
        const std::string& Context::getName() const
        {
            return _name;
        }

        std::vector<std::shared_ptr<ISystemBase> > Context::getSystems() const
        {
            return _systems;
        }

        void Context::removeSystem(const std::shared_ptr<ISystemBase>& value)
        {
            auto i = _systems.begin();
            while (i != _systems.end())
            {
                if (value == *i)
                {
                    i = _systems.erase(i);
                }
                else
                {
                    ++i;
                }
            }
        }
                
        void Context::tick()
        {
            if (_logSystemOrderInit)
            {
                _logSystemOrderInit = false;
                _logSystemOrder();
                if (_debugEnv)
                {
                    _writeSystemDotGraph();
                }
            }

            _calcFPS();

            const bool doStats = 0 == _tickCount % statsRate;
            TickTimes tickTimes;
            auto start = std::chrono::steady_clock::now();
            for (const auto& system : _systems)
            {
                system->tick();
                
                if (doStats)
                {
                    const auto end = std::chrono::steady_clock::now();
                    const auto diff = std::chrono::duration_cast<Time::Duration>(end - start);
                    start = end;
                    tickTimes.add(system->getSystemName(), diff);
                }
            }
            
            if (doStats)
            {
                tickTimes.sort();
                //tickTimes.print();
                _systemTickTimes = tickTimes.times;
            }
            
            ++_tickCount;
        }

        float Context::getFPSAverage() const
        {
            return _fpsAverage;
        }

        const std::vector<std::pair<std::string, Time::Duration> >& Context::getSystemTickTimes() const
        {
            return _systemTickTimes;
        }

        void Context::_addSystem(const std::shared_ptr<ISystemBase>& system)
        {
            _systems.push_back(system);
        }

        void Context::_logInfo(const std::string& argv0)
        {
            std::stringstream ss;
            ss << "Application: " << _name << '\n';
            ss << "System: " << OS::getInformation() << '\n';
            ss << "Hardware concurrency: " << std::thread::hardware_concurrency() << '\n';
            {
                std::stringstream ss2;
                ss2 << Memory::Unit::GB;
                ss << "RAM: " << (OS::getRAMSize() / Memory::gigabyte) << _textSystem->getText(ss2.str()) << '\n';
            }
            ss << "argv0: " << argv0 << '\n';
            ss << "Resource paths:" << '\n';
            for (auto path : File::getResourcePathEnums())
            {
                std::stringstream ss2;
                ss2 << path;
                ss << "    " << _textSystem->getText(ss2.str()) << ": " << _resourceSystem->getPath(path) << '\n';
            }
            _logSystem->log("djv::System::Context", ss.str());
        }

        void Context::_logSystemOrder()
        {
            size_t count = 0;
            for (const auto& system : _systems)
            {
                {
                    std::stringstream ss;
                    ss << "Tick system #" << count << ": " << system->getSystemName();
                    _logSystem->log("djv::System::Context", ss.str());
                    ++count;
                }
            }
        }
        
        void Context::_writeSystemDotGraph()
        {
            std::vector<std::string> dot;
            dot.push_back("digraph {");
            for (const auto& system : _systems)
            {
                for (const auto& dependency : system->getDependencies())
                {
                    std::stringstream ss;
                    ss << "    " << "\"" << system->getSystemName() << "\"";
                    ss << " -> " << "\"" << dependency->getSystemName() << "\"";
                    dot.push_back(ss.str());
                }
            }
            dot.push_back("}");
            File::writeLines("systems.dot", dot);
        }

        void Context::_calcFPS()
        {
            const auto now = std::chrono::steady_clock::now();
            const std::chrono::duration<float> delta = now - _fpsTime;
            _fpsTime = now;
            addSample(_fpsSamples, delta.count());
            _fpsAverage = 1.F / averageSamples(_fpsSamples);
        }

    } // namespace System
} // namespace djv

