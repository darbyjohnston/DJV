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

//! \file djvRangeUtilInline.h

#include <djvMath.h>

//------------------------------------------------------------------------------
// djvRangeUtil
//------------------------------------------------------------------------------

inline djvRangeUtil::~djvRangeUtil()
{}

template<typename T>
inline bool djvRangeUtil::intersect(T in, const djvRange<T> & range)
{
    return in >= range.min && in < range.max;
}

inline bool djvRangeUtil::intersect(int in, const djvRange<int> & range)
{
    return in >= range.min && in < range.max + 1;
}

template<typename T>
inline bool djvRangeUtil::intersect(
    const djvRange<T> & a,
    const djvRange<T> & b,
    djvRange<T> * out)
{
    out->min = djvMath::max(a.min, b.min);
    out->max = djvMath::min(a.max, b.max);
    return out->max > out->min;
}

inline bool djvRangeUtil::intersect(const djvRange<int> & a, const djvRange<int> & b,
    djvRange<int> * out)
{
    out->min = djvMath::max(a.min, b.min);
    out->max = djvMath::min(a.max, b.max);
    return (out->max + 1) > out->min;
}

template<typename T>
inline void djvRangeUtil::bound(
    const djvRange<T> & a,
    const djvRange<T> & b,
    djvRange<T> * out)
{
    out->min = djvMath::min(a.min, b.min);
    out->max = djvMath::max(a.max, b.max);
}

inline djvFrameRangeList djvRangeUtil::range(const djvFrameList & in)
{
    djvFrameRangeList out;

    if (in.count())
    {
        out += djvFrameRange(in[0], in[0]);
    }

    for (int i = 1; i < in.count(); ++i)
    {
        if (in[i] - 1 != out[out.count() - 1].max)
        {
            out += djvFrameRange(in[i], in[i]);
        }
        else
        {
            out[out.count() - 1].max = in[i];
        }
    }

    return out;
}

inline djvFrameList djvRangeUtil::frames(const djvFrameRange & in)
{
    djvFrameList out;
    
    for (qint64 i = in.min; i <= in.max; ++i)
    {
        out += i;
    }
    
    return out;
}

inline djvFrameList djvRangeUtil::frames(const djvFrameRangeList & in)
{
    djvFrameList out;
    
    for (int i = 0; i < in.count(); ++i)
    {
        out += frames(in[i]);
    }
    
    return out;
}
