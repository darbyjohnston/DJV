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

//! \file djvVector3Inline.h

//------------------------------------------------------------------------------
// djvVector<T, 3>
//------------------------------------------------------------------------------

template<typename T>
const int djvVector<T, 3>::dimension = 3;

#define _VEC3_INIT() \
    \
    x(e[0]), \
    y(e[1]), \
    z(e[2])

template<typename T>
inline djvVector<T, 3>::djvVector() :
    _VEC3_INIT()
{
    x = y = z = 0;
}

template<typename T>
inline djvVector<T, 3>::djvVector(const djvVector<T, 3> & in) :
    _VEC3_INIT()
{
    x = in.x;
    y = in.y;
    z = in.z;
}

template<typename T>
inline djvVector<T, 3>::djvVector(T _x, T _y, T _z) :
    _VEC3_INIT()
{
    x = _x;
    y = _y;
    z = _z;
}

template<typename T>
inline djvVector<T, 3>::djvVector(T in) :
    _VEC3_INIT()
{
    x = y = z = in;
}

template<typename T>
inline djvVector<T, 3> & djvVector<T, 3>::operator = (const djvVector<T, 3> & in)
{
    if (&in != this)
    {
        x = in.x;
        y = in.y;
        z = in.z;
    }

    return *this;
}

template<typename T>
inline void djvVector<T, 3>::set(T in)
{
    x = y = z = in;
}

template<typename T>
inline void djvVector<T, 3>::zero()
{
    x = y = z = T(0);
}

#define _VEC3_OP(IN) \
    \
    template<typename T> \
    inline djvVector<T, 3> & djvVector<T, 3>::operator IN (const djvVector<T, 3> & in) \
    { \
        x IN in.x; \
        y IN in.y; \
        z IN in.z; \
        return *this; \
    }

_VEC3_OP(+=)
_VEC3_OP(-=)
_VEC3_OP(*=)
_VEC3_OP(/=)

#define _VEC3_OP2(IN) \
    \
    template<typename T> \
    inline djvVector<T, 3> & djvVector<T, 3>::operator IN (T in) \
    { \
        x IN in; \
        y IN in; \
        z IN in; \
        return *this; \
    }

_VEC3_OP2(+=)
_VEC3_OP2(-=)
_VEC3_OP2(*=)
_VEC3_OP2(/=)

template<typename T>
inline djvVector<T, 3>::operator djvVector<int, 3>() const
{
    return djvVector<int, 3>(
        static_cast<int>(x),
        static_cast<int>(y),
        static_cast<int>(z));
}

template<typename T>
inline djvVector<T, 3>::operator djvVector<double, 3>() const
{
    return djvVector<double, 3>(
        static_cast<double>(x),
        static_cast<double>(y),
        static_cast<double>(z));
}
