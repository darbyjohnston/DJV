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

#include <djvCore/Context.h>

#include <djvCore/Animation.h>
#include <djvCore/ISystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>
#include <djvCore/UndoStack.h>

#include <chrono>
#include <list>
#include <mutex>

namespace
{
    const size_t fpsSamplesCount = 100;

} // namespace

namespace djv
{
    namespace Core
    {
        struct Context::Private
        {
            std::vector<std::string> args;
            std::string name;
            std::vector<std::weak_ptr<ISystem> > systems;
            std::shared_ptr<Time::TimerSystem> timerSystem;
            std::shared_ptr<ResourceSystem> resourceSystem;
            std::shared_ptr<LogSystem> logSystem;
            std::shared_ptr<TextSystem> textSystem;
            std::shared_ptr<Animation::System> animationSystem;
            std::chrono::time_point<std::chrono::system_clock> fpsTime = std::chrono::system_clock::now();
            std::list<float> fpsSamples;
            float fpsAverage = 0.f;
            std::shared_ptr<UndoStack> undoStack;
        };

        void Context::_init(int & argc, char ** argv)
        {
            DJV_PRIVATE_PTR();
            for (int i = 0; i < argc; ++i)
            {
                p.args.push_back(argv[i]);
            }
            const std::string argv0 = argc ? argv[0] : std::string();
            p.name = FileSystem::Path(argv0).getBaseName();

            // Create the core systems.
            p.timerSystem = Time::TimerSystem::create(this);
            p.resourceSystem = ResourceSystem::create(argv0, this);
            p.logSystem = LogSystem::create(p.resourceSystem->getPath(FileSystem::ResourcePath::LogFile), this);
            p.textSystem = TextSystem::create(this);
            p.animationSystem = Animation::System::create(this);

            // Log information.
            std::stringstream s;
            s << "Application: " << p.name << '\n';
            s << "System information: " << OS::getInformation() << '\n';
            s << "Hardware concurrency: " << std::thread::hardware_concurrency() << '\n';
            s << "Resource paths:" << '\n';
            for (auto path : FileSystem::getResourcePathEnums())
            {
                s << "    " << path << ": " << p.resourceSystem->getPath(path) << '\n';
            }
            p.logSystem->log("djv::Core::Context", s.str());

            p.undoStack = UndoStack::create(this);
        }

        Context::Context() :
            _p(new Private)
        {}

        Context::~Context()
        {}

        std::unique_ptr<Context> Context::create(int & argc, char ** argv)
        {
            auto out = std::unique_ptr<Context>(new Context);
            out->_init(argc, argv);
            return std::move(out);
        }

        const std::vector<std::string> & Context::getArgs() const
        {
            return _p->args;
        }
           
        const std::string& Context::getName() const
        {
            return _p->name;
        }

        float Context::getFpsAverage() const
        {
            return _p->fpsAverage;
        }

        const std::vector<std::weak_ptr<ISystem> > & Context::getSystems() const
        {
            return _p->systems;
        }

        void Context::log(const std::string& prefix, const std::string& message, LogLevel level)
        {
            _p->logSystem->log(prefix, message, level);
        }

        FileSystem::Path Context::getPath(FileSystem::ResourcePath value) const
        {
            return _p->resourceSystem->getPath(value);
        }

        FileSystem::Path Context::getPath(FileSystem::ResourcePath value, const std::string & fileName) const
        {
            return FileSystem::Path(_p->resourceSystem->getPath(value), fileName);
        }

        const std::shared_ptr<UndoStack> & Context::getUndoStack() const
        {
            return _p->undoStack;
        }

        void Context::tick(float dt)
        {
            DJV_PRIVATE_PTR();
            const auto now = std::chrono::system_clock::now();
            const std::chrono::duration<float> delta = now - p.fpsTime;
            p.fpsTime = now;
            p.fpsSamples.push_front(1.f / delta.count());
            while (p.fpsSamples.size() > fpsSamplesCount)
            {
                p.fpsSamples.pop_back();
            }
            p.fpsAverage = 0.f;
            for (auto i : p.fpsSamples)
            {
                p.fpsAverage += i;
            }
            p.fpsAverage /= static_cast<float>(p.fpsSamples.size());
            //std::cout << "fps = " << p.fpsAverage << std::endl;

            static bool logSystemOrder = true;
            size_t count = 0;
            for (const auto & i : p.systems)
            {
                if (auto system = i.lock())
                {
                    if (logSystemOrder)
                    {
                        if (auto logSystem = getSystemT<LogSystem>().lock())
                        {
                            std::stringstream s;
                            s << "Tick system #" << count << ": " << system->getName();
                            logSystem->log("djv::Core::Context", s.str());
                            ++count;
                        }
                    }
                    system->_tick(dt);
                }
            }
            logSystemOrder = false;
        }

        std::shared_ptr<ResourceSystem> Context::getResourceSystem() const
        {
            return _p->resourceSystem;
        }

        std::shared_ptr<LogSystem> Context::getLogSystem() const
        {
            return _p->logSystem;
        }

        void Context::_addSystem(const std::weak_ptr<ISystem>& system)
        {
            _p->systems.push_back(system);
        }

    } // namespace ViewExperiment
} // namespace djv

