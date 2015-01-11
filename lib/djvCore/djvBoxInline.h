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

//! \file djvBoxInline.h

#include <djvVectorUtil.h>

//------------------------------------------------------------------------------
// djvBox<T, D>
//------------------------------------------------------------------------------

template<typename T, int D>
const int djvBox<T, D>::dimension = D;

template<typename T, int D>
inline djvBox<T, D>::djvBox()
{}

template<typename T, int D>
inline djvBox<T, D>::djvBox(const djvBox<T, D> & in) :
    position(in.position),
    size    (in.size)
{}

template<typename T, int D>
inline djvBox<T, D>::djvBox(const djvVector<T, D> & position, const djvVector<T, D> & size) :
    position(position),
    size    (size)
{}

template<typename T, int D>
inline djvBox<T, D>::djvBox(const djvVector<T, D> & size) : size(size)
{}

template<typename T, int D>
inline bool djvBox<T, D>::isValid() const
{
    return djvVectorUtil::isSizeValid(size);
}

template<typename T, int D>
inline void djvBox<T, D>::zero()
{
    position.zero();
    size.zero();
}

template<typename T, int D>
inline djvVector<T, D> djvBox<T, D>::lowerRight() const
{
    return position + size;
}

template<typename T, int D>
inline void djvBox<T, D>::setLowerRight(const djvVector<T, D> & in)
{
    size = in - position;
}

template<typename T, int D>
inline djvBox<T, D> & djvBox<T, D>::operator *= (const djvVector<T, D> & in)
{
    position *= in;
    size *= in;
}

template<typename T, int D>
inline djvBox<T, D> & djvBox<T, D>::operator /= (const djvVector<T, D> & in)
{
    position /= in;
    size /= in;
}

template<typename T, int D>
inline djvBox<T, D> & djvBox<T, D>::operator *= (T in)
{
    position *= in;
    size *= in;
}

template<typename T, int D>
inline djvBox<T, D> & djvBox<T, D>::operator /= (T in)
{
    position /= in;
    size /= in;
}

//------------------------------------------------------------------------------

#define _BOX_FNC_OP(IN) \
    \
    template<typename T, int D> \
    inline djvBox<T, D> operator IN (const djvBox<T, D> & a, T b) \
    { \
        return djvBox<T, D>(a.position IN b, a.size IN b); \
    }

_BOX_FNC_OP(+)
_BOX_FNC_OP(-)
_BOX_FNC_OP(*)
_BOX_FNC_OP(/)

#define _BOX_FNC_OP2(IN) \
    \
    template<typename T, int D> \
    inline djvBox<T, D> operator IN (const djvBox<T, D> & a, const djvVector<T, D> & b) \
    { \
        return djvBox<T, D>(a.position IN b, a.size IN b); \
    }

_BOX_FNC_OP2(+)
_BOX_FNC_OP2(-)
_BOX_FNC_OP2(*)
_BOX_FNC_OP2(/)

template<typename T, int D>
inline QStringList & operator << (QStringList & out, const djvBox<T, D> & in)
{
    return out << in.position << in.size;
}

template<typename T, int D>
inline QStringList & operator >> (QStringList & in, djvBox<T, D> & out) throw (QString)
{
    return in >> out.position >> out.size;
}

template<typename T, int D>
inline bool operator == (const djvBox<T, D> & a, const djvBox<T, D> & b)
{
    return a.position == b.position && a.size == b.size;
}

template<typename T, int D>
inline bool operator != (const djvBox<T, D> & a, const djvBox<T, D> & b)
{
    return ! (a == b);
}

template<typename T, int D>
inline djvDebug & operator << (djvDebug & debug, const djvBox<T, D> & in)
{
    return debug << in.position << " " << in.size;
}
