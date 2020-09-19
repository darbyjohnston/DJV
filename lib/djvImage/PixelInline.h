// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvMath/MathFunc.h>

namespace djv
{
    namespace Image
    {
        constexpr bool U10_S_MSB::operator == (const U10_S_MSB& value) const noexcept
        {
            return
                value.r == r &&
                value.g == g &&
                value.b == b;
        }

        constexpr bool U10_S_MSB::operator != (const U10_S_MSB& value) const noexcept
        {
            return !(*this == value);
        }

        constexpr bool U10_S_LSB::operator == (const U10_S_LSB& value) const noexcept
        {
            return
                value.r == r &&
                value.g == g &&
                value.b == b;
        }

        constexpr bool U10_S_LSB::operator != (const U10_S_LSB& value) const noexcept
        {
            return !(*this == value);
        }

    } // namespace Image
} // namespace djv

