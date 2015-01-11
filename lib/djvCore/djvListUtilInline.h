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

//! \file djvListUtilInline.h

#include <algorithm>

//------------------------------------------------------------------------------
// djvListUtil
//------------------------------------------------------------------------------

inline djvListUtil::~djvListUtil()
{}

template<typename A, typename B>
inline void djvListUtil::convert(const QVector<A> & in, QVector<B> & out)
{
    const int count = in.count();

    out.resize(count);

    for (int i = 0; i < count; ++i)
    {
        out[i] = static_cast<B>(in[i]);
    }
}

template<typename A, typename B>
inline QVector<B> djvListUtil::convert(const QVector<A> & in)
{
    QVector<B> out;
    
    const int count = in.count();

    out.resize(count);

    for (int i = 0; i < count; ++i)
    {
        out[i] = static_cast<B>(in[i]);
    }
    
    return out;
}

template<typename A, typename B>
inline void djvListUtil::convertAndAppend(const QVector<A> & in, QVector<B> & out)
{
    const int inCount  = in.count();
    const int outCount = out.count();

    out.resize(inCount + outCount);

    for (int i = 0; i < inCount; ++i)
    {
        out[outCount + i] = static_cast<B>(in[i]);
    }
}
