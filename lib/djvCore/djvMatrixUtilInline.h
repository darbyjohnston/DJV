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

//! \file djvMatrixUtilInline.h

//------------------------------------------------------------------------------
// djvMatrixUtil
//------------------------------------------------------------------------------

inline djvMatrixUtil::~djvMatrixUtil()
{}

template<typename T, int D>
inline djvMatrix<T, D> djvMatrixUtil::transpose(const djvMatrix<T, D> & in)
{
    djvMatrix<T, D> out;

    for (int i = 0; i < D; ++i)
    {
        for (int j = 0; j < D; ++j)
        {
            out.e[j * D + i] = in.e[i * D + j];
        }
    }

    return out;
}

template<typename T, int D, int D2>
inline djvMatrix<T, D> djvMatrixUtil::translate(const djvVector<T, D2> & in)
{
    //DJV_ASSERT((D - 1) == D2);

    djvMatrix<T, D> out;

    for (int i = 0; i < D2; ++i)
    {
        out.e[(D - 1) * D + i] = in.e[i];
    }

    return out;
}

inline djvMatrix3f djvMatrixUtil::translate3f(const djvVector2f & in)
{
    return translate<double, 3, 2>(in);
}

template<typename T, int D, int D2>
inline djvMatrix<T, D> djvMatrixUtil::scale(const djvVector<T, D2> & in)
{
    //DJV_ASSERT((D - 1) == D2);

    djvMatrix<T, D> out;

    for (int i = 0; i < D2; ++i)
    {
        out.e[i * D + i] = in.e[i];
    }

    return out;
}

inline djvMatrix3f djvMatrixUtil::scale3f(const djvVector2f & in)
{
    return scale<double, 3, 2>(in);
}

inline djvMatrix3f djvMatrixUtil::rotate3f(double in)
{
    const double cos = djvMath::cos(djvMath::degreesToRadians(in));
    const double sin = djvMath::sin(djvMath::degreesToRadians(in));

    return djvMatrix3f(
        cos,  sin, 0,
        -sin, cos, 0,
        0,    0,   1);
}

template <typename T, typename T2, int D>
inline djvMatrix<T2, D> djvMatrixUtil::convert(const djvMatrix<T, D> & in)
{
    djvMatrix<T2, D> out;

    for (int i = 0; i < out.dimensionX2; ++i)
    {
        out.e[i] = T2(in.e[i]);
    }

    return out;
}

//! \todo This matrix conversion is probably wrong.

template <typename T>
inline djvMatrix<T, 4> djvMatrixUtil::matrix4(const djvMatrix<T, 3> & in)
{
    return djvMatrix<T, 4>(
        in.e[0], in.e[1], in.e[2], 0.0,
        in.e[3], in.e[4], in.e[5], 0.0,
        0.0,     0.0,     in.e[8], 0.0,
        in.e[6], in.e[7], 0.0,     1.0);
}
