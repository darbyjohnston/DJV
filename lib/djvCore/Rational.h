// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/PicoJSON.h>

#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace Math
        {
            //! This class provides a rational number.
            class Rational
            {
            public:
                Rational();
                Rational(int num, int den);

                int getNum() const;
                int getDen() const;
                bool isValid() const;

                float toFloat() const;
                static Rational fromFloat(float);

                Rational swap() const;

                bool operator == (const Rational&) const;
                bool operator != (const Rational&) const;

            private:
                int _num = 0;
                int _den = 0;
            };

        } // namespace Math
    } // namespace Core

    picojson::value toJSON(const Core::Math::Rational&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, Core::Math::Rational&);

    std::ostream & operator << (std::ostream &, const Core::Math::Rational&);
    std::istream & operator >> (std::istream &, Core::Math::Rational&);

} // namespace djv

#include <djvCore/RationalInline.h>
