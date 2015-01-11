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

//! \file djvMatrix3Inline.h

//------------------------------------------------------------------------------
// djvMatrix<T, 3>
//------------------------------------------------------------------------------

template<typename T>
const int djvMatrix<T, 3>::dimension = 3;
template<typename T>
const int djvMatrix<T, 3>::dimensionX2 = 3 * 3;

template<typename T>
inline djvMatrix<T, 3>::djvMatrix()
{
    identity();
}

template<typename T>
inline djvMatrix<T, 3>::djvMatrix(const djvMatrix<T, 3> & in)
{
    for (int i = 0; i < dimensionX2; ++i)
    {
        e[i] = in.e[i];
    }
}

template<typename T>
inline djvMatrix<T, 3>::djvMatrix(
    T _0, T _1, T _2,
    T _3, T _4, T _5,
    T _6, T _7, T _8)
{
    set(_0, _1, _2,
        _3, _4, _5,
        _6, _7, _8);
}

template<typename T>
inline djvMatrix<T, 3>::djvMatrix(const T * in)
{
    set(in);
}

template<typename T>
inline void djvMatrix<T, 3>::set(
    T _0, T _1, T _2,
    T _3, T _4, T _5,
    T _6, T _7, T _8)
{
    T * p = djvMatrix<T, 3>::e;
    p[0] = _0;
    p[1] = _1;
    p[2] = _2;
    p[3] = _3;
    p[4] = _4;
    p[5] = _5;
    p[6] = _6;
    p[7] = _7;
    p[8] = _8;
}

template<typename T>
inline void djvMatrix<T, 3>::set(const T * in)
{
    for (int i = 0; i < dimensionX2; ++i)
    {
        e[i] = in[i];
    }
}

template<typename T>
inline void djvMatrix<T, 3>::identity()
{
    static const T zero = static_cast<T>(0.0);
    static const T one  = static_cast<T>(1.0);

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            e[i * 3 + j] = (i == j) ? one : zero;
        }
    }
}

template<typename T>
inline void djvMatrix<T, 3>::zero()
{
    static const T zero = static_cast<T>(0.0);

    for (int i = 0; i < dimensionX2; ++i)
    {
        e[i] = zero;
    }
}

template<typename T>
inline djvMatrix<T, 3> & djvMatrix<T, 3>::operator = (const djvMatrix<T, 3> & in)
{
    if (&in != this)
    {
        for (int i = 0; i < dimensionX2; ++i)
        {
            e[i] = in.e[i];
        }
    }

    return *this;
}
