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

//! \file djvVectorUtil.cpp

#include <djvVectorUtil.h>

#include <djvMath.h>

#include <QPoint>
#include <QSize>

//------------------------------------------------------------------------------
// djvVectorUtil
//------------------------------------------------------------------------------

djvVectorUtil::~djvVectorUtil()
{}

void djvVectorUtil::sphericalToCartesian(const djvVector3f & in, djvVector3f * out)
{
    const double tmp = djvMath::sin(in.x);

    out->x = in.z * tmp * djvMath::cos(in.y);
    out->y = in.z * tmp * djvMath::sin(in.y);
    out->z = in.z * djvMath::cos(in.x);
}

void djvVectorUtil::cartesianToSpherical(const djvVector3f & in, djvVector3f * out)
{
    out->z = djvMath::sqrt(in.x * in.x + in.y * in.y + in.z * in.z);
    out->x = out->z != 0.0 ? djvMath::arcCos(in.z / out->z) : 0.0;
    out->y = in.x != 0.0 ? djvMath::arcTan(in.y / in.x) : 0.0;
}

QPoint djvVectorUtil::toQPoint(const djvVector2i & in)
{
    return QPoint(in.x, in.y);
}

QSize djvVectorUtil::toQSize(const djvVector2i & in)
{
    return QSize(in.x, in.y);
}

djvVector2i djvVectorUtil::fromQPoint(const QPoint & in)
{
    return djvVector2i(in.x(), in.y());
}

djvVector2i djvVectorUtil::fromQSize(const QSize & in)
{
    return djvVector2i(in.width(), in.height());
}

