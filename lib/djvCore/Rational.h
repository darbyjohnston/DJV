// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/RapidJSON.h>

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
                Rational() noexcept;
                explicit Rational(int num, int den = 1) noexcept;

                int getNum() const noexcept;
                int getDen() const noexcept;
                bool isValid() const noexcept;

                float toFloat() const noexcept;
                static Rational fromFloat(float);

                Rational swap() const noexcept;

                bool operator == (const Rational&) const noexcept;
                bool operator != (const Rational&) const noexcept;

            private:
                int _num = 0;
                int _den = 0;
            };

        } // namespace Math
    } // namespace Core

    std::ostream& operator << (std::ostream&, const Core::Math::Rational&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Core::Math::Rational&);

    rapidjson::Value toJSON(const Core::Math::Rational&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Core::Math::Rational&);

} // namespace djv

