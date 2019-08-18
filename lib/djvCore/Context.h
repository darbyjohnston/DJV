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

#pragma once

#include <djvCore/Path.h>

#include <chrono>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace djv
{
    namespace Core
    {
        class IObject;
        class ISystemBase;
        class LogSystem;
        class ResourceSystem;
        class TextSystem;

        namespace Time
        {
            class Timer;
            class TimerSystem;

        } // namespace Time

        //! This class provides core functionality.
        class Context
        {
            DJV_NON_COPYABLE(Context);

        protected:
            void _init(const std::vector<std::string>&);
            inline Context();

        public:
            virtual ~Context();

            //! Throws:
            //! - std::exception
            static Context* create(int&, char*[]);

            //! Throws:
            //! - std::exception
            static Context* create(const std::vector<std::string>&);

            //! Get the command line arguments.
            inline const std::vector<std::string> & getArgs() const;
            
            //! Get the context name.
            inline const std::string & getName() const;

            //! Get the average FPS.
            inline float getFpsAverage() const;

            //! \name Systems
            ///@{

            //! Get the list of systems.
            inline std::vector<std::shared_ptr<ISystemBase> > getSystems() const;

            //! Get the list of systems of the given type.
            template<typename T>
            inline std::vector<std::shared_ptr<T> > getSystemsT() const;

            //! Get a system of the given type. If the system is not found a
            //! null pointer is returned.
            template<typename T>
            inline std::shared_ptr<T> getSystemT() const;

            //! This function is called by the application event loop.
            virtual void tick(float dt);

            //! Get the average tick FPS.
            float getFPSAverage() const;

        protected:
            void _addSystem(const std::shared_ptr<ISystemBase> &);

        private:
            std::vector<std::string> _args;
            std::string _name;
            std::shared_ptr<Time::TimerSystem> _timerSystem;
            std::shared_ptr<ResourceSystem> _resourceSystem;
            std::shared_ptr<LogSystem> _logSystem;
            std::shared_ptr<TextSystem> _textSystem;
            std::vector<std::shared_ptr<ISystemBase> > _systems;
            std::chrono::time_point<std::chrono::system_clock> _fpsTime = std::chrono::system_clock::now();
            std::list<float> _fpsSamples;
            float _fpsAverage = 0.f;
            std::shared_ptr<Time::Timer> _fpsTimer;

            friend class ISystemBase;
        };

    } // namespace Core
} // namespace djv

#include <djvCore/ContextInline.h>

