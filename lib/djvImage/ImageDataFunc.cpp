// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/ImageDataFunc.h>

#include <djvImage/ImageData.h>

namespace djv
{
    std::ostream& operator << (std::ostream& s, const Image::Size& value)
    {
        s << value.w << " ";
        s << value.h;
        return s;
    }

    std::istream& operator >> (std::istream& s, Image::Size& value)
    {
        try
        {
            s.exceptions(std::istream::failbit | std::istream::badbit);
            s >> value.w;
            s >> value.h;
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return s;
    }

    rapidjson::Value toJSON(const Image::Size& value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(const Image::Mirror& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("X", toJSON(value.x, allocator), allocator);
            out.AddMember("Y", toJSON(value.y, allocator), allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, Image::Size& out)
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

    void fromJSON(const rapidjson::Value& value, Image::Mirror& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("X", i.name.GetString()))
                {
                    fromJSON(i.value, out.x);
                }
                else if (0 == strcmp("Y", i.name.GetString()))
                {
                    fromJSON(i.value, out.y);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

