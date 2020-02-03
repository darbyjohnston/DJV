//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvDesktopApp/DesktopApp.h>

#include <djvCore/BBox.h>
#include <djvCore/ISystem.h>
#include <djvCore/ListObserver.h>

struct GLFWmonitor;
struct GLFWwindow;

namespace djv
{
    namespace Desktop
    {
        //! This struct provides information about monitors.
        struct MonitorInfo
        {
            std::string  name;
            Core::BBox2i geometry       = Core::BBox2i(0, 0, 0, 0);
            glm::ivec2   physicalSizeMM = glm::ivec2(0, 0);

            bool operator == (const MonitorInfo&) const;
        };

            //! This class provides a system for GLFW functionality.
        class GLFWSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(GLFWSystem);
            
        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            GLFWSystem();

        public:
            virtual ~GLFWSystem();
            
            //! Create a new GLFW system.
            static std::shared_ptr<GLFWSystem> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::IListSubject<MonitorInfo> > observeMonitorInfo() const;

            bool isCursorVisible() const;
            void showCursor();
            void hideCursor();

        private:
            DJV_PRIVATE();
        };

    } // namespace Desktop
} // namespace djv
