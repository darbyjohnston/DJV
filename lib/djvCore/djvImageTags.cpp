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

//! \file djvImageTags.cpp

#include <djvImageTags.h>

#include <djvAssert.h>
#include <djvDebug.h>

#include <QCoreApplication>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QVector>

//------------------------------------------------------------------------------
// djvImageTags::P
//------------------------------------------------------------------------------

struct djvImageTags::P
{
    typedef QPair<QString, QString> Pair;

    QVector<Pair> list;
};

//------------------------------------------------------------------------------
// djvImageTags
//------------------------------------------------------------------------------

djvImageTags::djvImageTags() :
    _p(new P)
{}

djvImageTags::djvImageTags(const djvImageTags & other) :
    _p(new P)
{
    *_p = *(other._p);
}

djvImageTags::~djvImageTags()
{
    delete _p;
}

void djvImageTags::add(const djvImageTags & in)
{
    Q_FOREACH(const P::Pair & pair, in._p->list)
    {
        add(pair.first, pair.second);
    }
}

void djvImageTags::add(const QString & key, const QString & value)
{
    for (int i = 0; i < _p->list.count(); ++i)
    {
        if (key == _p->list[i].first)
        {
            _p->list[i].second = value;
            
            return;
        }
    }

    _p->list += P::Pair(key, value);
}

QString djvImageTags::tag(const QString & key) const
{
    Q_FOREACH(const P::Pair & pair, _p->list)
    {
        if (key == pair.first)
        {
            return pair.second;
        }
    }

    return QString();
}

QStringList djvImageTags::keys() const
{
    QStringList out;

    Q_FOREACH(const P::Pair & pair, _p->list)
    {
        out += pair.first;
    }

    return out;
}

QStringList djvImageTags::values() const
{
    QStringList out;

    Q_FOREACH(const P::Pair & pair, _p->list)
    {
        out += pair.second;
    }

    return out;
}

int djvImageTags::count() const
{
    return _p->list.count();
}

bool djvImageTags::isValid(const QString & key)
{
    Q_FOREACH(const P::Pair & pair, _p->list)
    {
        if (key == pair.first)
        {
            return true;
        }
    }

    return false;
}

void djvImageTags::clear()
{
    _p->list.clear();
}

const QStringList & djvImageTags::tagLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvImageTags", "Project") <<
        qApp->translate("djvImageTags", "Creator") <<
        qApp->translate("djvImageTags", "Description") <<
        qApp->translate("djvImageTags", "Copyright") <<
        qApp->translate("djvImageTags", "Time") <<
        qApp->translate("djvImageTags", "UTC Offset") <<
        qApp->translate("djvImageTags", "Keycode") <<
        qApp->translate("djvImageTags", "Timecode");

    DJV_ASSERT(data.count() == TAGS_COUNT);

    return data;
}

djvImageTags & djvImageTags::operator = (const djvImageTags & other)
{
    if (&other != this)
    {
        *_p = *(other._p);
    }

    return *this;
}

QString & djvImageTags::operator [] (const QString & key)
{
    for (int i = 0; i < _p->list.count(); ++i)
    {
        if (key == _p->list[i].first)
        {
            return _p->list[i].second;
        }
    }

    _p->list += P::Pair(key, QString());
    
    return _p->list[_p->list.count() - 1].second;
}

QString djvImageTags::operator [] (const QString & key) const
{
    return tag(key);
}

//------------------------------------------------------------------------------

bool operator == (const djvImageTags & a, const djvImageTags & b)
{
    if (a.keys() != b.keys())
    {
        return false;
    }

    if (a.values() != b.values())
    {
        return false;
    }

    return true;
}

bool operator != (const djvImageTags & a, const djvImageTags & b)
{
    return ! (a == b);
}

djvDebug & operator << (djvDebug & debug, const djvImageTags & tags)
{
    QStringList tmp;

    Q_FOREACH(const QString & key, tags.keys())
    {
        tmp += QString("\"%1 = %2\"").arg(key).arg(tags[key]);
    }
    
    return debug << tmp.join(", ");
}

