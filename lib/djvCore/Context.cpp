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

#include <list>

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
            std::vector<std::shared_ptr<ISystem> > systems;
            struct CoreSystems
            {
                std::shared_ptr<TimerSystem> timer;
                std::shared_ptr<ResourceSystem> resource;
                std::shared_ptr<LogSystem> log;
                std::shared_ptr<TextSystem> text;
                std::shared_ptr<AnimationSystem> animation;
            };
            std::unique_ptr<CoreSystems> coreSystems;
            std::chrono::time_point<std::chrono::system_clock> fpsTime = std::chrono::system_clock::now();
            std::list<float> fpsSamples;
            float fpsAverage = 0.f;
            std::shared_ptr<UndoStack> undoStack;
        };

        void Context::_init(int & argc, char ** argv)
        {
            for (int i = 0; i < argc; ++i)
            {
                _p->args.push_back(argv[i]);
            }
            const std::string argv0 = argc ? argv[0] : std::string();
            _p->name = Path(argv0).getBaseName();

            // Create the core systems.
            _p->coreSystems.reset(new Private::CoreSystems);
            _p->coreSystems->timer = TimerSystem::create(shared_from_this());
            _p->coreSystems->resource = ResourceSystem::create(argv0, shared_from_this());
            _p->coreSystems->log = LogSystem::create(_p->coreSystems->resource->getPath(ResourcePath::LogFile), shared_from_this());
            _p->coreSystems->text = TextSystem::create(shared_from_this());
            _p->coreSystems->animation = AnimationSystem::create(shared_from_this());

            // Log information.
            std::stringstream s;
            s << "Application: " << _p->name << '\n';
            s << "System information: " << OS::getInformation() << '\n';
            s << "Resource paths:" << '\n';
            for (auto path : getResourcePathEnums())
            {
                s << "    " << path << ": " << _p->coreSystems->resource->getPath(path) << '\n';
            }
            log("djv::Core::Context", s.str());

            _p->undoStack = UndoStack::create(shared_from_this());
        }

        Context::Context() :
            _p(new Private)
        {}

        Context::~Context()
        {}

        std::shared_ptr<Context> Context::create(int & argc, char ** argv)
        {
            auto out = std::shared_ptr<Context>(new Context);
            out->_init(argc, argv);
            return out;
        }

        const std::string& Context::getName() const
        {
            return _p->name;
        }

        void Context::exit()
        {
            std::stringstream s;
            s << "Exiting: " << _p->name;
            log("djv::Core::Context", s.str());

            auto systems = _p->systems;
            for (auto system = systems.rbegin(); system != systems.rend(); ++system)
            {
                (*system)->_exit();
            }

            //! \bug [1.0 S] It seems on OSX that the "Context::_p" pointer is getting
            //! set to NULL before the pointers are getting reset. So
            //! unfortunately we need to manually reset the pointer here.
            _p->coreSystems.reset();
        }

        float Context::getFpsAverage() const
        {
            return _p->fpsAverage;
        }

        const std::vector<std::shared_ptr<ISystem> >& Context::getSystems() const
        {
            return _p->systems;
        }

        const std::shared_ptr<ResourceSystem>& Context::getResourceSystem() const
        {
            return _p->coreSystems->resource;
        }

        const std::shared_ptr<LogSystem>& Context::getLogSystem() const
        {
            return _p->coreSystems->log;
        }

        const std::shared_ptr<TextSystem>& Context::getTextSystem() const
        {
            return _p->coreSystems->text;
        }

        Path Context::getResourcePath(ResourcePath value) const
        {
            return _p->coreSystems->resource->getPath(value);
        }

        Path Context::getResourcePath(ResourcePath value, const Path& fileName) const
        {
            Path path = getResourcePath(value);
            path.append(fileName);
            return path;
        }

        Path Context::getResourcePath(ResourcePath value, const std::string& fileName) const
        {
            Path path = getResourcePath(value);
            path.append(fileName);
            return path;
        }

        const std::shared_ptr<UndoStack> & Context::getUndoStack() const
        {
            return _p->undoStack;
        }

        void Context::log(const std::string& prefix, const std::string& message, LogLevel level)
        {
            if (_p->coreSystems->log)
            {
                _p->coreSystems->log->log(prefix, message, level);
            }
        }

        void Context::tick(float dt)
        {
            const auto now = std::chrono::system_clock::now();
            const std::chrono::duration<float> delta = now - _p->fpsTime;
            _p->fpsTime = now;
            _p->fpsSamples.push_front(1.f / delta.count());
            while (_p->fpsSamples.size() > fpsSamplesCount)
            {
                _p->fpsSamples.pop_back();
            }
            _p->fpsAverage = 0.f;
            for (auto i : _p->fpsSamples)
            {
                _p->fpsAverage += i;
            }
            _p->fpsAverage /= static_cast<float>(_p->fpsSamples.size());
            //std::cout << "fps = " << _p->fpsAverage << std::endl;

            static bool logSystemOrder = true;
            auto systems = _p->systems;
            size_t count = 0;
            for (const auto & system : systems)
            {
                if (logSystemOrder)
                {
                    std::stringstream s;
                    s << "Tick system #" << count << ": " << system->getName();
                    log("djv::Core::Context", s.str());
                    ++count;
                }
                system->_tick(dt);
            }
            logSystemOrder = false;
        }

        void Context::_addSystem(const std::shared_ptr<ISystem>& system)
        {
            _p->systems.push_back(system);
        }

    } // namespace ViewExperiment
} // namespace djv

