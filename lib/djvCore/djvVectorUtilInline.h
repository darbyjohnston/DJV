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

//! \file djvVectorUtilInline.h

#include <djvMath.h>

//------------------------------------------------------------------------------
// djvVectorUtil
//------------------------------------------------------------------------------

template<typename T, int D>
inline djvVector<T, D> djvVectorUtil::abs(const djvVector<T, D> & in)
{
    djvVector<T, D> out;

    for (int i = 0; i < in.dimension; ++i)
    {
        out.e[i] = djvMath::abs(in.e[i]);
    }

    return out;
}

template<typename T, int D>
inline djvVector<T, D> djvVectorUtil::swap(const djvVector<T, D> & in)
{
    djvVector<T, D> out;

    for (int i = 0; i < in.dimension; ++i)
    {
        out.e[i] = in.e[in.dimension - 1 - i];
    }

    return out;
}

template<typename T, int D>
inline djvVector<T, D> djvVectorUtil::min(const djvVector<T, D> & a, const djvVector<T, D> & b)
{
    djvVector<T, D> out;

    for (int i = 0; i < a.dimension; ++i)
    {
        out.e[i] = djvMath::min(a.e[i], b.e[i]);
    }

    return out;
}

template<typename T, int D>
inline djvVector<T, D> djvVectorUtil::max(const djvVector<T, D> & a, const djvVector<T, D> & b)
{
    djvVector<T, D> out;

    for (int i = 0; i < a.dimension; ++i)
    {
        out.e[i] = djvMath::max(a.e[i], b.e[i]);
    }

    return out;
}

template<typename T, int D>
inline djvVector<T, D> djvVectorUtil::clamp(
    const djvVector<T, D> & in,
    const djvVector<T, D> & min,
    const djvVector<T, D> & max)
{
    djvVector<T, D> out;

    for (int i = 0; i < in.dimension; ++i)
    {
        out.e[i] = djvMath::clamp(in.e[i], min.e[i], max.e[i]);
    }

    return out;
}

template<typename T, int D>
inline bool djvVectorUtil::isSizeValid(const djvVector<T, D> & in)
{
    return in > djvVector<T, D>(0);
}

template<typename T, int D>
inline double djvVectorUtil::length(const djvVector<T, D> & in)
{
    double out = 0.0;

    for (int i = 0; i < in.dimension; ++i)
    {
        out += in.e[i] * in.e[i];
    }

    return djvMath::sqrt(out);
}

template<typename T, int D>
inline double djvVectorUtil::area(const djvVector<T, D> & in)
{
    double out = static_cast<double>(in.e[0]);

    for (int i = 1; i < in.dimension; ++i)
    {
        out *= static_cast<double>(in.e[i]);
    }

    return djvMath::abs(out);
}

template<typename T, int D>
inline djvVector<T, D> djvVectorUtil::normalize(const djvVector<T, D> & in)
{
    djvVector<T, D> out;

    const double l = length(in);

    if (! djvMath::fuzzyCompare(l, 0.0))
    {
        for (int i = 0; i < in.dimension; ++i)
        {
            out.e[i] = T(in.e[i] / l);
        }
    }

    return out;
}

template<typename T, int D>
inline double djvVectorUtil::dot(const djvVector<T, D> & a, const djvVector<T, D> & b)
{
    double out = 0.0;

    for (int i = 0; i < a.dimension; ++i)
    {
        out += a.e[i] * b.e[i];
    }

    return out;
}

template<typename T, int D>
inline double djvVectorUtil::aspect(const djvVector<T, D> & in)
{
    return in.e[1] != T(0) ? (in.e[0] / static_cast<double>(in.e[1])) : 0.0;
}

template<typename T, typename T2, int D>
inline djvVector<T2, D> djvVectorUtil::convert(const djvVector<T, D> & in)
{
    djvVector<T2, D> out;

    for (int i = 0; i < in.dimension; ++i)
    {
        out.e[i] = T2(in.e[i]);
    }

    return out;
}

template<typename T, typename T2, int D>
inline djvVector<T2, D> djvVectorUtil::ceil(const djvVector<T, D> & in)
{
    djvVector<T2, D> out;

    for (int i = 0; i < in.dimension; ++i)
    {
        out.e[i] = djvMath::ceil(in.e[i]);
    }

    return out;
}

template<typename T, typename T2, int D>
inline djvVector<T2, D> djvVectorUtil::floor(const djvVector<T, D> & in)
{
    djvVector<T2, D> out;

    for (int i = 0; i < in.dimension; ++i)
    {
        out.e[i] = djvMath::floor(in.e[i]);
    }

    return out;
}
