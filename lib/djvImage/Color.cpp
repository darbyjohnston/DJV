// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/Color.h>

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

    } // namepsace Image
} // namespace djv

