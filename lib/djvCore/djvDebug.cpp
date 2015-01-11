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

//! \file djvDebug.cpp

#include <djvDebug.h>

#include <djvStringUtil.h>
#include <djvTimer.h>

#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
// djvDebug::P
//------------------------------------------------------------------------------

struct djvDebug::P
{
    QString  tmp;
    djvTimer timer;
};

//------------------------------------------------------------------------------
// djvDebug
//------------------------------------------------------------------------------

namespace
{

// Note that the indentation level is a global variable.

int indent = 0;

} // namespace

void djvDebug::init(const QString & in)
{
    _p = new P;

    *this << LINE_BEGIN << in << LINE_END;
    *this << LINE_BEGIN << "{" << LINE_END;

    indent += 4;
}

djvDebug::djvDebug(const QString & prefix, const QString & in)
{
    init(prefix + ": " + in);
}

djvDebug::~djvDebug()
{
    _p->timer.check();

    indent -= 4;

    *this <<
        LINE_BEGIN <<
        "} (" <<
        _p->timer.seconds() <<
        " seconds, " <<
        _p->timer.fps() <<
        " fps)" <<
        LINE_END;

    delete _p;
}

void djvDebug::add(const QString & in)
{
    _p->tmp += in;
}

namespace
{

QString bits(quint32 in, int size)
{
    QString out;

    for (int i = 0; i < size; ++i)
    {
        out += ((in >> i) & 1) ? '1' : '0';
    }

    return out;
}

} // namespace

QString djvDebug::bitsU8(quint8  in)
{
    return bits(in, 8);
}

QString djvDebug::bitsU16(quint16 in)
{
    return bits(in, 16);
}

QString djvDebug::bitsU32(quint32 in)
{
    return bits(in, 32);
}

djvDebug & djvDebug::operator << (LINE in)
{
    switch (in)
    {
        case LINE_BEGIN:
        
            _p->tmp = "debug " + QString(indent, ' ');
            
            break;

        case LINE_END:
        
            ::printf("%s\n", _p->tmp.toLatin1().data());
            
            ::fflush(stdout);
            
            break;
    }

    return *this;
}

//------------------------------------------------------------------------------

djvDebug & operator << (djvDebug & debug, const char * in)
{
    debug.add(in);

    return debug;
}

djvDebug & operator << (djvDebug & debug, bool in)
{
    static const QStringList label = djvStringUtil::boolLabels();

    return debug << label[in];
}

djvDebug & operator << (djvDebug & debug, int in)
{
    return debug << QString::number(in);
}

djvDebug & operator << (djvDebug & debug, unsigned int in)
{
    return debug << QString::number(in);
}

djvDebug & operator << (djvDebug & debug, qint64 in)
{
    return debug << QString::number(in);
}

djvDebug & operator << (djvDebug & debug, quint64 in)
{
    return debug << QString::number(in);
}

djvDebug & operator << (djvDebug & debug, double in)
{
    return debug << QString::number(in);
}

djvDebug & operator << (djvDebug & debug, const QString & in)
{
    debug.add(in);

    return debug;
}

djvDebug & operator << (djvDebug & debug, const QStringList & in)
{
    return debug << in.join(" ");
}
