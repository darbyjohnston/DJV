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

//! \file djvBoxUtilInline.h

#include <djvRangeUtil.h>
#include <djvVectorUtil.h>

#include <QRect>
#include <QRectF>

//------------------------------------------------------------------------------
// djvBoxUtil
//------------------------------------------------------------------------------

inline djvBoxUtil::~djvBoxUtil()
{}

template<typename T, int D>
inline djvBox<T, D> djvBoxUtil::corners(
    const djvVector<T, D> & upperLeft,
    const djvVector<T, D> & lowerRight)
{
    const djvVector<T, D> & a = djvVectorUtil::min(upperLeft, lowerRight);
    const djvVector<T, D> & b = djvVectorUtil::max(upperLeft, lowerRight);

    djvBox<T, D> out;

    out.position = a;
    out.setLowerRight(b);

    return out;
}

template<typename T, int D>
inline djvBox<T, D> djvBoxUtil::swap(const djvBox<T, D> & in)
{
    return djvBox<T, D>(
        djvVectorUtil::swap(in.position), djvVectorUtil::swap(in.size));
}

template<typename T, int D>
inline bool djvBoxUtil::intersect(const djvBox<T, D> & a, const djvVector<T, D> & b)
{
    const djvVector<T, D> a1 = a.lowerRight();

    for (int i = 0; i < a.dimension; ++i)
    {
        if (! djvRangeUtil::intersect(b.e[i], djvRange<T>(a.position.e[i], a1.e[i])))
        {
            return false;
        }
    }

    return true;
}

template<typename T, int D>
inline djvBox<T, D> djvBoxUtil::intersect(const djvBox<T, D> & a, const djvBox<T, D> & b)
{
    const djvVector<T, D> a1 = a.lowerRight();
    const djvVector<T, D> b1 = b.lowerRight();

    djvBox<T, D>    out;
    djvVector<T, D> lowerRight;

    for (int i = 0; i < a.dimension; ++i)
    {
        djvRange<T> tmp;
        
        djvRangeUtil::intersect(
            djvRange<T>(a.position.e[i], a1.e[i]),
            djvRange<T>(b.position.e[i], b1.e[i]),
            &tmp);
        
        out.position.e[i] = tmp.min;
        lowerRight.e[i] = tmp.max;
    }

    out.setLowerRight(lowerRight);

    return out;
}

template<typename T, int D>
inline djvBox<T, D> djvBoxUtil::bound(const djvBox<T, D> & a, const djvBox<T, D> & b)
{
    const djvVector<T, D> a1 = a.lowerRight();
    const djvVector<T, D> b1 = b.lowerRight();

    djvBox<T, D>    out;
    djvVector<T, D> lowerRight;

    for (int i = 0; i < a.dimension; ++i)
    {
        djvRange<T> tmp;
        
        djvRangeUtil::bound(
            djvRange<T>(a.position.e[i], a1.e[i]),
            djvRange<T>(b.position.e[i], b1.e[i]),
            &tmp);
        
        out.position.e[i] = tmp.min;
        lowerRight.e[i] = tmp.max;
    }

    out.setLowerRight(lowerRight);

    return out;
}

template<typename T, int D>
inline djvBox<T, D> djvBoxUtil::expand(const djvBox<T, D> & in, const djvVector<T, D> & expand)
{
    return corners(
        djvVectorUtil::min(in.position, expand),
        djvVectorUtil::max(in.lowerRight(), expand));
}

template<typename T, int D>
inline djvBox<T, D> djvBoxUtil::border(const djvBox<T, D> & in, const djvVector<T, D> & border)
{
    return djvBox<T, D>(in.position - border, in.size + border * T(2));
}

template<typename T, int D>
inline bool djvBoxUtil::isSizeValid(const djvBox<T, D> & in)
{
    return djvVectorUtil::isSizeValid(in.size);
}

template<typename T, typename T2, int D>
inline djvBox<T2, D> djvBoxUtil::convert(const djvBox<T, D> & in)
{
    djvBox<T2, D> out;

    out.position = djvVectorUtil::convert<T, T2, D>(in.position);
    out.size     = djvVectorUtil::convert<T, T2, D>(in.size);

    return out;
}

inline QRect djvBoxUtil::toQt(const djvBox<int, 2> & in)
{
    return QRect(in.x, in.y, in.w, in.h);
}

inline QRectF djvBoxUtil::toQt(const djvBox<double, 2> & in)
{
    return QRectF(in.x, in.y, in.w, in.h);
}

inline djvBox2i djvBoxUtil::fromQt(const QRect & in)
{
    return djvBox2i(in.x(), in.y(), in.width(), in.height());
}

inline djvBox2f djvBoxUtil::fromQt(const QRectF & in)
{
    return djvBox2f(in.x(), in.y(), in.width(), in.height());
}
