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

#include <djvCore/Core.h>

#include <memory>
#include <string>
#include <vector>

namespace djv
{
    namespace Core
    {
        class Context;
        class LogSystem;
        class ResourceSystem;
        class TextSystem;

        //! This class provides the very base functionality for systems.
        class ISystemBase : public std::enable_shared_from_this<ISystemBase>
        {
        protected:
            void _init(const std::string& name, Context*);
            inline ISystemBase();

        public:
            virtual ~ISystemBase() = 0;

            //! Get the system name.
            inline const std::string& getSystemName() const;

            //! Get the context.
            inline Context* getContext() const;

            //! Get the list of system dependencies.
            inline const std::vector<std::shared_ptr<ISystemBase> >& getDependencies() const;
            
            //! Add a system dependency.
            void addDependency(const std::shared_ptr<ISystemBase>&);

            //! Override this function to do work each frame.
            virtual void tick(float dt) {};

        private:
            std::string _name;
            Context* _context = nullptr;
            std::vector<std::shared_ptr<ISystemBase> > _dependencies;
        };

        //! This class provides the base functionality for systems.
        class ISystem : public ISystemBase
        {
        protected:
            void _init(const std::string& name, Context*);
            inline ISystem();

        public:
            ~ISystem() override;

        protected:
            //! Log a message.
            void _log(const std::string& message, Core::LogLevel = Core::LogLevel::Information);
            
            //! Get the resource system.
            std::shared_ptr<ResourceSystem> _getResourceSystem() const;
            
            //! Translate a text ID.
            const std::string& _getText(const std::string& id) const;

        private:
            std::shared_ptr<LogSystem> _logSystem;
            std::shared_ptr<ResourceSystem> _resourceSystem;
            std::shared_ptr<TextSystem> _textSystem;
        };

    } // namespace Core
} // namespace djv

#include <djvCore/ISystemInline.h>
