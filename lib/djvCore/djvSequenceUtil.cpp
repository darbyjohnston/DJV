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

//! \file djvSequenceUtil.cpp

#include <djvSequenceUtil.h>

#include <djvDebug.h>
#include <djvMath.h>

#include <QVector>

#include <limits>

#include <stdio.h>

//------------------------------------------------------------------------------
// djvSequenceUtil
//------------------------------------------------------------------------------

djvSequenceUtil::~djvSequenceUtil()
{}

qint64 djvSequenceUtil::findClosest(qint64 frame, const djvFrameList & frames)
{
    const int count = frames.count();

    if (! count)
        return -1;

    qint64 out = 0;
    qint64 min = 0;

    for (int i = 0; i < count; ++i)
    {
        const qint64 tmp = djvMath::abs(frame - frames[i]);

        if (tmp < min || 0 == i)
        {
            out = static_cast<qint64>(i);
            min = tmp;
        }
    }

    return out;
}

QString djvSequenceUtil::frameToString(qint64 frame, int pad)
{
    const bool    negative = frame < 0;
    const quint64 abs      = negative ? -frame : frame;
    
    char c[djvStringUtil::cStringLength] = "";

    const int length = djvStringUtil::intToString<qint64>(abs, c);

    QString p;
    
    if (negative)
    {
        p.append('-');
    }

    for (int i = 0; i < pad - length; ++i)
    {
        p.append('0');
    }
    
    for (int i = 0; i < length; ++i)
    {
        p.append(c[i]);
    }

    return p;
}

namespace
{

enum INC
{
    BREAK,
    ASCEND,
    DESCEND
};

inline INC inc(qint64 a, qint64 b)
{
    if (a + 1 == b)
    {
        return ASCEND;
    }
    else if (a - 1 == b)
    {
        return DESCEND;
    }

    return BREAK;
}

} // namespace

QString djvSequenceUtil::sequenceToString(const djvSequence & seq)
{
    //DJV_DEBUG("djvSequenceUtil::sequenceToString");
    //DJV_DEBUG_PRINT("frames = " << in.frames);

    QStringList out;

    djvFrameList frames = seq.frames;
    const int    count  = frames.count();
    const int    pad    = seq.pad;

    // Add the list end marker.

    static const qint64 marker = -std::numeric_limits<qint64>::min();

    frames += marker;

    // Sequence.

    qint64 tmp = frames[0];
    
    INC _inc = BREAK;

    for (int i = 0; i < count; ++i)
    {
        const INC inc_ = inc(frames[i], frames[i + 1]);

        //DJV_DEBUG_PRINT(frames[i] << " = " << inc_);

        if ((inc_ != _inc && _inc != BREAK) || BREAK == inc_)
        {
            if (tmp != frames[i])
            {
                out += frameToString(tmp, pad) +
                    "-" +
                    frameToString(frames[i], pad);
            }
            else
            {
                out += frameToString(frames[i], pad);
            }

            tmp = frames[i + 1];
            
            _inc = inc_;
        }
    }

    //DJV_DEBUG_PRINT("out = " << out);

    return out.join(",");
}

djvSequence djvSequenceUtil::stringToSequence(const QString & seq)
{
    //DJV_DEBUG("djvSequenceUtil::stringToSequence");
    //DJV_DEBUG_PRINT("seq = " << seq);

    djvSequence out;

    int pad = 0;

    Q_FOREACH(const QString & s, seq.split(',', QString::SkipEmptyParts))
    {
        const int count = s.count();
        
        if (count)
        {
            QString a, b;
        
            int j = 0;
            
            if ('-' == s[j])
            {
                a.append(s[j]);
                
                ++j;
            }
            
            while (j < count && s[j].isNumber())
            {
                a.append(s[j]);
                
                ++j;
            }
            
            if (j < count - 1 && '-' == s[j] && '-' == s[j + 1])
            {
                ++j;
            }
            else if (j < count && '-' == s[j])
            {
                ++j;
            }
            
            while (j < count)
            {
                b.append(s[j]);
                
                ++j;
            }
            
            //DJV_DEBUG_PRINT("a = " << a);
            //DJV_DEBUG_PRINT("b = " << b);
            
            int          _pad  = 0;
            const qint64 start = stringToFrame(a, &_pad);
            const qint64 end   = b.count() ? stringToFrame(b) : start;

            if (start < end)
            {
                const qint64 size = end - start + 1;

                for (qint64 j = start, k = 0;
                    j <= end && k < size; ++j,
                    ++k)
                {
                    out.frames += j;
                }
            }
            else
            {
                const qint64 size = start - end + 1;

                for (qint64 j = start, k = 0;
                    j >= end && k < size; --j,
                    ++k)
                {
                    out.frames += j;
                }
            }

            pad = djvMath::max(_pad, pad);
        }
    }

    out.pad = pad;

    //DJV_DEBUG_PRINT("out = " << out);
    //DJV_DEBUG_PRINT("pad = " << out.pad);

    return out;
}
