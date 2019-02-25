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

namespace djv
{
    namespace Core
    {
		class CoreSystem;
        class IObject;
        class ISystem;

        //! This class provides core functionality.
        class Context
        {
            DJV_NON_COPYABLE(Context);

        protected:
            void _init(int &, char **);
            inline Context();

        public:
            virtual ~Context();

            //! Throws:
            //! - std::exception
            static std::unique_ptr<Context> create(int &, char **);

            //! Get the command line arguments.
            inline const std::vector<std::string> & getArgs() const;
            
            //! Get the context name.
            inline const std::string& getName() const;

            //! Get the average FPS.
            inline float getFpsAverage() const;

            //! Get the root object.
            std::shared_ptr<IObject> getRootObject() const;

            //! \name Systems
            ///@{

            //! Get the list of systems.
            inline const std::vector<std::weak_ptr<ISystem> > & getSystems() const;

            //! Get the list of systems of the given type.
            template<typename T>
            inline std::vector<std::weak_ptr<T> > getSystemsT() const;

            //! Get a system of the given type.
            template<typename T>
            inline std::weak_ptr<T> getSystemT() const;

			//! Get the core system.
			inline const std::shared_ptr<CoreSystem> & getCoreSystem() const;

            //! This function is called by the application's event loop.
            virtual void tick(float dt);

            //! \name Utilities
            ///@{

            //! Convenience function for message logging.
            void log(const std::string& prefix, const std::string& message, LogLevel = LogLevel::Information);

            //! Convenience function got getting a resource path.
            FileSystem::Path getPath(FileSystem::ResourcePath) const;

            //! Convenience function got getting a resource path.
            FileSystem::Path getPath(FileSystem::ResourcePath, const std::string &) const;

            //! Convenience function for text translations.
            std::string getText(const std::string & id) const;

            ///@}

        protected:
            void _addSystem(const std::shared_ptr<ISystem> &);

        private:
            std::vector<std::string> _args;
            std::string _name;
            class RootObject;
            std::shared_ptr<RootObject> _rootObject;
            std::vector<std::weak_ptr<ISystem> > _systems;
			std::shared_ptr<CoreSystem> _coreSystem;
            std::chrono::time_point<std::chrono::system_clock> _fpsTime = std::chrono::system_clock::now();
            std::list<float> _fpsSamples;
            float _fpsAverage = 0.f;

            friend class ISystem;
        };

    } // namespace Core
} // namespace djv

#include <djvCore/ContextInline.h>

