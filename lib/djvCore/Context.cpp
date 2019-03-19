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
#include <djvCore/IObject.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/TextSystem.h>

#include <thread>

namespace
{
    const size_t fpsSamplesCount = 100;

} // namespace

namespace djv
{
    namespace Core
    {
        class Context::RootObject : public IObject
        {
        public:
            RootObject()
            {
                setClassName("djv::Core::Context::RootObject");
            }

            ~RootObject() override
            {}
        };

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

            // Create the core system.
            _coreSystem = CoreSystem::create(argv0, this);

            // Log information.
            if (auto logSystem = getSystemT<LogSystem>().lock())
            {
                std::stringstream s;
                s << "Application: " << _name << '\n';
                s << "System information: " << OS::getInformation() << '\n';
                s << "Hardware concurrency: " << std::thread::hardware_concurrency() << '\n';
                if (auto resourceSystem = getSystemT<ResourceSystem>().lock())
                {
                    s << "Resource paths:" << '\n';
                    for (auto path : FileSystem::getResourcePathEnums())
                    {
                        s << "    " << path << ": " << resourceSystem->getPath(path) << '\n';
                    }
                }
                logSystem->log("djv::Core::Context", s.str());
            }
        }

        Context::~Context()
        {
            _rootObject->clearChildren();
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

        void Context::log(const std::string & prefix, const std::string & message, LogLevel level)
        {
            if (auto logSystem = getSystemT<LogSystem>().lock())
            {
                logSystem->log(prefix, message, level);
            }
        }

        FileSystem::Path Context::getPath(FileSystem::ResourcePath value) const
        {
            FileSystem::Path out;
            if (auto resourceSystem = getSystemT<ResourceSystem>().lock())
            {
                out = resourceSystem->getPath(value);
            }
            return out;
        }

        FileSystem::Path Context::getPath(FileSystem::ResourcePath value, const std::string & fileName) const
        {
            FileSystem::Path out;
            if (auto resourceSystem = getSystemT<ResourceSystem>().lock())
            {
                out = FileSystem::Path(resourceSystem->getPath(value), fileName);
            }
            return out;
        }

        std::string Context::getText(const std::string & id) const
        {
            std::string out;
            if (auto textSystem = getSystemT<TextSystem>().lock())
            {
                out = textSystem->getText(id);
            }
            return out;
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
            _rootObject->addChild(system);
            _systems.push_back(system);
        }

    } // namespace ViewExperiment
} // namespace djv

