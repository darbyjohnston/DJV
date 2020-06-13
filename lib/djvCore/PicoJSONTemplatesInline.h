// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    template<typename T>
    inline picojson::value toJSON(const std::vector<T>& value)
    {
        picojson::value out(picojson::array_type, true);
        for (const auto& i : value)
        {
            out.get<picojson::array>().push_back(toJSON(i));
        }
        return out;
    }
    
    template<typename T>
    inline picojson::value toJSON(const std::map<std::string, T>& value)
    {
        picojson::value out(picojson::object_type, true);
        for (auto i : value)
        {
            out.get<picojson::object>()[i.first] = toJSON(i.second);
        }
        return out;
    }

    template<typename T>
    inline void fromJSON(const picojson::value& value, std::vector<T>& out)
    {
        if (value.is<picojson::array>())
        {
            out.clear();
            for (const auto& i : value.get<picojson::array>())
            {
                T tmp;
                fromJSON(i, tmp);
                out.push_back(tmp);
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }
    
    template<typename T>
    inline void fromJSON(const picojson::value& value, std::map<std::string, T>& out)
    {
        if (value.is<picojson::object>())
        {
            out.clear();
            for (const auto& i : value.get<picojson::object>())
            {
                fromJSON(i.second, out[i.first]);
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
