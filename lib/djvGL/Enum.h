// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvGL/GL.h>

#include <djvCore/Enum.h>

namespace djv
{
    namespace GL
    {
        //! This enumeration provides GLFW swap intervals.
        enum class SwapInterval
        {
            Default,
            _0,
            _1,

            Count,
            First = Default
        };

    } // namespace UI
} // namespace djv

