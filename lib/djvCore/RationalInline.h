// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace Math
        {
            inline int Rational::getNum() const
            {
                return _num;
            }

            inline int Rational::getDen() const
            {
                return _den;
            }

            inline bool Rational::isValid() const
            {
                return _num != 0 && _den != 0;
            }

            inline Rational Rational::swap() const
            {
                return Rational(_den, _num);
            }

        } // namespace Math
    } // namespace Core
} // namespace djv
