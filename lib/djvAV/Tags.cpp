//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvAV/Tags.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

#include <QCoreApplication>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QVector>

namespace djv
{
    namespace AV
    {
        struct Tags::Private
        {
            typedef QPair<QString, QString> Pair;
            QVector<Pair> list;
        };

        Tags::Tags() :
            _p(new Private)
        {}

        Tags::Tags(const Tags & other) :
            _p(new Private)
        {
            *_p = *(other._p);
        }

        Tags::~Tags()
        {}

        void Tags::add(const Tags & in)
        {
            Q_FOREACH(const Private::Pair & pair, in._p->list)
            {
                add(pair.first, pair.second);
            }
        }

        void Tags::add(const QString & key, const QString & value)
        {
            for (int i = 0; i < _p->list.count(); ++i)
            {
                if (key == _p->list[i].first)
                {
                    _p->list[i].second = value;
                    return;
                }
            }
            _p->list += Private::Pair(key, value);
        }

        QString Tags::tag(const QString & key) const
        {
            Q_FOREACH(const Private::Pair & pair, _p->list)
            {
                if (key == pair.first)
                {
                    return pair.second;
                }
            }
            return QString();
        }

        QStringList Tags::keys() const
        {
            QStringList out;
            Q_FOREACH(const Private::Pair & pair, _p->list)
            {
                out += pair.first;
            }
            return out;
        }

        QStringList Tags::values() const
        {
            QStringList out;
            Q_FOREACH(const Private::Pair & pair, _p->list)
            {
                out += pair.second;
            }
            return out;
        }

        int Tags::count() const
        {
            return _p->list.count();
        }

        bool Tags::isValid(const QString & key)
        {
            Q_FOREACH(const Private::Pair & pair, _p->list)
            {
                if (key == pair.first)
                {
                    return true;
                }
            }
            return false;
        }

        void Tags::clear()
        {
            _p->list.clear();
        }

        const QStringList & Tags::tagLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::Tags", "Project") <<
                qApp->translate("djv::AV::Tags", "Creator") <<
                qApp->translate("djv::AV::Tags", "Description") <<
                qApp->translate("djv::AV::Tags", "Copyright") <<
                qApp->translate("djv::AV::Tags", "Time") <<
                qApp->translate("djv::AV::Tags", "UTC Offset") <<
                qApp->translate("djv::AV::Tags", "Keycode") <<
                qApp->translate("djv::AV::Tags", "Timecode");
            DJV_ASSERT(data.count() == TAGS_COUNT);
            return data;
        }

        Tags & Tags::operator = (const Tags & other)
        {
            if (&other != this)
            {
                *_p = *(other._p);
            }
            return *this;
        }

        QString & Tags::operator [] (const QString & key)
        {
            for (int i = 0; i < _p->list.count(); ++i)
            {
                if (key == _p->list[i].first)
                {
                    return _p->list[i].second;
                }
            }
            _p->list += Private::Pair(key, QString());
            return _p->list[_p->list.count() - 1].second;
        }

        QString Tags::operator [] (const QString & key) const
        {
            return tag(key);
        }

    } // namespace AV

    bool operator == (const AV::Tags & a, const AV::Tags & b)
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

    bool operator != (const AV::Tags & a, const AV::Tags & b)
    {
        return !(a == b);
    }

    Core::Debug & operator << (Core::Debug & debug, const AV::Tags & tags)
    {
        QStringList tmp;
        Q_FOREACH(const QString & key, tags.keys())
        {
            tmp += QString("\"%1 = %2\"").arg(key).arg(tags[key]);
        }
        return debug << tmp.join(", ");
    }

} // namespace djv
