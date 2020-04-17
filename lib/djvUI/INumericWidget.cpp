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
        NumericWidgetKey fromGLFWKey(int key)
        {
            NumericWidgetKey out = NumericWidgetKey::None;
            switch (key)
            {
            case GLFW_KEY_HOME:         out = NumericWidgetKey::Home;       break;
            case GLFW_KEY_END:          out = NumericWidgetKey::End;        break;
            case GLFW_KEY_UP:           out = NumericWidgetKey::Up;         break;
            case GLFW_KEY_RIGHT:        out = NumericWidgetKey::Right;      break;
            case GLFW_KEY_DOWN:         out = NumericWidgetKey::Down;       break;
            case GLFW_KEY_LEFT:         out = NumericWidgetKey::Left;       break;
            case GLFW_KEY_PAGE_UP:      out = NumericWidgetKey::PageUp;     break;
            case GLFW_KEY_PAGE_DOWN:    out = NumericWidgetKey::PageDown;   break;
            default: break;
            }
            return out;
        }

    } // namespace UI
} // namespace djv
