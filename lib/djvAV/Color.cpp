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

#include <djvAV/Color.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            Color::Color() :
                _pixel(Pixel::None)
            {}

            Color::Color(Pixel pixel) :
                _pixel(pixel),
                _data(getByteCount(pixel))
            {
                zero();
            }

            Color::Color(int r, int g, int b, int a) :
                _pixel(Pixel::RGBA_U8),
                _data(getByteCount(Pixel::RGBA_U8))
            {
                _data[0] = r;
                _data[1] = g;
                _data[2] = b;
                _data[3] = a;
            }

            Color::Color(F32_T r, F32_T g, F32_T b, F32_T a) :
                _pixel(Pixel::RGBA_F32),
                _data(getByteCount(Pixel::RGBA_F32))
            {
                float * p = reinterpret_cast<float *>(_data.data());
                p[0] = r;
                p[1] = g;
                p[2] = b;
                p[3] = a;
            }

            void Color::zero()
            {
                memset(_data.data(), 0, getByteCount(_pixel));
            }

            Color Color::convert(const Pixel & pixel) const
            {
                auto out = Color(pixel);
                Image::convert(_data.data(), _pixel, out._data.data(), pixel, 1);
                return out;
            }

            bool Color::operator == (const Color & other) const
            {
                return
                    _pixel == other._pixel &&
                    0 == memcmp(_data.data(), other._data.data(), getByteCount(_pixel));
            }

            bool Color::operator != (const Color & other) const
            {
                return !(*this == other);
            }

        } // namespace Image
    } // namespace AV

    std::ostream & operator << (std::ostream & os, const AV::Image::Color & value)
    {
        const auto pixel = value.getPixel();
        os << pixel;
        const size_t channelCount = AV::Image::getChannelCount(pixel);
        switch (AV::Image::getType(pixel))
        {
        case GL_UNSIGNED_BYTE:
        {
            os << " ";
            const auto * p = reinterpret_cast<const AV::Image::U8_T *>(value.getData());
            for (size_t i = 0; i < channelCount; ++i)
            {
                os << static_cast<int>(p[i]);
                if (i < channelCount - 1)
                {
                    os << " ";
                }
            }
            break;
        }
        case GL_UNSIGNED_SHORT:
        {
            os << " ";
            const auto * p = reinterpret_cast<const AV::Image::U16_T *>(value.getData());
            for (size_t i = 0; i < channelCount; ++i)
            {
                os << p[i];
                if (i < channelCount - 1)
                {
                    os << " ";
                }
            }
            break;
        }
        case GL_UNSIGNED_INT:
        {
            os << " ";
            const auto * p = reinterpret_cast<const AV::Image::U32_T *>(value.getData());
            for (size_t i = 0; i < channelCount; ++i)
            {
                os << p[i];
                if (i < channelCount - 1)
                {
                    os << " ";
                }
            }
            break;
        }
        case GL_HALF_FLOAT:
        {
            os << " ";
            const auto * p = reinterpret_cast<const AV::Image::F16_T *>(value.getData());
            for (size_t i = 0; i < channelCount; ++i)
            {
                os << p[i];
                if (i < channelCount - 1)
                {
                    os << " ";
                }
            }
            break;
        }
        case GL_FLOAT:
        {
            os << " ";
            const auto * p = reinterpret_cast<const AV::Image::F32_T *>(value.getData());
            for (size_t i = 0; i < channelCount; ++i)
            {
                os << p[i];
                if (i < channelCount - 1)
                {
                    os << " ";
                }
            }
            break;
        }
        case GL_UNSIGNED_INT_10_10_10_2:
        {
            const auto * p = reinterpret_cast<const AV::Image::U10_S *>(value.getData());
            os << " " << p->r << " " << p->g << " " << p->b;
            break;
        }
        }
        return os;
    }

    std::istream & operator >> (std::istream & is, AV::Image::Color & value)
    {
        AV::Image::Pixel pixel = AV::Image::Pixel::None;
        is >> pixel;
        value = AV::Image::Color(pixel);
        const size_t channelCount = AV::Image::getChannelCount(pixel);
        switch (AV::Image::getType(pixel))
        {
        case GL_UNSIGNED_BYTE:
        {
            auto * p = reinterpret_cast<AV::Image::U8_T *>(value.getData());
            for (size_t i = 0; i < channelCount; ++i)
            {
                int tmp = 0;
                is >> tmp;
                p[i] = tmp;
            }
            break;
        }
        case GL_UNSIGNED_SHORT:
        {
            auto * p = reinterpret_cast<AV::Image::U16_T *>(value.getData());
            for (size_t i = 0; i < channelCount; ++i)
            {
                is >> p[i];
            }
            break;
        }
        case GL_UNSIGNED_INT:
        {
            auto * p = reinterpret_cast<AV::Image::U32_T *>(value.getData());
            for (size_t i = 0; i < channelCount; ++i)
            {
                is >> p[i];
            }
            break;
        }
        case GL_HALF_FLOAT:
        {
            auto * p = reinterpret_cast<AV::Image::F16_T *>(value.getData());
            for (size_t i = 0; i < channelCount; ++i)
            {
                is >> p[i];
            }
            break;
        }
        case GL_FLOAT:
        {
            auto * p = reinterpret_cast<AV::Image::F32_T *>(value.getData());
            for (size_t i = 0; i < channelCount; ++i)
            {
                is >> p[i];
            }
            break;
        }
        case GL_UNSIGNED_INT_10_10_10_2:
        {
            auto * p = reinterpret_cast<AV::Image::U10_S *>(value.getData());
            int tmp = 0;
            is >> tmp;
            p->r = tmp;
            is >> tmp;
            p->g = tmp;
            is >> tmp;
            p->b = tmp;
            break;
        }
        }
        return is;
    }

} // namespace djv

