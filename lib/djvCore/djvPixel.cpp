//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvPixel.cpp

#include <djvPixel.h>

#include <djvAssert.h>
#include <djvDebug.h>

//------------------------------------------------------------------------------
// djvPixel
//------------------------------------------------------------------------------

const QStringList & djvPixel::formatLabels()
{
    static const QStringList data = QStringList() <<
        "L" <<
        "LA" <<
        "RGB" <<
        "RGBA";

    DJV_ASSERT(data.count() == FORMAT_COUNT);

    return data;
}

const QStringList & djvPixel::typeLabels()
{
    static const QStringList data = QStringList() <<
        "U8" <<
        "U10" <<
        "U16" <<
        "F16" <<
        "F32";

    DJV_ASSERT(data.count() == TYPE_COUNT);

    return data;
}

const QStringList & djvPixel::dataLabels()
{
    static const QStringList data = QStringList() <<
        "Integer" <<
        "Float";

    DJV_ASSERT(data.count() == DATA_COUNT);

    return data;
}

const QStringList & djvPixel::pixelLabels()
{
    static const QStringList data = QStringList() <<
        "L U8" <<
        "L U16" <<
        "L F16" <<
        "L F32" <<
        "LA U8" <<
        "LA U16" <<
        "LA F16" <<
        "LA F32" <<
        "RGB U8" <<
        "RGB U10" <<
        "RGB U16" <<
        "RGB F16" <<
        "RGB F32" <<
        "RGBA U8" <<
        "RGBA U16" <<
        "RGBA F16" <<
        "RGBA F32";

    DJV_ASSERT(data.count() == PIXEL_COUNT);

    return data;
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvPixel::FORMAT, djvPixel::formatLabels())
_DJV_STRING_OPERATOR_LABEL(djvPixel::TYPE, djvPixel::typeLabels())
_DJV_STRING_OPERATOR_LABEL(djvPixel::DATA, djvPixel::dataLabels())
_DJV_STRING_OPERATOR_LABEL(djvPixel::PIXEL, djvPixel::pixelLabels())


QStringList & operator >> (QStringList & in, djvPixel::Mask & out) throw (QString)
{
    for (int i = 0; i < djvPixel::channelsMax; ++i)
    {
        in >> out[i];
    }
    
    return in;
}

QStringList & operator << (QStringList & out, const djvPixel::Mask & in)
{
    for (int i = 0; i < djvPixel::channelsMax; ++i)
    {
        out << in[i];
    }
    
    return out;
}

djvDebug & operator << (djvDebug & debug, const djvPixel::FORMAT & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const djvPixel::TYPE & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const djvPixel::DATA & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const djvPixel::PIXEL & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const djvPixel::Mask & in)
{
    for (int i = 0; i < djvPixel::channelsMax; ++i)
    {
        debug << static_cast<int>(in[i]);
    }
    
    return debug;
}

