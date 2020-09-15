// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <stdexcept>

namespace djv
{
    template<typename T>
    inline rapidjson::Value toJSON(const std::vector<T>& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kArrayType);
        for (const auto& i : value)
        {
            out.PushBack(toJSON(i, allocator), allocator);
        }
        return out;
    }
    
    template<typename T>
    inline rapidjson::Value toJSON(const std::map<std::string, T>& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        for (const auto& i : value)
        {
            out.AddMember(rapidjson::Value(i.first.c_str(), allocator), toJSON(i.second, allocator), allocator);
        }
        return out;
    }

    template<typename T>
    inline void fromJSON(const rapidjson::Value& value, std::vector<T>& out)
    {
        if (value.IsArray())
        {
            out.clear();
            for (const auto& i : value.GetArray())
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
    inline void fromJSON(const rapidjson::Value& value, std::map<std::string, T>& out)
    {
        if (value.IsObject())
        {
            out.clear();
            for (const auto& i : value.GetObject())
            {
                fromJSON(i.value, out[i.name.GetString()]);
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
