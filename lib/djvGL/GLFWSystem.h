// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvGL/Enum.h>

#include <djvSystem/ISystem.h>

#include <djvCore/ValueObserver.h>

#include <stdexcept>

struct GLFWwindow;

namespace djv
{
    namespace GL
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
            
            //! This class provides a GLFW error.
            class Error : public std::runtime_error
            {
            public:
                explicit Error(const std::string&);
            };
        
            //! This class provides a system for GLFW functionality.
            class GLFWSystem : public System::ISystem
            {
                DJV_NON_COPYABLE(GLFWSystem);
                
            protected:
                void _init(const std::shared_ptr<System::Context>&);
                GLFWSystem();

            public:
                ~GLFWSystem() override;

                //! Create a new GLFW system.
                //! Throws:
                //! - Error
                static std::shared_ptr<GLFWSystem> create(const std::shared_ptr<System::Context>&);

                GLFWwindow* getGLFWWindow() const;

                std::shared_ptr<Core::IValueSubject<SwapInterval> > observeSwapInterval() const;

                void setSwapInterval(SwapInterval);

            private:
                DJV_PRIVATE();
            };
        
        } // namespace GLFW
    } // namespace GL
} // namespace djv

