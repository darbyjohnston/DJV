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

#pragma once

#include <djvCore/Path.h>

#include <chrono>
#include <list>
#include <mutex>

namespace djv
{
    namespace Core
    {
        class ISystem;
        class UndoStack;
        
        class Context
        {
            DJV_NON_COPYABLE(Context);

        protected:
            void _init(int &, char **);
            Context();

        public:
            virtual ~Context();

            //! Throws:
            //! - std::exception
            static std::unique_ptr<Context> create(int &, char **);

            //! Get the command line arguments.
            const std::vector<std::string> & getArgs() const;
            
            //! Get the context name.
            const std::string& getName() const;

            //! Get the average FPS.
            float getFpsAverage() const;

            //! \name Systems
            ///@{

            //! Get the systems.
            std::vector<std::weak_ptr<ISystem> > getSystems();

            //! Get systems by type.
            template<typename T>
            inline std::vector<std::weak_ptr<T> > getSystemsT();

            //! Get a system by type.
            template<typename T>
            inline std::weak_ptr<T> getSystemT();

            //! This function is called by the application to tick the systems.
            virtual void tick(float dt);

            ///@}

            const std::shared_ptr<UndoStack> & getUndoStack() const;

        protected:
            void _addSystem(const std::weak_ptr<ISystem>&);

        private:
            std::vector<std::string> _args;
            std::string _name;
            std::vector<std::weak_ptr<ISystem> > _systems;
            std::mutex _systemsMutex;
            std::vector<std::shared_ptr<ISystem> > _coreSystems;
            std::chrono::time_point<std::chrono::system_clock> _fpsTime = std::chrono::system_clock::now();
            std::list<float> _fpsSamples;
            float _fpsAverage = 0.f;
            std::shared_ptr<UndoStack> _undoStack;

            friend class ISystem;
        };

    } // namespace Core
} // namespace djv

#include <djvCore/ContextInline.h>

