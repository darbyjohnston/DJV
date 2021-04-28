// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/Color.h>

#include <djvMath/Math.h>

#include <iomanip>

using namespace djv::Core;

namespace djv
{
    namespace Image
    {
        Color::Color()
        {}

        Color::Color(Type type) :
            _type(type),
            _data(getByteCount(_type))
        {
            zero();
        }

        Color::Color(int v) :
            _type(Type::L_U8),
            _data(getByteCount(_type))
        {
            auto p = reinterpret_cast<U8_T*>(_data.data());
            p[0] = v;
        }

        Color::Color(int r, int g, int b, int a) :
            _type(Type::RGBA_U8),
            _data(getByteCount(_type))
        {
            auto p = reinterpret_cast<U8_T*>(_data.data());
            p[0] = r;
            p[1] = g;
            p[2] = b;
            p[3] = a;
        }

        Color::Color(F32_T r, F32_T g, F32_T b, F32_T a) :
            _type(Type::RGBA_F32),
            _data(getByteCount(_type))
        {
            auto p = reinterpret_cast<F32_T*>(_data.data());
            p[0] = r;
            p[1] = g;
            p[2] = b;
            p[3] = a;
        }

        Color::Color(F32_T v) :
            _type(Type::L_F32),
            _data(getByteCount(_type))
        {
            auto p = reinterpret_cast<F32_T*>(_data.data());
            p[0] = v;
        }

        void Color::setData(const uint8_t* value)
        {
            memcpy(_data.data(), value, getByteCount(_type));
        }

        void Color::zero()
        {
            memset(_data.data(), 0, getByteCount(_type));
        }

        Color Color::convert(Type type) const
        {
            Color out(type);
            Image::convert(_data.data(), _type, out._data.data(), type, 1);
            return out;
        }

        void rgbToHSV(const float in[3], float out[3])
        {
            const float& min = std::min(in[0], std::min(in[1], in[2]));
            const float& max = std::max(in[0], std::max(in[1], in[2]));
            out[2] = max;
            out[1] = max != 0.F ? (1.F - min / max) : 0.F;
            const float v = (max - min) * 6.F;
            if (max == min)
            {
                out[0] = 0.F;
            }
            else if (in[0] == max)
            {
                if (in[1] >= in[2])
                {
                    out[0] = (in[1] - in[2]) / v;
                }
                else
                {
                    out[0] = 1.F + (in[1] - in[2]) / v;
                }
            }
            else if (in[1] == max)
            {
                out[0] = 1.F / 3.F + (in[2] - in[0]) / v;
            }
            else if (in[2] == max)
            {
                out[0] = 2.F / 3.F + (in[0] - in[1]) / v;
            }
        }

        namespace
        {
            float mod(float value, float mod)
            {
                float tmp = value;
                if (mod != 0.F)
                {
                    tmp = value - (static_cast<float>(static_cast<int>(value / mod)) * mod);
                    if (tmp < 0.F)
                    {
                        tmp += mod;
                    }
                }
                return tmp;
            }

        } // namespace

        void hsvToRGB(const float in[3], float out[3])
        {
            const float h = mod(in[0] * 6.F, 6.F);
            const int   i = static_cast<int>(floorf(h));
            const float f = h - static_cast<float>(i);
            const float p = in[2] * (1.F - in[1]);
            const float q = in[2] * (1.F - (in[1] * f));
            const float t = in[2] * (1.F - (in[1] * (1.F - f)));
            switch (i)
            {
            case 0:
                out[0] = in[2];
                out[1] = t;
                out[2] = p;
                break;
            case 1:
                out[0] = q;
                out[1] = in[2];
                out[2] = p;
                break;
            case 2:
                out[0] = p;
                out[1] = in[2];
                out[2] = t;
                break;
            case 3:
                out[0] = p;
                out[1] = q;
                out[2] = in[2];
                break;
            case 4:
                out[0] = t;
                out[1] = p;
                out[2] = in[2];
                break;
            case 5:
                out[0] = in[2];
                out[1] = p;
                out[2] = q;
                break;
            default: break;
            }
        }

