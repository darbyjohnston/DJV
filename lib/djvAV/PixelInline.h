// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/MathFunc.h>
#include <djvCore/MemoryFunc.h>

#include <array>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            constexpr bool U10_S_MSB::operator == (const U10_S_MSB& value) const noexcept
            {
                return
                    value.r == r &&
                    value.g == g &&
                    value.b == b;
            }

            constexpr bool U10_S_MSB::operator != (const U10_S_MSB& value) const noexcept
            {
                return !(*this == value);
            }

            constexpr bool U10_S_LSB::operator == (const U10_S_LSB& value) const noexcept
            {
                return
                    value.r == r &&
                    value.g == g &&
                    value.b == b;
            }

            constexpr bool U10_S_LSB::operator != (const U10_S_LSB& value) const noexcept
            {
                return !(*this == value);
            }

            inline Channels getChannels(Type value) noexcept
            {
                const std::array<Channels, static_cast<size_t>(Type::Count)> data =
                {
                    Channels::None,
                    
                    Channels::L,
                    Channels::L,
                    Channels::L,
                    Channels::L,
                    Channels::L,

                    Channels::LA,
                    Channels::LA,
                    Channels::LA,
                    Channels::LA,
                    Channels::LA,

                    Channels::RGB,
                    Channels::RGB,
                    Channels::RGB,
                    Channels::RGB,
                    Channels::RGB,
                    Channels::RGB,

                    Channels::RGBA,
                    Channels::RGBA,
                    Channels::RGBA,
                    Channels::RGBA,
                    Channels::RGBA
                };
                return data[static_cast<size_t>(value)];
            }

            inline uint8_t getChannelCount(Type value) noexcept
            {
                const std::array<uint8_t, static_cast<size_t>(Type::Count)> data =
                {
                    0,
                    1, 1, 1, 1, 1,
                    2, 2, 2, 2, 2,
                    3, 3, 3, 3, 3, 3,
                    4, 4, 4, 4, 4
                };
                return data[static_cast<size_t>(value)];
            }

            inline DataType getDataType(Type value) noexcept
            {
                const std::array<DataType, static_cast<size_t>(Type::Count)> data =
                {
                    DataType::None,

                    DataType::U8,
                    DataType::U16,
                    DataType::U32,
                    DataType::F16,
                    DataType::F32,

                    DataType::U8,
                    DataType::U16,
                    DataType::U32,
                    DataType::F16,
                    DataType::F32,

                    DataType::U8,
                    DataType::U10,
                    DataType::U16,
                    DataType::U32,
                    DataType::F16,
                    DataType::F32,

                    DataType::U8,
                    DataType::U16,
                    DataType::U32,
                    DataType::F16,
                    DataType::F32
                };
                return data[static_cast<size_t>(value)];
            }

            inline uint8_t getBitDepth(Type value) noexcept
            {
                const std::array<uint8_t, static_cast<size_t>(Type::Count)> data =
                {
                    0,
                    8, 16, 32, 16, 32,
                    8, 16, 32, 16, 32,
                    8, 10, 16, 32, 16, 32,
                    8, 16, 32, 16, 32
                };
                return data[static_cast<size_t>(value)];
            }

            inline uint8_t getBitDepth(DataType value) noexcept
            {
                const std::array<uint8_t, static_cast<size_t>(DataType::Count)> data =
                {
                    0,
                    8,
                    10,
                    16,
                    32,
                    16,
                    32
                };
                return data[static_cast<size_t>(value)];
            }

            inline size_t getByteCount(Type value) noexcept
            {
                const std::array<size_t, static_cast<size_t>(Type::Count)> data =
                {
                    0,
                    1, 2, 4, 2, 4,
                    2, 4, 8, 4, 8,
                    3, 4, 6, 12, 6, 12,
                    4, 8, 16, 8, 16
                };
                return data[static_cast<size_t>(value)];
            }

            inline size_t getByteCount(DataType value) noexcept
            {
                const std::array<size_t, static_cast<size_t>(DataType::Count)> data =
                {
                    0,
                    1,
                    2,
                    2,
                    4,
                    2,
                    4
                };
                return data[static_cast<size_t>(value)];
            }

            inline bool isIntType(Type value) noexcept
            {
                const std::array<bool, static_cast<size_t>(Type::Count)> data =
                {
                    false,
                    true, true, true, false, false,
                    true, true, true, false, false,
                    true, true, true, true, false, false,
                    true, true, true, false, false,
                };
                return data[static_cast<size_t>(value)];
            }

            inline bool isFloatType(Type value) noexcept
            {
                const std::array<bool, static_cast<size_t>(Type::Count)> data =
                {
                    false,
                    false, false, false, true, true,
                    false, false, false, true, true,
                    false, false, false, false, true, true,
                    false, false, false, true, true
                };
                return data[static_cast<size_t>(value)];
            }

            inline Core::IntRange getIntRange(Type value)
            {
                using namespace Core;
                const std::array<IntRange, static_cast<size_t>(Type::Count)> data =
                {
                    IntRange(0, 0),

                    IntRange(U8Range.getMin(), U8Range.getMax()),
                    IntRange(U16Range.getMin(), U16Range.getMax()),
                    IntRange(U32Range.getMin(), U32Range.getMax()),
                    IntRange(0, 0),
                    IntRange(0, 0),

                    IntRange(U8Range.getMin(), U8Range.getMax()),
                    IntRange(U16Range.getMin(), U16Range.getMax()),
                    IntRange(U32Range.getMin(), U32Range.getMax()),
                    IntRange(0, 0),
                    IntRange(0, 0),

                    IntRange(U8Range.getMin(), U8Range.getMax()),
                    IntRange(U10Range.getMin(), U10Range.getMax()),
                    IntRange(U16Range.getMin(), U16Range.getMax()),
                    IntRange(U32Range.getMin(), U32Range.getMax()),
                    IntRange(0, 0),
                    IntRange(0, 0),

                    IntRange(U8Range.getMin(), U8Range.getMax()),
                    IntRange(U16Range.getMin(), U16Range.getMax()),
                    IntRange(U32Range.getMin(), U32Range.getMax()),
                    IntRange(0, 0),
                    IntRange(0, 0),
                };
                return data[static_cast<size_t>(value)];
            }

            inline Core::FloatRange getFloatRange(Type value)
            {
                using namespace Core;
                const std::array<FloatRange, static_cast<size_t>(Type::Count)> data =
                {
                    FloatRange(0.F, 0.F),

                    FloatRange(0.F, 0.F),
                    FloatRange(0.F, 0.F),
                    FloatRange(0.F, 0.F),
                    FloatRange(F16Range.getMin(), F16Range.getMax()),
                    FloatRange(F32Range.getMin(), F32Range.getMax()),

                    FloatRange(0.F, 0.F),
                    FloatRange(0.F, 0.F),
                    FloatRange(0.F, 0.F),
                    FloatRange(F16Range.getMin(), F16Range.getMax()),
                    FloatRange(F32Range.getMin(), F32Range.getMax()),

                    FloatRange(0.F, 0.F),
                    FloatRange(0.F, 0.F),
                    FloatRange(0.F, 0.F),
                    FloatRange(0.F, 0.F),
                    FloatRange(F16Range.getMin(), F16Range.getMax()),
                    FloatRange(F32Range.getMin(), F32Range.getMax()),

                    FloatRange(0.F, 0.F),
                    FloatRange(0.F, 0.F),
                    FloatRange(0.F, 0.F),
                    FloatRange(F16Range.getMin(), F16Range.getMax()),
                    FloatRange(F32Range.getMin(), F32Range.getMax()),
                };
                return data[static_cast<size_t>(value)];
            }

            inline Type getIntType(uint8_t channelCount, uint8_t bitDepth) noexcept
            {
                switch (channelCount)
                {
                case 1:
                    switch (bitDepth)
                    {
                    case  8: return Type::L_U8;
                    case 16: return Type::L_U16;
                    case 32: return Type::L_U32;
                    default: break;
                    }
                    break;
                case 2:
                    switch (bitDepth)
                    {
                    case  8: return Type::LA_U8;
                    case 16: return Type::LA_U16;
                    case 32: return Type::LA_U32;
                    default: break;
                    }
                    break;
                case 3:
                    switch (bitDepth)
                    {
                    case  8: return Type::RGB_U8;
                    case 10: return Type::RGB_U10;
                    case 16: return Type::RGB_U16;
                    case 32: return Type::RGB_U32;
                    default: break;
                    }
                    break;
                case 4:
                    switch (bitDepth)
                    {
                    case  8: return Type::RGBA_U8;
                    case 16: return Type::RGBA_U16;
                    case 32: return Type::RGBA_U32;
                    default: break;
                    }
                    break;
                default: break;
                }
                return Type::None;
            }

            inline Type getFloatType(uint8_t channelCount, uint8_t bitDepth) noexcept
            {
                switch (channelCount)
                {
                case 1:
                    switch (bitDepth)
                    {
                    case 16: return Type::L_F16;
                    case 32: return Type::L_F32;
                    default: break;
                    }
                    break;
                case 2:
                    switch (bitDepth)
                    {
                    case 16: return Type::LA_F16;
                    case 32: return Type::LA_F32;
                    default: break;
                    }
                    break;
                case 3:
                    switch (bitDepth)
                    {
                    case 16: return Type::RGB_F16;
                    case 32: return Type::RGB_F32;
                    default: break;
                    }
                    break;
                case 4:
                    switch (bitDepth)
                    {
                    case 16: return Type::RGBA_F16;
                    case 32: return Type::RGBA_F32;
                    default: break;
                    }
                    break;
                default: break;
                }
                return Type::None;
            }

            inline GLenum getGLFormat(Type value) noexcept
            {
                const std::array<GLenum, static_cast<size_t>(Type::Count)> data =
                {
                    GL_NONE,

#if defined(DJV_OPENGL_ES2)
                    GL_LUMINANCE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,

                    GL_LUMINANCE_ALPHA,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    
                    GL_RGB,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,

                    GL_RGBA,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE
#else // DJV_OPENGL_ES2
                    GL_RED,
                    GL_RED,
                    GL_RED,
                    GL_RED,
                    GL_RED,

                    GL_RG,
                    GL_RG,
                    GL_RG,
                    GL_RG,
                    GL_RG,

                    GL_RGB,
                    GL_RGBA,
                    GL_RGB,
                    GL_RGB,
                    GL_RGB,
                    GL_RGB,

                    GL_RGBA,
                    GL_RGBA,
                    GL_RGBA,
                    GL_RGBA,
                    GL_RGBA
#endif // DJV_OPENGL_ES2
                };
                return data[static_cast<size_t>(value)];
            }

            inline GLenum getGLType(Type value) noexcept
            {
                const std::array<GLenum, static_cast<size_t>(Type::Count)> data =
                {
                    GL_NONE,

#if defined(DJV_OPENGL_ES2)
                    GL_UNSIGNED_BYTE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,

                    GL_UNSIGNED_BYTE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,

                    GL_UNSIGNED_BYTE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,

                    GL_UNSIGNED_BYTE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
                    GL_NONE,
#else // DJV_OPENGL_ES2
                    GL_UNSIGNED_BYTE,
                    GL_UNSIGNED_SHORT,
                    GL_UNSIGNED_INT,
                    GL_HALF_FLOAT,
                    GL_FLOAT,

                    GL_UNSIGNED_BYTE,
                    GL_UNSIGNED_SHORT,
                    GL_UNSIGNED_INT,
                    GL_HALF_FLOAT,
                    GL_FLOAT,

                    GL_UNSIGNED_BYTE,
                    GL_UNSIGNED_INT_10_10_10_2,
                    GL_UNSIGNED_SHORT,
                    GL_UNSIGNED_INT,
                    GL_HALF_FLOAT,
                    GL_FLOAT,

                    GL_UNSIGNED_BYTE,
                    GL_UNSIGNED_SHORT,
                    GL_UNSIGNED_INT,
                    GL_HALF_FLOAT,
                    GL_FLOAT
#endif // DJV_OPENGL_ES2
                };
                return data[static_cast<size_t>(value)];
            }

            inline void convert_U8_U8(U8_T in, U8_T& out)
            {
                out = in;
            }

            inline void convert_U8_U10(U8_T in, U10_T& out)
            {
                out = in << 2;
            }

            inline void convert_U8_U16(U8_T in, U16_T& out)
            {
                out = in << 8;
            }

            inline void convert_U8_U32(U8_T in, U32_T& out)
            {
                out = in << 24;
            }

            inline void convert_U8_F16(U8_T in, F16_T& out)
            {
                out = in / static_cast<float>(U8Range.getMax());
            }

            inline void convert_U8_F32(U8_T in, F32_T& out)
            {
                out = in / static_cast<float>(U8Range.getMax());
            }

            inline void convert_U10_U8(U10_T in, U8_T& out)
            {
                out = in >> 2;
            }

            inline void convert_U10_U8(U10_T in, U10_T& out)
            {
                out = in;
            }

            inline void convert_U10_U16(U10_T in, U16_T& out)
            {
                out = in << 6;
            }

            inline void convert_U10_U32(U10_T in, U32_T& out)
            {
                out = in << 22;
            }

            inline void convert_U10_F16(U10_T in, F16_T& out)
            {
                out = in / static_cast<float>(U10Range.getMax());
            }

            inline void convert_U10_F32(U10_T in, F32_T& out)
            {
                out = in / static_cast<float>(U10Range.getMax());
            }

            inline void convert_U16_U8(U16_T in, U8_T& out)
            {
                out = in >> 8;
            }

            inline void convert_U16_U10(U16_T in, U10_T& out)
            {
                out = in >> 6;
            }

            inline void convert_U16_U16(U16_T in, U16_T& out)
            {
                out = in;
            }

            inline void convert_U16_U32(U16_T in, U32_T& out)
            {
                out = in << 16;
            }

            inline void convert_U16_F16(U16_T in, F16_T& out)
            {
                out = in / static_cast<float>(U16Range.getMax());
            }

            inline void convert_U16_F32(U16_T in, F32_T& out)
            {
                out = in / static_cast<float>(U16Range.getMax());
            }

            inline void convert_U32_U8(U32_T in, U8_T& out)
            {
                out = in >> 24;
            }

            inline void convert_U32_U10(U32_T in, U10_T& out)
            {
                out = in >> 22;
            }

            inline void convert_U32_U16(U32_T in, U16_T& out)
            {
                out = in >> 16;
            }

            inline void convert_U32_U32(U32_T in, U32_T& out)
            {
                out = in;
            }

            inline void convert_U32_F16(U32_T in, F16_T& out)
            {
                out = in / static_cast<float>(U32Range.getMax());
            }

            inline void convert_U32_F32(U32_T in, F32_T& out)
            {
                out = in / static_cast<float>(U32Range.getMax());
            }

            inline void convert_F16_U8(F16_T in, U8_T& out)
            {
                out = static_cast<U8_T>(Core::Math::clamp(
                    static_cast<uint16_t>(in * U8Range.getMax()),
                    static_cast<uint16_t>(U8Range.getMin()),
                    static_cast<uint16_t>(U8Range.getMax())));
            }

            inline void convert_F16_U10(F16_T in, U10_T& out)
            {
                out = static_cast<U10_T>(Core::Math::clamp(
                    static_cast<uint16_t>(in * U10Range.getMax()),
                    static_cast<uint16_t>(U10Range.getMin()),
                    static_cast<uint16_t>(U10Range.getMax())));
            }

            inline void convert_F16_U16(F16_T in, U16_T& out)
            {
                out = static_cast<U16_T>(Core::Math::clamp(
                    static_cast<uint32_t>(in * U16Range.getMax()),
                    static_cast<uint32_t>(U16Range.getMin()),
                    static_cast<uint32_t>(U16Range.getMax())));
            }

            inline void convert_F16_U32(F16_T in, U32_T& out)
            {
                out = static_cast<U32_T>(Core::Math::clamp(
                    static_cast<uint64_t>(static_cast<double>(in) * static_cast<uint64_t>(U32Range.getMax())),
                    static_cast<uint64_t>(U32Range.getMin()),
                    static_cast<uint64_t>(U32Range.getMax())));
            }

            inline void convert_F16_F16(F16_T in, F16_T& out)
            {
                out = in;
            }

            inline void convert_F16_F32(F16_T in, F32_T& out)
            {
                out = in;
            }

            inline void convert_F32_U8(F32_T in, U8_T& out)
            {
                out = static_cast<U8_T>(Core::Math::clamp(
                    static_cast<uint16_t>(in * U8Range.getMax()),
                    static_cast<uint16_t>(U8Range.getMin()),
                    static_cast<uint16_t>(U8Range.getMax())));
            }

            inline void convert_F32_U10(F32_T in, U10_T& out)
            {
                out = static_cast<U10_T>(Core::Math::clamp(
                    static_cast<uint16_t>(in * U10Range.getMax()),
                    static_cast<uint16_t>(U10Range.getMin()),
                    static_cast<uint16_t>(U10Range.getMax())));
            }

            inline void convert_F32_U16(F32_T in, U16_T& out)
            {
                out = static_cast<U16_T>(Core::Math::clamp(
                    static_cast<uint32_t>(in * U16Range.getMax()),
                    static_cast<uint32_t>(U16Range.getMin()),
                    static_cast<uint32_t>(U16Range.getMax())));
            }

            inline void convert_F32_U32(F32_T in, U32_T& out)
            {
                out = static_cast<U32_T>(Core::Math::clamp(
                    static_cast<uint64_t>(static_cast<double>(in) * static_cast<uint64_t>(U32Range.getMax())),
                    static_cast<uint64_t>(U32Range.getMin()),
                    static_cast<uint64_t>(U32Range.getMax())));
            }

            inline void convert_F32_F16(F32_T in, F16_T& out)
            {
                out = in;
            }

            inline void convert_F32_F32(F32_T in, F32_T& out)
            {
                out = in;
            }

        } // namespace Image
    } // namespace AV
} // namespace djv

namespace std
{
    inline std::size_t hash<djv::AV::Image::Type>::operator() (djv::AV::Image::Type value) const noexcept
    {
        size_t hash = 0;
        djv::Core::Memory::hashCombine<int>(hash, static_cast<int>(value));
        return hash;
    }

} // namespace std

