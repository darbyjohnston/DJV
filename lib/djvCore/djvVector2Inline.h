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

//! \file djvVector2Inline.h

//------------------------------------------------------------------------------
// djvVector<T, 2>
//------------------------------------------------------------------------------

template<typename T>
const int djvVector<T, 2>::dimension = 2;

#define _VEC2_INIT() \
    \
    x(djvVector<T, 2>::e[0]), \
    y(djvVector<T, 2>::e[1])

template<typename T>
inline djvVector<T, 2>::djvVector() :
    _VEC2_INIT()
{
    x = y = 0;
}

template<typename T>
inline djvVector<T, 2>::djvVector(const djvVector<T, 2> & in) :
    _VEC2_INIT()
{
    x = in.x;
    y = in.y;
}

template<typename T>
inline djvVector<T, 2>::djvVector(T _x, T _y) :
    _VEC2_INIT()
{
    x = _x;
    y = _y;
}

template<typename T>
inline djvVector<T, 2>::djvVector(T in) :
    _VEC2_INIT()
{
    x = y = in;
}

template<typename T>
inline djvVector<T, 2> & djvVector<T, 2>::operator = (const djvVector<T, 2> & in)
{
    if (&in != this)
    {
        x = in.x;
        y = in.y;
    }

    return *this;
}

template<typename T>
inline void djvVector<T, 2>::set(T in)
{
    x = y = in;
}

template<typename T>
inline void djvVector<T, 2>::zero()
{
    x = y = T(0);
}

#define _VEC2_OP(IN) \
    \
    template<typename T> \
    inline djvVector<T, 2> & djvVector<T, 2>::operator IN (const djvVector<T, 2> & in) \
    { \
        x IN in.x; \
        y IN in.y; \
        return *this; \
    }

_VEC2_OP(+=)
_VEC2_OP(-=)
_VEC2_OP(*=)
_VEC2_OP(/=)

#define _VEC2_OP2(IN) \
    \
    template<typename T> \
    inline djvVector<T, 2> & djvVector<T, 2>::operator IN (T in) \
    { \
        x IN in; \
        y IN in; \
        return *this; \
    }

_VEC2_OP2(+=)
_VEC2_OP2(-=)
_VEC2_OP2(*=)
_VEC2_OP2(/=)

template<typename T>
inline djvVector<T, 2>::operator djvVector<int, 2>() const
{
    return djvVector<int, 2>(
        static_cast<int>(x),
        static_cast<int>(y));
}

template<typename T>
inline djvVector<T, 2>::operator djvVector<double, 2>() const
{
    return djvVector<double, 2>(
        static_cast<double>(x),
        static_cast<double>(y));
}
