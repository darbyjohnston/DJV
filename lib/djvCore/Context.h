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

namespace djv
{
    namespace Core
    {
        class ISystem;
        class LogSystem;
        class ResourceSystem;
        class TextSystem;
        class UndoStack;
        
        class Context : public std::enable_shared_from_this<Context>
        {
            DJV_NON_COPYABLE(Context);

        protected:
            void _init(int &, char **);
            Context();

        public:
            virtual ~Context();

            //! Throws:
            //! - std::exception
            static std::shared_ptr<Context> create(int &, char **);

            //! Get the context name.
            const std::string& getName() const;

            //! This should be called before destroying the context.
            virtual void exit();

            //! Get the average FPS.
            float getFpsAverage() const;

            //! \name Systems
            ///@{

            //! Get the systems.
            const std::vector<std::shared_ptr<ISystem> > & getSystems() const;

            //! Get systems by type.
            template<typename T>
            inline std::vector<std::shared_ptr<T> > getSystemsT() const;

            //! Get a system by type.
            template<typename T>
            inline std::shared_ptr<T> getSystemT() const;

            //! This function is called by the application to tick the systems.
            virtual void tick(float dt);

            const std::shared_ptr<ResourceSystem>& getResourceSystem() const;
            const std::shared_ptr<LogSystem>& getLogSystem() const;
            const std::shared_ptr<TextSystem>& getTextSystem() const;

            ///@}

            //! \name Resources
            ///@{

            Path getResourcePath(ResourcePath) const;
            Path getResourcePath(ResourcePath, const Path&) const;
            Path getResourcePath(ResourcePath, const std::string&) const;

            ///@}

            //! \name Logging
            ///@{

            void log(const std::string& prefix, const std::string& message, LogLevel = LogLevel::Information);

            ///@}

            const std::shared_ptr<UndoStack> & getUndoStack() const;

        private:
            void _addSystem(const std::shared_ptr<ISystem>&);

            DJV_PRIVATE();

            friend class ISystem;
        };

    } // namespace Core
} // namespace djv

#include <djvCore/ContextInline.h>

