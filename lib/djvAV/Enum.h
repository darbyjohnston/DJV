// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AV.h>

#include <djvCore/Enum.h>

namespace djv
{
    namespace AV
    {
        //! This enumeration provides sides.
        enum class Side
        {
            None,
            Left,
            Top,
            Right,
            Bottom,

            Count,
            First = Left
        };

        //! This enumeration provides GLFW swap intervals.
        enum class SwapInterval
        {
            Default,
            _0,
            _1,

            Count,
            First = Default
        };

        //! This enumeration provides how the alpha channel is used for blending.
        //!
        //! References:
        //! - https://microsoft.github.io/Win2D/html/PremultipliedAlpha.htm
        enum class AlphaBlend
        {
            None,
            Straight,
            Premultiplied,

            Count,
            First = None
        };
    } // namespace UI
} // namespace djv

