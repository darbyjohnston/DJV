// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Color.h>

#include <djvCore/Math.h>

#include <iomanip>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Image
        {
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

            void Color::rgbToHSV(const float in[3], float out[3])
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

            void Color::hsvToRGB(const float in[3], float out[3])
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

            std::string Color::getLabel(const Color& value, int precision, bool pad)
            {
                std::stringstream ss;
                ss.precision(precision);
                const Type type = value.getType();
                const uint8_t channelCount = AV::Image::getChannelCount(type);
                switch (AV::Image::getDataType(type))
                {
                case AV::Image::DataType::U8:
                {
                    for (uint8_t i = 0; i < channelCount; ++i)
                    {
                        if (pad)
                        {
                            ss << std::setfill(' ') << std::setw(3);
                        }
                        ss << static_cast<uint32_t>(value.getU8(i));
                        if (i < channelCount - 1)
                        {
                            ss << " ";
                        }
                    }
                    break;
                }
                case AV::Image::DataType::U10:
                {
                    for (uint8_t i = 0; i < channelCount; ++i)
                    {
                        if (pad)
                        {
                            ss << std::setfill(' ') << std::setw(4);
                        }
                        ss << std::fixed << value.getU10(i);
                        if (i < channelCount - 1)
                        {
                            ss << " ";
                        }
                    }
                    break;
                }
                case AV::Image::DataType::U16:
                {
                    for (uint8_t i = 0; i < channelCount; ++i)
                    {
                        if (pad)
                        {
                            ss << std::setfill(' ') << std::setw(5);
                        }
                        ss << std::fixed << value.getU16(i);
                        if (i < channelCount - 1)
                        {
                            ss << " ";
                        }
                    }
                    break;
                }
                case AV::Image::DataType::U32:
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
                case AV::Image::DataType::F16:
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
                case AV::Image::DataType::F32:
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
    } // namespace AV
} // namespace djv

