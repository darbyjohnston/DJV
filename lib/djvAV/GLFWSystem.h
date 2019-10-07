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

#include <djvAV/AV.h>

#include <djvCore/ISystem.h>

#include <stdexcept>

struct GLFWwindow;

namespace djv
{
    namespace Core
    {
        class Context;

    } // namespace Core
    
    namespace AV
    {
        namespace GLFW
        {
            //! This enumeration provides GLFW error strings.
            enum class ErrorString
            {
                Init,
                Window,
                GLAD
            };
            
            //! Get a GLFW error string.
            std::string getErrorMessage(ErrorString);
            
            //! This class provides a GLFW error.
            class Error : public std::runtime_error
            {
            public:
                explicit Error(const std::string&);
            };
        
            //! Initialize GLFW.
            //! Throws:
            //! - Error
            void init(const std::shared_ptr<Core::Context>&);
        
            //! This class provides a system for GLFW functionality.
            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);
                
            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                System();

            public:
                virtual ~System();

                //! Create a new GLFW system.
                //! Throws:
                //! - Error
                static std::shared_ptr<System> create(const std::shared_ptr<Core::Context>&);

                GLFWwindow* getGLFWWindow() const;

            private:
                DJV_PRIVATE();
            };
        
        } // namespace GLFW
    } // namespace AV
} // namespace djv

