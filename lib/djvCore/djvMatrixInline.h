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

//! \file djvMatrixInline.h

#include <djvBoxUtil.h>

//------------------------------------------------------------------------------
// djvMatrix<T, D>
//------------------------------------------------------------------------------

template<typename T, int D>
const int djvMatrix<T, D>::dimension = D;

template<typename T, int D>
const int djvMatrix<T, D>::dimensionX2 = D * D;

template<typename T, int D>
inline djvMatrix<T, D>::djvMatrix()
{
    identity();
}

template<typename T, int D>
inline djvMatrix<T, D>::djvMatrix(const djvMatrix<T, D> & in)
{
    for (int i = 0; i < dimensionX2; ++i)
    {
        e[i] = in.e[i];
    }
}

template<typename T, int D>
inline djvMatrix<T, D>::djvMatrix(const T * in)
{
    set(in);
}

template<typename T, int D>
inline void djvMatrix<T, D>::set(const T * in)
{
    for (int i = 0; i < dimensionX2; ++i)
    {
        e[i] = in[i];
    }
}

template<typename T, int D>
inline void djvMatrix<T, D>::identity()
{
    for (int i = 0; i < D; ++i)
    {
        for (int j = 0; j < D; ++j)
        {
            e[i * D + j] = (i == j) ? 1.0 : 0.0;
        }
    }
}

template<typename T, int D>
inline void djvMatrix<T, D>::zero()
{
    for (int i = 0; i < dimensionX2; ++i)
    {
        e[i] = 0.0;
    }
}

//------------------------------------------------------------------------------

template<typename T, int D>
inline djvMatrix<T, D> operator * (const djvMatrix<T, D> & a, const djvMatrix<T, D> & b)
{
    djvMatrix<T, D> out;

    out.zero();

    for (int i = 0; i < D; ++i)
    {
        for (int j = 0; j < D; ++j)
        {
            for (int k = 0; k < D; ++k)
            {
                out.e[i * D + j] += a.e[i * D + k] * b.e[k * D + j];
            }
        }
    }

    return out;
}

template<typename T, int D, int D2>
inline djvVector<T, D2> operator * (const djvMatrix<T, D> & a, const djvVector<T, D2> & b)
{
    //DJV_ASSERT((D - 1) == D2);

    djvVector<T, D2> out;

    for (int i = 0; i < D2; ++i)
    {
        int j = 0;

        for (; j < D2; ++j)
        {
            out.e[i] += a.e[j * D + i] * b.e[j];
        }

        out.e[i] += a.e[j * D + i];
    }

    return out;
}

template<typename T>
inline djvBox<T, 2> operator * (const djvMatrix<T, 3> & a, const djvBox<T, 2> & b)
{
    const djvVector<T, 2> lowerRight = b.lowerRight();

    djvVector<T, 2> pt[] =
    {
        b.position,
        b.position,
        lowerRight,
        b.position
    };
    pt[1].y = lowerRight.y;
    pt[3].x = lowerRight.x;

    djvBox<T, 2> out;

    for (int i = 0; i < 4; ++i)
    {
        pt[i] = a * pt[i];

        if (0 == i)
        {
            out.position = pt[i];
        }
        else
        {
            out = djvBoxUtil::expand(out, pt[i]);
        }
    }

    return out;
}

template<typename T, int D>
inline QStringList & operator << (QStringList & out, const djvMatrix<T, D> & in)
{
    for (int i = 0; i < in.dimensionX2; ++i)
    {
        out << in.e[i];
    }

    return out;
}

template<typename T, int D>
inline QStringList & operator >> (QStringList & in, djvMatrix<T, D> & out) throw (QString)
{
    for (int i = 0; i < out.dimensionX2; ++i)
    {
        in >> out.e[i];
    }

    return in;
}

template<typename T, int D>
inline bool operator == (const djvMatrix<T, D> & a, const djvMatrix<T, D> & b)
{
    for (int i = 0; i < a.dimensionX2; ++i)
    {
        if (a.e[i] != b.e[i])
        {
            return false;
        }
    }

    return true;
}

template<typename T, int D>
inline bool operator != (const djvMatrix<T, D> & a, const djvMatrix<T, D> & b)
{
    return ! (a == b);
}

template<typename T, int D>
inline djvDebug & operator << (djvDebug & debug, const djvMatrix<T, D> & in)
{
    debug << "\n";

    for (int i = 0; i < D; ++i)
    {
        for (int j = 0; j < D; ++j)
        {
            debug << in.e[i * D + j];

            if (j < D - 1)
            {
                debug << " ";
            }
        }

        if (i < D - 1)
        {
            debug << "\n";
        }
    }

    return debug;
}
