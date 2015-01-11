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

//! \file djvBox2Inline.h

#include <djvVectorUtil.h>

//------------------------------------------------------------------------------
// djvBox<T, 2>
//------------------------------------------------------------------------------

//! \todo Is this specialization for integer box types correct?
//! \todo Why do these specializations need to come first on Windows (error
//! C2908)?

template<>
inline djvVector<int, 2> djvBox<int, 2>::lowerRight() const
{
    return position + size - 1;
}

template<>
inline void djvBox<int, 2>::setLowerRight(const djvVector<int, 2> & in)
{
    size = in - position + 1;
}

template<typename T>
const int djvBox<T, 2>::dimension = 2;

#define _BOX2_INIT() \
    \
    x(djvBox::position.e[0]), \
    y(djvBox::position.e[1]), \
    w(djvBox::size.e[0]),     \
    h(djvBox::size.e[1])

template<typename T>
inline djvBox<T, 2>::djvBox() :
    _BOX2_INIT()
{}

template<typename T>
inline djvBox<T, 2>::djvBox(const djvBox<T, 2> & in) :
    position(in.position),
    size(in.size),
    _BOX2_INIT()
{}

template<typename T>
inline djvBox<T, 2>::djvBox(const djvVector<T, 2> & position, const djvVector<T, 2> & size) :
    position(position),
    size(size),
    _BOX2_INIT()
{}

template<typename T>
inline djvBox<T, 2>::djvBox(const djvVector<T, 2> & size) :
    size(size),
    _BOX2_INIT()
{}

template<typename T>
inline djvBox<T, 2>::djvBox(T _x, T _y, T _w, T _h) :
    _BOX2_INIT()
{
    x = _x;
    y = _y;
    w = _w;
    h = _h;
}

template<typename T>
inline djvBox<T, 2>::djvBox(T _w, T _h) :
    _BOX2_INIT()
{
    w = _w;
    h = _h;
}

template<typename T>
inline bool djvBox<T, 2>::isValid() const
{
    return djvVectorUtil::isSizeValid(size);
}

template<typename T>
inline void djvBox<T, 2>::zero()
{
    position.zero();
    size.zero();
}

template<typename T>
inline djvVector<T, 2> djvBox<T, 2>::lowerRight() const
{
    return position + size;
}

template<typename T>
inline void djvBox<T, 2>::setLowerRight(const djvVector<T, 2> & in)
{
    size = in - position;
}

template<typename T>
inline djvBox<T, 2> & djvBox<T, 2>::operator = (const djvBox<T, 2> & in)
{
    if (&in != this)
    {
        position = in.position;
        size     = in.size;
    }

    return *this;
}

template<typename T>
inline djvBox<T, 2> & djvBox<T, 2>::operator *= (const djvVector<T, 2> & in)
{
    position *= in;
    size *= in;

    return *this;
}

template<typename T>
inline djvBox<T, 2> & djvBox<T, 2>::operator /= (const djvVector<T, 2> & in)
{
    position /= in;
    size /= in;

    return *this;
}

template<typename T>
inline djvBox<T, 2> & djvBox<T, 2>::operator *= (T in)
{
    position *= in;
    size *= in;

    return *this;
}

template<typename T>
inline djvBox<T, 2> & djvBox<T, 2>::operator /= (T in)
{
    position /= in;
    size /= in;

    return *this;
}

template<typename T>
inline djvBox<T, 2>::operator djvBox<int, 2>() const
{
    return djvBox<int, 2>(
        static_cast<int>(x),
        static_cast<int>(y),
        static_cast<int>(w),
        static_cast<int>(h));
}

template<typename T>
inline djvBox<T, 2>::operator djvBox<double, 2>() const
{
    return djvBox<double, 2>(
        static_cast<double>(x),
        static_cast<double>(y),
        static_cast<double>(w),
        static_cast<double>(h));
}
