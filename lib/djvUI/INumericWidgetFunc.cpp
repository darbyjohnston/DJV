// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/INumericWidget.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Numeric
        {
            Key glfwToKey(int key)
            {
                Key out = Key::None;
                switch (key)
                {
                case GLFW_KEY_HOME:      out = Key::Home;     break;
                case GLFW_KEY_END:       out = Key::End;      break;
                case GLFW_KEY_UP:        out = Key::Up;       break;
                case GLFW_KEY_RIGHT:     out = Key::Right;    break;
                case GLFW_KEY_DOWN:      out = Key::Down;     break;
                case GLFW_KEY_LEFT:      out = Key::Left;     break;
                case GLFW_KEY_PAGE_UP:   out = Key::PageUp;   break;
                case GLFW_KEY_PAGE_DOWN: out = Key::PageDown; break;
                default: break;
                }
                return out;
            }

        } // namespace Numeric
    } // namespace UI
} // namespace djv
