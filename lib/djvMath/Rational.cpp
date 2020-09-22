// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvMath/Rational.h>

#include <math.h>

namespace djv
{
    namespace Math
    {
        Rational::Rational() noexcept
        {}

        Rational::Rational(int num, int den) noexcept :
            _num(num),
            _den(den)
        {}

    } // namespace Math
} // namespace djv

