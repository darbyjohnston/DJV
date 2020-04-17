// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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

