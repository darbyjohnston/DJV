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

//! \file djvStringUtil.cpp

#include <djvStringUtil.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvMath.h>

#include <QCoreApplication>

#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
// djvStringUtil
//------------------------------------------------------------------------------

djvStringUtil::~djvStringUtil()
{}

QStringList djvStringUtil::addQuotes(const QStringList & list)
{
    QStringList tmp;
    
    Q_FOREACH(const QString & s, list)
    {
        tmp += QString("\"%1\"").arg(s);
    }
    
    return tmp;
}

const QStringList & djvStringUtil::boolLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvStringUtil", "False") <<
        qApp->translate("djvStringUtil", "True");

    return data;
}

int djvStringUtil::cString(
    const QString & string,
    char *          out,
    int             maxLen,
    bool            terminate)
{
    DJV_ASSERT(maxLen >= 0);
    
    const QByteArray & b = string.toLatin1();

    const char * c = b.data();

    const int length =
        maxLen ?
        djvMath::min(
            string.length(),
            maxLen - static_cast<int>(terminate)) :
        (string.length() + static_cast<int>(terminate));

    int i = 0;

    for (; i < length; ++i)
    {
        out[i] = c[i];
    }

    if (terminate)
    {
        out[i++] = 0;
    }

    return i;
}

//------------------------------------------------------------------------------

bool djvSerialize(QStringList & in, QString & out)
{
    const int count = in.count();

    if (! count)
        return false;

    out = in.first();

    in.pop_front();

    return true;
}

bool djvSerialize(QStringList & in, unsigned int & out, const QStringList & labels)
{
    const int count = in.count();

    if (! count)
        return false;

    const int labelCount = labels.count();

    for (int i = 0; i < labelCount; ++i)
    {
        if (0 == in[0].compare(labels[i], Qt::CaseInsensitive))
        {
            in.pop_front();

            out = static_cast<unsigned int>(i);

            return true;
        }
    }

    return false;
}

QStringList & operator >> (QStringList & in, QString & out) throw (QString)
{
    if (! djvSerialize(in, out))
        throw in.join(" ");

    return in;
}

QStringList & operator >> (QStringList & in, QStringList & out) throw (QString)
{
    out = in;

    in.clear();

    return in;
}

QStringList & operator >> (QStringList & in, bool & out) throw (QString)
{
    unsigned int i = 0;

    if (! djvSerialize(in, i, djvStringUtil::boolLabels()))
        throw in.join(" ");

    out = i != 0;

    return in;
}

QStringList & operator >> (QStringList & in, int & out) throw (QString)
{
    QString tmp;
    in >> tmp;
    out = djvStringUtil::stringToInt<int>(tmp.toLatin1().data());
    return in;
}

QStringList & operator >> (QStringList & in, unsigned int & out) throw (QString)
{
    QString tmp;
    in >> tmp;
    out = djvStringUtil::stringToUInt<unsigned int>(tmp.toLatin1().data());
    return in;
}

QStringList & operator >> (QStringList & in, qint64 & out) throw (QString)
{
    QString tmp;
    in >> tmp;
    out = djvStringUtil::stringToInt<qint64>(tmp.toLatin1().data());
    return in;
}

QStringList & operator >> (QStringList & in, quint64 & out) throw (QString)
{
    QString tmp;
    in >> tmp;
    out = djvStringUtil::stringToUInt<quint64>(tmp.toLatin1().data());
    return in;
}

QStringList & operator >> (QStringList & in, double & out) throw (QString)
{
    QString tmp;
    in >> tmp;
    out = tmp.toDouble();
    return in;
}

QStringList & operator << (QStringList & out, const char * in)
{
    out += QString(in);

    return out;
}

QStringList & operator << (QStringList & out, bool in)
{
    out += djvStringUtil::boolLabels()[in];

    return out;
}

QStringList & operator << (QStringList & out, int in)
{
    char tmp[djvStringUtil::cStringLength];

    djvStringUtil::intToString<int>(in, tmp);

    out += tmp;

    return out;
}

QStringList & operator << (QStringList & out, unsigned int in)
{
    char tmp[djvStringUtil::cStringLength];

    djvStringUtil::intToString<unsigned int>(in, tmp);

    out += tmp;

    return out;
}

QStringList & operator << (QStringList & out, qint64 in)
{
    char tmp[djvStringUtil::cStringLength];

    djvStringUtil::intToString<qint64>(in, tmp);

    out += tmp;

    return out;
}

QStringList & operator << (QStringList & out, quint64 in)
{
    char tmp[djvStringUtil::cStringLength];

    djvStringUtil::intToString<quint64>(in, tmp);

    out += tmp;

    return out;
}

QStringList & operator << (QStringList & out, double in)
{
    out += QString::number(in);

    return out;
}

