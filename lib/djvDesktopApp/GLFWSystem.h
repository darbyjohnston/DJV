// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

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
