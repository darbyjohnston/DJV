// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

namespace djv
{
    namespace Math
    {
        //! This class provides a rational number.
        class Rational
        {
        public:
            Rational() noexcept;
            explicit Rational(int num, int den = 1) noexcept;

            //! \name Information
            ///@{

            int getNum() const noexcept;
            int getDen() const noexcept;
            bool isValid() const noexcept;

            ///@}

            //! \name Conversion
            ///@{

            float toFloat() const noexcept;

            static Rational fromFloat(float);

            Rational swap() const noexcept;

            ///@}

            bool operator == (const Rational&) const noexcept;
            bool operator != (const Rational&) const noexcept;

        private:
            int _num = 0;
            int _den = 0;
        };

    } // namespace Math
} // namespace djv

#include <djvMath/RationalInline.h>
