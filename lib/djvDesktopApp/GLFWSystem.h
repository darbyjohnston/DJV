// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>

#include <djvMath/BBox.h>

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
            Math::BBox2i geometry       = Math::BBox2i(0, 0, 0, 0);
            glm::ivec2   physicalSizeMM = glm::ivec2(0, 0);

            bool operator == (const MonitorInfo&) const;
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
            static std::shared_ptr<GLFWSystem> create(const std::shared_ptr<System::Context>&);

            //! \name Monitors
            ///@{

            std::shared_ptr<Core::Observer::IListSubject<MonitorInfo> > observeMonitorInfo() const;

            ///@}
            /// 
            //! \name Cursor
            ///@{

            bool isCursorVisible() const;

            void showCursor();
            void hideCursor();

            ///@}

        private:
            DJV_PRIVATE();
        };

    } // namespace Desktop
} // namespace djv
