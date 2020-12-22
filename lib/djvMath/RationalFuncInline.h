// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Math
    {
        template<typename T>
        inline Math::Rational<T> operator * (const Math::Rational<T>& a, const Math::Rational<T>& b)
        {
            return Math::Rational(a.getNum() * b.getNum(), a.getDen() * b.getDen());
        }

    } // namespace Math
} // namespace djv
