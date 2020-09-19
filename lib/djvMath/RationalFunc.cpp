// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvMath/RationalFunc.h>

#include <djvCore/StringFunc.h>

namespace djv
{
    std::ostream& operator << (std::ostream& os, const Math::Rational& value)
    {
        os << value.getNum() << '/' << value.getDen();
        return os;
    }

    std::istream& operator >> (std::istream& is, Math::Rational& value)
    {
        try
        {
            is.exceptions(std::istream::failbit | std::istream::badbit);
            std::string s;
            is >> s;
            const auto split = Core::String::split(s, '/');
            if (2 == split.size())
            {
                value = Math::Rational(std::stoi(split[0]), std::stoi(split[1]));
            }
            else
            {
                throw std::exception();
            }
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return is;
    }

    rapidjson::Value toJSON(const Math::Rational& value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, Math::Rational& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

