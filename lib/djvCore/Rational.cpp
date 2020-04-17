// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Rational.h>

#include <djvCore/String.h>

#include <math.h>

namespace djv
{
    namespace Core
    {
        namespace Math
        {
            Rational::Rational()
            {}

            Rational::Rational(int num, int den) :
                _num(num),
                _den(den)
            {}

            float Rational::toFloat() const
            {
                return _num / static_cast<float>(_den);
            }

            Rational Rational::fromFloat(float value)
            {
                //! \todo Implement a proper floating-point to rational number conversion.
                //! Check-out: OpenEXR\IlmImf\ImfRational.h
                return Rational(static_cast<int>(round(value)), 1);
            }

            bool Rational::operator == (const Rational& other) const
            {
                return _num == other._num && _den == other._den;
            }

            bool Rational::operator != (const Rational& other) const
            {
                return !(*this == other);
            }

        } // namespace Math
    } // namespace Core

    picojson::value toJSON(const Core::Math::Rational& value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    void fromJSON(const picojson::value& value, Core::Math::Rational& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    std::ostream & operator << (std::ostream & os, const Core::Math::Rational& value)
    {
        os << value.getNum() << '/' << value.getDen();
        return os;
    }

    std::istream & operator >> (std::istream & is, Core::Math::Rational& value)
    {
        is.exceptions(std::istream::failbit | std::istream::badbit);
        std::string s;
        is >> s;
        const auto split = Core::String::split(s, '/');
        if (2 == split.size())
        {
            value = Core::Math::Rational(std::stoi(split[0]), std::stoi(split[1]));
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return is;
    }

} // namespace djv