        std::string getLabel(const Color& value, int precision)
        {
            std::stringstream ss;
            ss.precision(precision);
            const Type type = value.getType();
            const uint8_t channelCount = Image::getChannelCount(type);
            switch (Image::getDataType(type))
            {
            case Image::DataType::U8:
            {
                for (uint8_t i = 0; i < channelCount; ++i)
                {
                    ss << static_cast<uint32_t>(value.getU8(i));
                    if (i < channelCount - 1)
                    {
                        ss << " ";
                    }
                }
                break;
            }
            case Image::DataType::U10:
            {
                for (uint8_t i = 0; i < channelCount; ++i)
                {
                    ss << std::fixed << value.getU10(i);
                    if (i < channelCount - 1)
                    {
                        ss << " ";
                    }
                }
                break;
            }
            case Image::DataType::U16:
            {
                for (uint8_t i = 0; i < channelCount; ++i)
                {
                    ss << std::fixed << value.getU16(i);
                    if (i < channelCount - 1)
                    {
                        ss << " ";
                    }
                }
                break;
            }
            case Image::DataType::U32:
            {
                for (uint8_t i = 0; i < channelCount; ++i)
                {
                    ss << std::fixed << value.getU32(i);
                    if (i < channelCount - 1)
                    {
                        ss << " ";
                    }
                }
                break;
            }
            case Image::DataType::F16:
            {
                for (uint8_t i = 0; i < channelCount; ++i)
                {
                    ss << std::fixed << value.getF16(i);
                    if (i < channelCount - 1)
                    {
                        ss << " ";
                    }
                }
                break;
            }
            case Image::DataType::F32:
            {
                for (uint8_t i = 0; i < channelCount; ++i)
                {
                    ss << std::fixed << value.getF32(i);
                    if (i < channelCount - 1)
                    {
                        ss << " ";
                    }
                }
                break;
            }
            default: break;
            }
            return ss.str();
        }

    } // namepsace Image

    std::ostream& operator << (std::ostream& os, const Image::Color& value)
    {
        const auto type = value.getType();
        os << type;
        const uint8_t channelCount = Image::getChannelCount(type);
        switch (Image::getDataType(type))
        {
        case Image::DataType::U8:
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
        case Image::DataType::U10:
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
        case Image::DataType::U16:
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
        case Image::DataType::U32:
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
        case Image::DataType::F16:
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
        case Image::DataType::F32:
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

    std::istream& operator >> (std::istream& is, Image::Color& value)
    {
        try
        {
            is.exceptions(std::istream::failbit | std::istream::badbit);
            Image::Type type = Image::Type::None;
            is >> type;
            value = Image::Color(type);
            const size_t channelCount = Image::getChannelCount(type);
            switch (Image::getDataType(type))
            {
            case Image::DataType::U8:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    uint32_t tmp = 0;
                    is >> tmp;
                    value.setU8(tmp, i);
                }
                break;
            }
            case Image::DataType::U10:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    Image::U10_T tmp = 0;
                    is >> tmp;
                    value.setU10(tmp, i);
                }
                break;
            }
            case Image::DataType::U16:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    Image::U16_T tmp = 0;
                    is >> tmp;
                    value.setU16(tmp, i);
                }
                break;
            }
            case Image::DataType::U32:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    Image::U32_T tmp = 0;
                    is >> tmp;
                    value.setU32(tmp, i);
                }
                break;
            }
            case Image::DataType::F16:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    Image::F16_T tmp = 0.F;
                    is >> tmp;
                    value.setF16(tmp, i);
                }
                break;
            }
            case Image::DataType::F32:
            {
                for (size_t i = 0; i < channelCount; ++i)
                {
                    Image::F32_T tmp = 0.F;
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

    rapidjson::Value toJSON(const Image::Color& value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, Image::Color& out)
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

