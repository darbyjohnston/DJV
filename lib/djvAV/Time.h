// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/Rational.h>

#include <djvCore/Core.h>

#include <chrono>

namespace djv
{
    namespace AV
    {
        //! This namespace provides time functionality.
        namespace Time
        {
            //! This enumeration provides time units.
            enum class Units
            {
                Timecode,
                Frames,

                Count,
                First = Timecode
            };

        } // namespace Time
    } // namespace AV
} // namespace djv

