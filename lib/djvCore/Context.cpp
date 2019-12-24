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

#include <djvCore/Context.h>

#include <djvCore/CoreSystem.h>
#include <djvCore/FileIO.h>
#include <djvCore/IObject.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Time.h>
#include <djvCore/Timer.h>

#include <thread>

#if defined(DJV_PLATFORM_WINDOWS)
#include <fcntl.h>
#endif // DJV_PLATFORM_WINDOWS

namespace
{
    const size_t fpsSamplesCount = 100;

} // namespace

namespace djv
{
    namespace Core
    {
        void Context::_init(const std::vector<std::string>& args)
        {
            _args = args;
            const std::string argv0 = _args.size() > 0 ? _args[0] : std::string();
            _name = FileSystem::Path(argv0).getBaseName();

#if defined(DJV_PLATFORM_WINDOWS)
            _set_fmode(_O_BINARY);
#endif // DJV_PLATFORM_WINDOWS

            _timerSystem = Time::TimerSystem::create(shared_from_this());
            _resourceSystem = ResourceSystem::create(argv0, shared_from_this());
            _logSystem = LogSystem::create(shared_from_this());
            _textSystem = TextSystem::create(shared_from_this());
            CoreSystem::create(argv0, shared_from_this());

            {
                std::stringstream ss;
                ss << "Application: " << _name << '\n';
                ss << "System information: " << OS::getInformation() << '\n';
                ss << "Hardware concurrency: " << std::thread::hardware_concurrency() << '\n';
                ss << "System RAM: " << (OS::getRAMSize() / Memory::gigabyte) << "GB" << '\n';
                ss << "argv0: " << argv0 << '\n';
                ss << "Resource paths:" << '\n';
                for (auto path : FileSystem::getResourcePathEnums())
                {
                    ss << "    " << path << ": " << _resourceSystem->getPath(path) << '\n';
                }
                _logSystem->log("djv::Core::Context", ss.str());
            }

            _fpsTimer = Time::Timer::create(shared_from_this());
            _fpsTimer->setRepeating(true);
            auto weak = std::weak_ptr<Context>(shared_from_this());
            _fpsTimer->start(
                Time::getMilliseconds(Time::TimerValue::VerySlow),
                [weak](float)
            {
                if (auto context = weak.lock())
                {
                    std::stringstream ss;
                    ss << "FPS: " << context->_fpsAverage;
                    context->_logSystem->log("djv::Core::Context", ss.str());
                }
            });
        }

        Context::~Context()
        {}

        std::shared_ptr<Context> Context::create(const std::vector<std::string>& args)
        {
            auto out = std::shared_ptr<Context>(new Context);
            out->_init(args);
            return out;
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
        
        void Context::tick(float dt)
        {
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<float> delta = now - _fpsTime;
            _fpsTime = now;
            _fpsSamples.push_front(1.F / delta.count());
            while (_fpsSamples.size() > fpsSamplesCount)
            {
                _fpsSamples.pop_back();
            }
            _fpsAverage = 0.F;
            for (auto i : _fpsSamples)
            {
                _fpsAverage += i;
            }
            _fpsAverage /= static_cast<float>(_fpsSamples.size());
            //std::cout << "fps = " << _fpsAverage << std::endl;

            static bool logSystemOrder = true;
            size_t count = 0;
            if (logSystemOrder)
            {
                logSystemOrder = false;
                std::vector<std::string> dot;
                dot.push_back("digraph {");
                for (const auto & system : _systems)
                {
                    {
                        std::stringstream ss;
                        ss << "Tick system #" << count << ": " << system->getSystemName();
                        _logSystem->log("djv::Core::Context", ss.str());
                        ++count;
                    }
                    for (const auto & dependency : system->getDependencies())
                    {
                        std::stringstream ss;
                        ss << "    " << "\"" << system->getSystemName() << "\"";
                        ss << " -> " << "\"" << dependency->getSystemName() << "\"";
                        dot.push_back(ss.str());
                    }
                }
                dot.push_back("}");
                //FileSystem::FileIO::writeLines("systems.dot", dot);
            }

            float total = 0.F;
            auto start = std::chrono::steady_clock::now();
            std::vector<std::pair<std::string, float> > systemTickTimes;
            for (const auto & system : _systems)
            {
                system->tick(dt);
                auto end = std::chrono::steady_clock::now();
                std::chrono::duration<float, std::milli> diff = end - start;
                systemTickTimes.push_back(std::make_pair(system->getSystemName(), diff.count()));
                start = end;
                total += diff.count();
            }
            std::sort(
                systemTickTimes.begin(),
                systemTickTimes.end(),
                [](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b)
                {
                    return a.second > b.second;
                });
            /*for (const auto& i : systemTickTimes)
            {
                std::cout << i.first << ": " << i.second << std::endl;
            }
            std::cout << "total: " << total << std::endl << std::endl;*/
            _systemTickTimes = systemTickTimes;
        }

        void Context::_addSystem(const std::shared_ptr<ISystemBase> & system)
        {
            _systems.push_back(system);
        }

    } // namespace ViewExperiment
} // namespace djv

