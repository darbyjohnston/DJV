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

#include <djvRangeUtil.h>
#include <djvVectorUtil.h>

#include <QRect>
#include <QRectF>

//------------------------------------------------------------------------------
// djvBoxUtil
//------------------------------------------------------------------------------

inline djvBoxUtil::~djvBoxUtil()
{}

template<typename T, glm::precision P>
inline djvBox2<T, P> djvBoxUtil::corners(
    const glm::tvec2<T, P> & upperLeft,
    const glm::tvec2<T, P> & lowerRight)
{
    const glm::tvec2<T, P> & a = djvVectorUtil::min(upperLeft, lowerRight);
    const glm::tvec2<T, P> & b = djvVectorUtil::max(upperLeft, lowerRight);
    djvBox2<T, P> out;
    out.position = a;
    out.setLowerRight(b);
    return out;
}

template<typename T, glm::precision P>
inline djvBox2<T, P> djvBoxUtil::swap(const djvBox2<T, P> & in)
{
    return djvBox2<T, P>(djvVectorUtil::swap(in.position), djvVectorUtil::swap(in.size));
}

template<typename T, glm::precision P>
inline bool djvBoxUtil::intersect(const djvBox2<T, P> & a, const glm::tvec2<T, P> & b)
{
    const glm::tvec2<T, P> a1 = a.lowerRight();
    for (int i = 0; i < 2; ++i)
    {
        if (! djvRangeUtil::intersect(b[i], djvRange<T>(a.position[i], a1[i])))
        {
            return false;
        }
    }
    return true;
}

template<typename T, glm::precision P>
inline djvBox2<T, P> djvBoxUtil::intersect(const djvBox2<T, P> & a, const djvBox2<T, P> & b)
{
    const glm::tvec2<T, P> a1 = a.lowerRight();
    const glm::tvec2<T, P> b1 = b.lowerRight();
    djvBox2<T, P>    out;
    glm::tvec2<T, P> lowerRight;
    for (int i = 0; i < 2; ++i)
    {
        djvRange<T> tmp;
        djvRangeUtil::intersect(
            djvRange<T>(a.position[i], a1[i]),
            djvRange<T>(b.position[i], b1[i]),
            &tmp);
        out.position[i] = tmp.min;
        lowerRight[i] = tmp.max;
    }
    out.setLowerRight(lowerRight);
    return out;
}

template<typename T, glm::precision P>
inline djvBox2<T, P> djvBoxUtil::bound(const djvBox2<T, P> & a, const djvBox2<T, P> & b)
{
    const glm::tvec2<T, P> a1 = a.lowerRight();
    const glm::tvec2<T, P> b1 = b.lowerRight();
    djvBox2<T, P>    out;
    glm::tvec2<T, P> lowerRight;
    for (int i = 0; i < 2; ++i)
    {
        djvRange<T> tmp;
        djvRangeUtil::bound(
            djvRange<T>(a.position[i], a1[i]),
            djvRange<T>(b.position[i], b1[i]),
            &tmp);
        out.position[i] = tmp.min;
        lowerRight[i] = tmp.max;
    }
    out.setLowerRight(lowerRight);
    return out;
}

template<typename T, glm::precision P>
inline djvBox2<T, P> djvBoxUtil::expand(const djvBox2<T, P> & in, const glm::tvec2<T, P> & expand)
{
    return corners(
        djvVectorUtil::min(in.position, expand),
        djvVectorUtil::max(in.lowerRight(), expand));
}

template<typename T, glm::precision P>
inline djvBox2<T, P> djvBoxUtil::border(const djvBox2<T, P> & in, const glm::tvec2<T, P> & border)
{
    return djvBox2<T, P>(in.position - border, in.size + border * T(2));
}

template<typename T, glm::precision P>
inline bool djvBoxUtil::isSizeValid(const djvBox2<T, P> & in)
{
    return in.size.x > T(0) && in.size.y > T(0);
}

inline QRect djvBoxUtil::toQt(const djvBox2<int> & in)
{
    return QRect(in.x, in.y, in.w, in.h);
}

inline QRectF djvBoxUtil::toQt(const djvBox2<float> & in)
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

