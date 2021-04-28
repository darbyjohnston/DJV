// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/Type.h>

namespace djv
{
    namespace Image
    {
        inline Type Color::getType() const noexcept
        {
            return _type;
        }

        inline bool Color::isValid() const noexcept
        {
            return _type != Type::None;
        }

        inline U8_T Color::getU8(size_t channel) const
        {
            return reinterpret_cast<const U8_T *>(_data.data())[channel];
        }

        inline U10_T Color::getU10(size_t channel) const
        {
            U10_T out = 0;
            auto p = reinterpret_cast<const U10_S *>(_data.data());
            switch (channel)
            {
            case 0: out = p->r; break;
            case 1: out = p->g; break;
            case 2: out = p->b; break;
            default: break;
            }
            return out;
        }

        inline U16_T Color::getU16(size_t channel) const
        {
            return reinterpret_cast<const U16_T *>(_data.data())[channel];
        }

        inline U32_T Color::getU32(size_t channel) const
        {
            return reinterpret_cast<const U32_T *>(_data.data())[channel];
        }

        inline F16_T Color::getF16(size_t channel) const
        {
            return reinterpret_cast<const F16_T *>(_data.data())[channel];
        }

        inline F32_T Color::getF32(size_t channel) const
        {
            return reinterpret_cast<const F32_T *>(_data.data())[channel];
        }

        inline void Color::setU8(U8_T value, size_t channel)
        {
            reinterpret_cast<U8_T *>(_data.data())[channel] = value;
        }

        inline void Color::setU10(U10_T value, size_t channel)
        {
            auto p = reinterpret_cast<U10_S *>(_data.data());
            switch (channel)
            {
            case 0: p->r = value; break;
            case 1: p->g = value; break;
            case 2: p->b = value; break;
            default: break;
            }
        }

        inline void Color::setU16(U16_T value, size_t channel)
        {
            reinterpret_cast<U16_T *>(_data.data())[channel] = value;
        }

        inline void Color::setU32(U32_T value, size_t channel)
        {
            reinterpret_cast<U32_T *>(_data.data())[channel] = value;
        }

        inline void Color::setF16(F16_T value, size_t channel)
        {
            reinterpret_cast<F16_T *>(_data.data())[channel] = value;
        }

        inline void Color::setF32(F32_T value, size_t channel)
        {
            reinterpret_cast<F32_T *>(_data.data())[channel] = value;
        }

        inline const uint8_t * Color::getData() const noexcept
        {
            return _data.data();
        }

        inline uint8_t * Color::getData() noexcept
        {
            return _data.data();
        }

        inline Color Color::RGB_U8(int r, int g, int b)
        {
            Color out(Type::RGB_U8);
            out.setU8(r, 0);
            out.setU8(g, 1);
            out.setU8(b, 2);
            return out;
        }

        inline Color Color::RGB_F32(float r, float g, float b)
        {
            Color out(Type::RGB_F32);
            out.setF32(r, 0);
            out.setF32(g, 1);
            out.setF32(b, 2);
            return out;
        }

        inline bool Color::operator == (const Color& other) const
        {
            return
                _type == other._type &&
                0 == memcmp(_data.data(), other._data.data(), getByteCount(_type));
        }

        inline bool Color::operator != (const Color& other) const
        {
            return !(*this == other);
        }

    } // namespace Image
} // namespace djv
