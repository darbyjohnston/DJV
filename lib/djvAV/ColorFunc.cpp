// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAV/ColorFunc.h>

using namespace djv::Core;

namespace djv
{
    std::ostream& operator << (std::ostream& os, const AV::Image::Color& value)
    {
        const auto type = value.getType();
        os << type;
        const uint8_t channelCount = AV::Image::getChannelCount(type);
        switch (AV::Image::getDataType(type))
        {
        case AV::Image::DataType::U8:
        {
            os << " ";
            for (uint8_t i = 0; i < channelCount; ++i)
            {
                os << static_cast<uint32_t>(value.getU8(i));
                if (i < channelCount - 1)
                {
                    os << " ";
                }
            }
            break;
        }
        case AV::Image::DataType::U10:
        {
            os << " ";
            for (uint8_t i = 0; i < channelCount; ++i)
            {
                os << value.getU10(i);
                if (i < channelCount - 1)
                {
                    os << " ";
                }
            }
            break;
        }
        case AV::Image::DataType::U16:
        {
            os << " ";
            for (uint8_t i = 0; i < channelCount; ++i)
            {
                os << value.getU16(i);
                if (i < channelCount - 1)
                {
                    os << " ";
                }
            }
            break;
        }
        case AV::Image::DataType::U32:
        {
            os << " ";
            for (uint8_t i = 0; i < channelCount; ++i)
            {
                os << value.getU32(i);
                if (i < channelCount - 1)
                {
                    os << " ";
                }
            }
            break;
        }
        case AV::Image::DataType::F16:
        {
            os << " ";
            for (uint8_t i = 0; i < channelCount; ++i)
            {
                os << value.getF16(i);
                if (i < channelCount - 1)
                {
                    os << " ";
                }
            }
            break;
        }
        case AV::Image::DataType::F32:
        {
            os << " ";
            for (uint8_t i = 0; i < channelCount; ++i)
            {
                os << value.getF32(i);
                if (i < channelCount - 1)
                {
                    os << " ";
                }
            }
            break;
        }
        default: break;
        }
        return os;
    }

    std::istream& operator >> (std::istream& is, AV::Image::Color& value)
    {
        try
        {
            is.exceptions(std::istream::failbit | std::istream::badbit);
            AV::Image::Type type = AV::Image::Type::None;
            is >> type;
            value = AV::Image::Color(type);
            const size_t channelCount = AV::Image::getChannelCount(type);
            switch (AV::Image::getDataType(type))
            {
            case AV::Image::DataType::U8:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    uint32_t tmp = 0;
                    is >> tmp;
                    value.setU8(tmp, i);
                }
                break;
            }
            case AV::Image::DataType::U10:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    AV::Image::U10_T tmp = 0;
                    is >> tmp;
                    value.setU10(tmp, i);
                }
                break;
            }
            case AV::Image::DataType::U16:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    AV::Image::U16_T tmp = 0;
                    is >> tmp;
                    value.setU16(tmp, i);
                }
                break;
            }
            case AV::Image::DataType::U32:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    AV::Image::U32_T tmp = 0;
                    is >> tmp;
                    value.setU32(tmp, i);
                }
                break;
            }
            case AV::Image::DataType::F16:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    AV::Image::F16_T tmp = 0.F;
                    is >> tmp;
                    value.setF16(tmp, i);
                }
                break;
            }
            case AV::Image::DataType::F32:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    AV::Image::F32_T tmp = 0.F;
                    is >> tmp;
                    value.setF32(tmp, i);
                }
                break;
            }
            default: break;
            }
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return is;
    }

    rapidjson::Value toJSON(const AV::Image::Color& value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, AV::Image::Color& out)
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

