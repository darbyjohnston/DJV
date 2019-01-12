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
#include <djvCore/IEventLoop.h>
#include <djvCore/IObject.h>
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
        class Context::RootObject : public IObject {};

        void Context::_init(int & argc, char ** argv)
        {
            for (int i = 0; i < argc; ++i)
            {
                _args.push_back(argv[i]);
            }
            const std::string argv0 = argc ? argv[0] : std::string();
            _name = FileSystem::Path(argv0).getBaseName();

            // Create the root object.
            _rootObject = std::shared_ptr<RootObject>(new RootObject);

            // Create the core systems.
            _timerSystem = Time::TimerSystem::create(this);
            _resourceSystem = ResourceSystem::create(argv0, this);
            _logSystem = LogSystem::create(_resourceSystem->getPath(FileSystem::ResourcePath::LogFile), this);
            _textSystem = TextSystem::create(this);
            _animationSystem = Animation::System::create(this);

            // Log information.
            std::stringstream s;
            s << "Application: " << _name << '\n';
            s << "System information: " << OS::getInformation() << '\n';
            s << "Hardware concurrency: " << std::thread::hardware_concurrency() << '\n';
            s << "Resource paths:" << '\n';
            for (auto path : FileSystem::getResourcePathEnums())
            {
                s << "    " << path << ": " << _resourceSystem->getPath(path) << '\n';
            }
            _logSystem->log("djv::Core::Context", s.str());

            _undoStack = UndoStack::create(this);
        }

        std::unique_ptr<Context> Context::create(int & argc, char ** argv)
        {
            auto out = std::unique_ptr<Context>(new Context);
            out->_init(argc, argv);
            return std::move(out);
        }

        std::shared_ptr<IObject> Context::getRootObject() const
        {
            return _rootObject;
        }

        void Context::log(const std::string& prefix, const std::string& message, LogLevel level)
        {
            _logSystem->log(prefix, message, level);
        }

        FileSystem::Path Context::getPath(FileSystem::ResourcePath value) const
        {
            return _resourceSystem->getPath(value);
        }

        FileSystem::Path Context::getPath(FileSystem::ResourcePath value, const std::string & fileName) const
        {
            return FileSystem::Path(_resourceSystem->getPath(value), fileName);
        }

        std::string Context::getText(const std::string & id) const
        {
            return _textSystem->getText(id);
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
            size_t count = 0;
            for (const auto & i : _systems)
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
                    system->tick(dt);
                }
            }
            logSystemOrder = false;
        }

        void Context::_addSystem(const std::shared_ptr<ISystem> & system)
        {
            system->setParent(_rootObject);
            _systems.push_back(system);
        }

    } // namespace ViewExperiment
} // namespace djv

