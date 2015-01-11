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

//! \file djvVectorInline.h

#include <djvDebug.h>

//------------------------------------------------------------------------------
// djvVector<T, D>
//------------------------------------------------------------------------------

template<typename T, int D>
const int djvVector<T, D>::dimension = D;

template<typename T, int D>
inline djvVector<T, D>::djvVector()
{
    zero();
}

template<typename T, int D>
inline djvVector<T, D>::djvVector(const djvVector<T, D> & in)
{
    for (int i = 0; i < dimension; ++i)
    {
        djvVector<T, D>::e[i] = in.e[i];
    }
}

template<typename T, int D>
inline djvVector<T, D>::djvVector(T in)
{
    set(in);
}

template<typename T, int D>
inline djvVector<T, D> & djvVector<T, D>::operator = (const djvVector<T, D> & in)
{
    if (&in != this)
    {
        for (int i = 0; i < dimension; ++i)
        {
            djvVector<T, D>::e[i] = in.e[i];
        }
    }

    return *this;
}

template<typename T, int D>
inline void djvVector<T, D>::set(T in)
{
    for (int i = 0; i < dimension; ++i)
    {
        e[i] = in;
    }
}

template<typename T, int D>
inline void djvVector<T, D>::zero()
{
    for (int i = 0; i < dimension; ++i)
    {
        e[i] = 0;
    }
}

#define _VEC_OP(IN) \
    \
    template<typename T, int D> \
    inline djvVector<T, D> & djvVector<T, D>::operator IN (const djvVector<T, D> & in) \
    { \
        for (int i = 0; i < dimension; ++i) \
        { \
            djvVector<T, D>::e[i] IN in.e[i]; \
        } \
        \
        return *this; \
    }

_VEC_OP(+=)
_VEC_OP(-=)
_VEC_OP(*=)
_VEC_OP(/=)

#define _VEC_OP2(IN) \
    \
    template<typename T, int D> \
    inline djvVector<T, D> & djvVector<T, D>::operator IN (T in) \
    { \
        for (int i = 0; i < dimension; ++i) \
        { \
            djvVector<T, D>::e[i] IN in; \
        } \
        \
        return *this; \
    }

_VEC_OP2(+=)
_VEC_OP2(-=)
_VEC_OP2(*=)
_VEC_OP2(/=)

//------------------------------------------------------------------------------

#define _VEC_FNC_OP(IN) \
    \
    template<typename T, int D> \
    inline djvVector<T, D> operator IN (const djvVector<T, D> & a, const djvVector<T, D> & b) \
    { \
        djvVector<T, D> out; \
        \
        for (int i = 0; i < a.dimension; ++i) \
        { \
            out.e[i] = a.e[i] IN b.e[i]; \
        } \
        \
        return out; \
    }

_VEC_FNC_OP(+)
_VEC_FNC_OP(-)
_VEC_FNC_OP(*)
_VEC_FNC_OP(/)

#define _VEC_FNC_OP2(IN) \
    \
    template<typename T, int D> \
    inline djvVector<T, D> operator IN (const djvVector<T, D> & a, T b) \
    { \
        djvVector<T, D> out; \
        \
        for (int i = 0; i < a.dimension; ++i) \
        { \
            out.e[i] = a.e[i] IN b; \
        } \
        \
        return out; \
    }

_VEC_FNC_OP2(+)
_VEC_FNC_OP2(-)
_VEC_FNC_OP2(*)
_VEC_FNC_OP2(/)

template<typename T, int D>
inline djvVector<T, D> operator - (const djvVector<T, D> & in)
{
    djvVector<T, D> out;

    for (int i = 0; i < in.dimension; ++i)
    {
        out.e[i] = -in.e[i];
    }

    return out;
}

template<typename T, int D>
inline QStringList & operator << (QStringList & out, const djvVector<T, D> & in)
{
    for (int i = 0; i < in.dimension; ++i)
    {
        out << in.e[i];
    }

    return out;
}

template<typename T, int D>
inline QStringList & operator >> (QStringList & in, djvVector<T, D> & out) throw (QString)
{
    for (int i = 0; i < out.dimension; ++i)
    {
        in >> out.e[i];
    }

    return in;
}

#define _VEC_COMPARE(IN) \
    \
    template<typename T, int D> \
    inline bool operator IN (const djvVector<T, D> & a, const djvVector<T, D> & b) \
    { \
        for (int i = 0; i < a.dimension; ++i) \
        { \
            if (! (a.e[i] IN b.e[i])) \
            { \
                return false; \
            } \
        } \
        \
        return true; \
    }

_VEC_COMPARE(==)
_VEC_COMPARE(< )
_VEC_COMPARE(<=)
_VEC_COMPARE(> )
_VEC_COMPARE(>=)

template<typename T, int D>
inline bool operator != (const djvVector<T, D> & a, const djvVector<T, D> & b)
{
    return ! (a == b);
}

template<typename T, int D>
inline djvDebug & operator << (djvDebug & debug, const djvVector<T, D> & in)
{
    for (int i = 0; i < in.dimension; ++i)
    {
        debug << in.e[i];

        if (i < in.dimension - 1)
            debug << " ";
    }

    return debug;
}

