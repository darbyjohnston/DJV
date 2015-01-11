
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

//! \file djvSequence.cpp

#include <djvSequence.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvListUtil.h>
#include <djvMath.h>
#include <djvSequenceUtil.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvSequence
//------------------------------------------------------------------------------

const QStringList & djvSequence::compressLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvSequence", "Off") <<
        qApp->translate("djvSequence", "Sparse") <<
        qApp->translate("djvSequence", "Range");

    DJV_ASSERT(data.count() == COMPRESS_COUNT);

    return data;
}

djvSequence::djvSequence() :
    pad(0)
{}

djvSequence::djvSequence(const djvFrameList & frames, int pad, const djvSpeed & speed) :
    frames(frames),
    pad   (pad),
    speed (speed)
{}

djvSequence::djvSequence(qint64 start, qint64 end, int pad, const djvSpeed & speed) :
    pad  (pad),
    speed(speed)
{
    setFrames(start, end);
}

namespace
{

// Set the maximum number of frames large enough for a two hour movie.

qint64 _maxFrames = djvSequence::maxFramesDefault();

} // namespace

void djvSequence::setFrames(qint64 start, qint64 end)
{
    if (start < end)
    {
        const qint64 size = djvMath::min<qint64>(end - start + 1, _maxFrames);

        frames.resize(size);

        for (qint64 i = start, j = 0; i <= end && j < size; ++i, ++j)
        {
            frames[j] = i;
        }
    }
    else
    {
        const qint64 size = djvMath::min<qint64>(start - end + 1, _maxFrames);

        frames.resize(size);

        for (qint64 i = start, j = 0; i >= end && j < size; --i, ++j)
        {
            frames[j] = i;
        }
    }
}

namespace
{

bool compare(qint64 a, qint64 b)
{
    return a < b;
}

} // namespace

void djvSequence::sort()
{
    qSort(frames.begin(), frames.end(), compare);
}

qint64 djvSequence::maxFramesDefault()
{
    return 120 * 60 * 24;
}

qint64 djvSequence::maxFrames()
{
    return _maxFrames;
}

void djvSequence::setMaxFrames(qint64 size)
{
    _maxFrames = size;
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvSequence::COMPRESS, djvSequence::compressLabels())

QStringList & operator << (QStringList & out, const djvSequence & in)
{
    return out << djvSequenceUtil::sequenceToString(in);
}

QStringList & operator >> (QStringList & in, djvSequence & out) throw (QString)
{
    QString tmp;
    in >> tmp;
    out = djvSequenceUtil::stringToSequence(tmp);
    return in;
}

djvDebug & operator << (djvDebug & debug, const djvSequence & in)
{
    return debug <<
        djvStringUtil::label(in) <<
        "@" <<
        djvSpeed::speedToFloat(in.speed);
}

djvDebug & operator << (djvDebug & debug, const djvSequence::COMPRESS & in)
{
    return debug << djvStringUtil::label(in);
}

