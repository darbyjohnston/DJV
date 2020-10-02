// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/INumericWidget.h>

namespace djv
{
    namespace UI
    {
        namespace Numeric
        {
            //! \name Conversion
            ///@{

            //! Convert a GLFW key to a numeric widget keyboard shortcut.
            Key glfwToKey(int);

            ///@}

        } // namespace Numeric
    } // namespace UI
} // namespace djv
