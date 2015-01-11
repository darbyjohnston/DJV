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

//! \file djvColorUtil.cpp

#include <djvColorUtil.h>

#include <djvColor.h>

#include <QColor>

//------------------------------------------------------------------------------
// djvColorUtil
//------------------------------------------------------------------------------

djvColorUtil::~djvColorUtil()
{}

void djvColorUtil::scale(double value, const djvColor & in, djvColor & out)
{
    djvColor in_(djvPixel::pixel(djvPixel::format(out.pixel()), djvPixel::F32));
    convert(in, in_);

    const int channels = djvPixel::channels(out.pixel());

    for (int c = 0; c < channels; ++c)
    {
        in_.setF32(in_.f32(c) * static_cast<float>(value), c);
    }

    convert(in_, out);
}

djvColor djvColorUtil::scale(double value, const djvColor & in)
{
    djvColor out(in.pixel());
    scale(value, in, out);
    return out;
}

void djvColorUtil::lerp(
    double           value,
    const djvColor & min,
    const djvColor & max,
    djvColor &       out)
{
    djvColor min_(djvPixel::pixel(djvPixel::format(out.pixel()), djvPixel::F32));
    djvColor max_(djvPixel::pixel(djvPixel::format(out.pixel()), djvPixel::F32));
    convert(min, min_);
    convert(max, max_);

    const int channels = djvPixel::channels(out.pixel());

    for (int c = 0; c < channels; ++c)
    {
        min_.setF32(
            djvMath::lerp(value, min_.f32(c), max_.f32(c)),
            c);
    }

    convert(min_, out);
}

void djvColorUtil::lerp(
    double         value,
    const QColor & min,
    const QColor & max,
    QColor &       out)
{
    out.setRedF  (djvMath::lerp(value, min.redF(),   max.redF  ()));
    out.setGreenF(djvMath::lerp(value, min.greenF(), max.greenF()));
    out.setBlueF (djvMath::lerp(value, min.blueF(),  max.blueF ()));
    out.setAlphaF(djvMath::lerp(value, min.alphaF(), max.alphaF()));
}

djvColor djvColorUtil::lerp(double value, const djvColor & min, const djvColor & max)
{
    djvColor out(djvMath::max(min.pixel(), max.pixel()));
    lerp(value, min, max, out);
    return out;
}

QColor djvColorUtil::lerp(double value, const QColor & min, const QColor & max)
{
    QColor out;
    out.setRedF  (djvMath::lerp(value, min.redF(),   max.redF  ()));
    out.setGreenF(djvMath::lerp(value, min.greenF(), max.greenF()));
    out.setBlueF (djvMath::lerp(value, min.blueF(),  max.blueF ()));
    out.setAlphaF(djvMath::lerp(value, min.alphaF(), max.alphaF()));
    return out;
}

void djvColorUtil::convert(const djvColor & in, djvColor & out)
{
    djvPixel::convert(in.data(), in.pixel(), out.data(), out.pixel());
}

QColor djvColorUtil::toQt(const djvColor & in)
{
    djvColor tmp(djvPixel::RGBA_U8);

    djvColorUtil::convert(in, tmp);

    return QColor(tmp.u8(0), tmp.u8(1), tmp.u8(2), tmp.u8(3));
}

djvColor djvColorUtil::fromQt(const QColor & in)
{
    djvColor out(djvPixel::RGBA_U8);

    out.setU8(in.red(), 0);
    out.setU8(in.green(), 1);
    out.setU8(in.blue(), 2);
    out.setU8(in.alpha(), 3);

    return out;
}
