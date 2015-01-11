//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

//! \file djvColor.cpp

#include <djvColor.h>

#include <djvColorUtil.h>
#include <djvDebug.h>
#include <djvMemory.h>

//------------------------------------------------------------------------------
// djvColor
//------------------------------------------------------------------------------

void djvColor::init()
{
    _channels = djvPixel::channels(_pixel);
}

djvColor::djvColor() :
    _pixel(static_cast<djvPixel::PIXEL>(0))
{
    init();

    zero();
}

djvColor::djvColor(const djvColor & in) :
    _pixel(static_cast<djvPixel::PIXEL>(0))
{
    init();

    *this = in;
}

djvColor::djvColor(djvPixel::PIXEL pixel) :
    _pixel(pixel)
{
    init();

    zero();
}

djvColor::djvColor(djvPixel::F32_T l) :
    _pixel(djvPixel::L_F32)
{
    init();

    setF32(l, 0);
}

djvColor::djvColor(djvPixel::F32_T l, djvPixel::F32_T a) :
    _pixel(djvPixel::LA_F32)
{
    init();

    setF32(l, 0);
    setF32(a, 1);
}

djvColor::djvColor(djvPixel::F32_T r, djvPixel::F32_T g, djvPixel::F32_T b) :
    _pixel(djvPixel::RGB_F32)
{
    init();

    setF32(r, 0);
    setF32(g, 1);
    setF32(b, 2);
}

djvColor::djvColor(djvPixel::F32_T r, djvPixel::F32_T g, djvPixel::F32_T b, djvPixel::F32_T a) :
    _pixel(djvPixel::RGBA_F32)
{
    init();

    setF32(r, 0);
    setF32(g, 1);
    setF32(b, 2);
    setF32(a, 3);
}

djvColor::djvColor(const quint8 * data, djvPixel::PIXEL pixel) :
    _pixel(pixel)
{
    init();

    djvMemory::copy(data, _data, djvPixel::byteCount(_pixel));
}

void djvColor::setPixel(djvPixel::PIXEL pixel)
{
    if (pixel == _pixel)
        return;

    djvColor tmp(pixel);
    djvColorUtil::convert(*this, tmp);
    *this = tmp;
}

void djvColor::zero()
{
    djvMemory::fill<quint8>(0, _data, djvPixel::byteCount(_pixel));
}

djvColor & djvColor::operator = (const djvColor & in)
{
    if (&in != this)
    {
        _pixel    = in._pixel;
        _channels = in._channels;
        
        djvMemory::copy(in._data, _data, djvPixel::byteCount(_pixel));
    }

    return *this;
}

//------------------------------------------------------------------------------

bool operator == (const djvColor & a, const djvColor & b)
{
    const djvPixel::PIXEL pixel = a.pixel();
    
    if (pixel != b.pixel())
        return false;

    const int channels = djvPixel::channels(pixel);

    for (int c = 0; c < channels; ++c)
    {
        switch (djvPixel::type(pixel))
        {
            case djvPixel::U8:
                if (a.u8(c) != b.u8(c))
                    return false;
                break;

            case djvPixel::U10:
                if (a.u10(c) != b.u10(c))
                    return false;
                break;

            case djvPixel::U16:
                if (a.u16(c) != b.u16(c))
                    return false;
                break;

            case djvPixel::F16:
                if (a.f16(c) != b.f16(c))
                    return false;
                break;

            case djvPixel::F32:
                if (! djvMath::fuzzyCompare(a.f32(c), b.f32(c)))
                    return false;
                break;

            default: break;
        }
    }
    
    return true;
}

bool operator != (const djvColor & a, const djvColor & b)
{
    return ! (a == b);
}

QStringList & operator >> (QStringList & in, djvColor & out) throw (QString)
{
    djvPixel::PIXEL pixel = static_cast<djvPixel::PIXEL>(0);
    in >> pixel;
    out.setPixel(pixel);

    const int channels = djvPixel::channels(pixel);

    for (int c = 0; c < channels; ++c)
    {
        switch (djvPixel::type(pixel))
        {
            case djvPixel::U8:
            {
                int value = 0;
                in >> value;
                out.setU8(djvMath::clamp(value, 0, djvPixel::u8Max), c);
            }
            break;

            case djvPixel::U10:
            {
                int value = 0;
                in >> value;
                out.setU10(djvMath::clamp(value, 0, djvPixel::u10Max), c);
            }
            break;

            case djvPixel::U16:
            {
                int value = 0;
                in >> value;
                out.setU16(djvMath::clamp(value, 0, djvPixel::u16Max), c);
            }
            break;

            case djvPixel::F16:
            {
                double value = 0.0;
                in >> value;
                out.setF16(static_cast<djvPixel::F16_T>(value), c);
            }
            break;

            case djvPixel::F32:
            {
                double value = 0.0;
                in >> value;
                out.setF32(static_cast<djvPixel::F32_T>(value), c);
            }
            break;

            default: break;
        }
    }
    
    return in;
}

QStringList & operator << (QStringList & out, const djvColor & in)
{
    out << in.pixel();

    const int channels = djvPixel::channels(in.pixel());

    for (int c = 0; c < channels; ++c)
    {
        switch (djvPixel::type(in.pixel()))
        {
            case djvPixel::U8:  out << in.u8 (c); break;
            case djvPixel::U10: out << in.u10(c); break;
            case djvPixel::U16: out << in.u16(c); break;
            case djvPixel::F16: out << in.f16(c); break;
            case djvPixel::F32: out << in.f32(c); break;

            default: break;
        }
    }
    
    return out;
}

djvDebug & operator << (djvDebug & debug, const djvColor & in)
{
    return debug << djvStringUtil::label(in);
}
