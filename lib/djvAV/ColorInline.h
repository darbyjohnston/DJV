//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            inline Color::Color() :
                _type(Type::None)
            {}

            inline Color::Color(Type type) :
                _type(type),
                _data(getByteCount(_type))
            {
                zero();
            }

            inline Color::Color(int r, int g, int b, int a) :
                _type(Type::RGBA_U8),
                _data(getByteCount(_type))
            {
                auto p = reinterpret_cast<U8_T *>(_data.data());
                p[0] = r;
                p[1] = g;
                p[2] = b;
                p[3] = a;
            }

            inline Color::Color(F32_T r, F32_T g, F32_T b, F32_T a) :
                _type(Type::RGBA_F32),
                _data(getByteCount(_type))
            {
                auto p = reinterpret_cast<F32_T *>(_data.data());
                p[0] = r;
                p[1] = g;
                p[2] = b;
                p[3] = a;
            }

            inline Type Color::getType() const
            {
                return _type;
            }

            inline bool Color::isValid() const
            {
                return _type != Type::None;
            }

            inline U8_T Color::getU8(size_t channel) const
            {
                return reinterpret_cast<const U8_T *>(_data.data())[channel];
            }

            inline U10_T Color::getU10(size_t channel) const
            {
                auto p = reinterpret_cast<const U10_S *>(_data.data());
                switch (channel)
                {
                case 0: return p->r;
                case 1: return p->g;
                case 2: return p->b;
                }
                return 0;
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

            inline const uint8_t * Color::getData() const
            {
                return _data.data();
            }

            inline uint8_t * Color::getData()
            {
                return _data.data();
            }

        } // namespace Image
    } // namespace AV
} // namespace djv
