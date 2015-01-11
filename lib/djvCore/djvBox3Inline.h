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

//! \file djvBox3Inline.h

#include <djvVectorUtil.h>

//------------------------------------------------------------------------------
// djvBox<T, 3>
//------------------------------------------------------------------------------

//! \todo Is this specialization for integer box types correct?
//! \todo Why do these specializations need to come first on Windows (error
//! C2908)?

template<>
inline djvVector<int, 3> djvBox<int, 3>::lowerRight() const
{
    return position + size - 1;
}

template<>
inline void djvBox<int, 3>::setLowerRight(const djvVector<int, 3> & in)
{
    size = in - position + 1;
}

template<typename T>
const int djvBox<T, 3>::dimension = 3;

#define _BOX3_INIT() \
    \
    x(djvBox::position.e[0]), \
    y(djvBox::position.e[1]), \
    z(djvBox::position.e[2]), \
    w(djvBox::size.e[0]),     \
    h(djvBox::size.e[1]),     \
    d(djvBox::size.e[2])

template<typename T>
inline djvBox<T, 3>::djvBox() :
    _BOX3_INIT()
{}

template<typename T>
inline djvBox<T, 3>::djvBox(const djvBox<T, 3> & in) :
    position(in.position),
    size(in.size),
    _BOX3_INIT()
{}

template<typename T>
inline djvBox<T, 3>::djvBox(const djvVector<T, 3> & position, const djvVector<T, 3> & size) :
    position(position),
    size(size),
    _BOX3_INIT()
{}

template<typename T>
inline djvBox<T, 3>::djvBox(const djvVector<T, 3> & size) :
    size(size),
    _BOX3_INIT()
{}

template<typename T>
inline djvBox<T, 3>::djvBox(T _x, T _y, T _z, T _w, T _h, T _d) :
    _BOX3_INIT()
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
    h = _h;
    d = _d;
}

template<typename T>
inline djvBox<T, 3>::djvBox(T _w, T _h, T _d) :
    _BOX3_INIT()
{
    w = _w;
    h = _h;
    d = _d;
}

template<typename T>
inline bool djvBox<T, 3>::isValid() const
{
    return djvVectorUtil::isSizeValid(size);
}

template<typename T>
inline void djvBox<T, 3>::zero()
{
    position.zero();
    size.zero();
}

template<typename T>
inline djvVector<T, 3> djvBox<T, 3>::lowerRight() const
{
    return position + size;
}

template<typename T>
inline void djvBox<T, 3>::setLowerRight(const djvVector<T, 3> & in)
{
    size = in - position;
}

template<typename T>
inline djvBox<T, 3> & djvBox<T, 3>::operator = (const djvBox<T, 3> & in)
{
    if (&in != this)
    {
        position = in.position;
        size     = in.size;
    }

    return *this;
}

template<typename T>
inline djvBox<T, 3> & djvBox<T, 3>::operator *= (const djvVector<T, 3> & in)
{
    position *= in;
    size *= in;

    return *this;
}

template<typename T>
inline djvBox<T, 3> & djvBox<T, 3>::operator /= (const djvVector<T, 3> & in)
{
    position /= in;
    size /= in;

    return *this;
}

template<typename T>
inline djvBox<T, 3> & djvBox<T, 3>::operator *= (T in)
{
    position *= in;
    size *= in;

    return *this;
}

template<typename T>
inline djvBox<T, 3> & djvBox<T, 3>::operator /= (T in)
{
    position /= in;
    size /= in;

    return *this;
}

template<typename T>
inline djvBox<T, 3>::operator djvBox<int, 3>() const
{
    return djvBox<int, 3>(
        static_cast<int>(x), static_cast<int>(y), static_cast<int>(z),
        static_cast<int>(w), static_cast<int>(h), static_cast<int>(d));
}

template<typename T>
inline djvBox<T, 3>::operator djvBox<double, 3>() const
{
    return djvBox<double, 3>(
        static_cast<double>(x), static_cast<double>(y), static_cast<double>(z),
        static_cast<double>(w), static_cast<double>(h), static_cast<double>(d));
}
