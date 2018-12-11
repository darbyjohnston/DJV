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

namespace
{
    const size_t fpsSamplesCount = 100;

} // namespace

namespace djv
{
    namespace Core
    {
        void Context::_init(int & argc, char ** argv)
        {
            for (int i = 0; i < argc; ++i)
            {
                _args.push_back(argv[i]);
            }
            const std::string argv0 = argc ? argv[0] : std::string();
            _name = Path(argv0).getBaseName();

            // Create the core systems.
            auto timerSystem = TimerSystem::create(this);
            _coreSystems.push_back(timerSystem);
            auto resourceSystem = ResourceSystem::create(argv0, this);
            _coreSystems.push_back(resourceSystem);
            auto logSystem = LogSystem::create(resourceSystem->getPath(ResourcePath::LogFile), this);
            _coreSystems.push_back(logSystem);
            _coreSystems.push_back(TextSystem::create(this));
            _coreSystems.push_back(AnimationSystem::create(this));

            // Log information.
            std::stringstream s;
            s << "Application: " << _name << '\n';
            s << "System information: " << OS::getInformation() << '\n';
            s << "Hardware concurrency: " << std::thread::hardware_concurrency() << '\n';
            s << "Resource paths:" << '\n';
            for (auto path : getResourcePathEnums())
            {
                s << "    " << path << ": " << resourceSystem->getPath(path) << '\n';
            }
            logSystem->log("djv::Core::Context", s.str());

            _undoStack = UndoStack::create(this);
        }

        Context::Context()
        {}

        Context::~Context()
        {
            while (_coreSystems.size())
            {
                _coreSystems.pop_back();
            }
        }

        std::unique_ptr<Context> Context::create(int & argc, char ** argv)
        {
            auto out = std::unique_ptr<Context>(new Context);
            out->_init(argc, argv);
            return std::move(out);
        }

        const std::vector<std::string> & Context::getArgs() const
        {
            return _args;
        }
           
        const std::string& Context::getName() const
        {
            return _name;
        }

        float Context::getFpsAverage() const
        {
            return _fpsAverage;
        }

        std::vector<std::weak_ptr<ISystem> > Context::getSystems()
        {
            std::vector<std::weak_ptr<ISystem> > out;
            {
                std::lock_guard<std::mutex> lock(_systemsMutex);
                out = _systems;
            }
            return out;
        }

        const std::shared_ptr<UndoStack> & Context::getUndoStack() const
        {
            return _undoStack;
        }

        void Context::tick(float dt)
        {
            const auto now = std::chrono::system_clock::now();
            const std::chrono::duration<float> delta = now - _fpsTime;
            _fpsTime = now;
            _fpsSamples.push_front(1.f / delta.count());
            while (_fpsSamples.size() > fpsSamplesCount)
            {
                _fpsSamples.pop_back();
            }
            _fpsAverage = 0.f;
            for (auto i : _fpsSamples)
            {
                _fpsAverage += i;
            }
            _fpsAverage /= static_cast<float>(_fpsSamples.size());
            //std::cout << "fps = " << _fpsAverage << std::endl;

            static bool logSystemOrder = true;
            std::vector<std::weak_ptr<ISystem> > systems;
            {
                std::lock_guard<std::mutex> lock(_systemsMutex);
                systems = _systems;
            }
            size_t count = 0;
            for (const auto & i : systems)
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

        void Context::_addSystem(const std::weak_ptr<ISystem>& system)
        {
            std::lock_guard<std::mutex> lock(_systemsMutex);
            _systems.push_back(system);
        }

    } // namespace ViewExperiment
} // namespace djv

