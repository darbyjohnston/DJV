// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/RapidJSON.h>

#include <sstream>

namespace djv
{
    namespace Math
    {
        //! This class provides a rational number.
        template<typename T>
        class Rational
        {
        public:
            Rational() noexcept;
            explicit Rational(T num, T den = 1) noexcept;

            //! \name Information
            ///@{

            T getNum() const noexcept;
            T getDen() const noexcept;
            bool isValid() const noexcept;

            ///@}

            //! \name Conversion
            ///@{

            float toFloat() const noexcept;

            static Rational<T> fromFloat(float);

            Rational<T> swap() const noexcept;

            ///@}

            bool operator == (const Rational&) const noexcept;
            bool operator != (const Rational&) const noexcept;

        private:
            T _num = static_cast<T>(0);
            T _den = static_cast<T>(0);
        };

        //! This typedef provides an integer rational.
        typedef Rational<int> IntRational;

    } // namespace Math
    
    template<typename T>
    Math::Rational<T> operator * (const Math::Rational<T>&, const Math::Rational<T>&);

    std::ostream& operator << (std::ostream&, const Math::IntRational&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Math::IntRational&);

    rapidjson::Value toJSON(const Math::IntRational&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Math::IntRational&);
    
} // namespace djv

#include <djvMath/RationalInline.h>
