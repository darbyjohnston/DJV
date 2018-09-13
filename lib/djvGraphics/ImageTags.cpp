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

#include <djvGraphics/ImageTags.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

#include <QCoreApplication>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QVector>

using namespace djv;

namespace djv
{
    namespace Graphics
    {
        struct ImageTags::Private
        {
            typedef QPair<QString, QString> Pair;
            QVector<Pair> list;
        };

        ImageTags::ImageTags() :
            _p(new Private)
        {}

        ImageTags::ImageTags(const ImageTags & other) :
            _p(new Private)
        {
            *_p = *(other._p);
        }

        ImageTags::~ImageTags()
        {}

        void ImageTags::add(const ImageTags & in)
        {
            Q_FOREACH(const Private::Pair & pair, in._p->list)
            {
                add(pair.first, pair.second);
            }
        }

        void ImageTags::add(const QString & key, const QString & value)
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

        QString ImageTags::tag(const QString & key) const
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

        QStringList ImageTags::keys() const
        {
            QStringList out;
            Q_FOREACH(const Private::Pair & pair, _p->list)
            {
                out += pair.first;
            }
            return out;
        }

        QStringList ImageTags::values() const
        {
            QStringList out;
            Q_FOREACH(const Private::Pair & pair, _p->list)
            {
                out += pair.second;
            }
            return out;
        }

        int ImageTags::count() const
        {
            return _p->list.count();
        }

        bool ImageTags::isValid(const QString & key)
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

        void ImageTags::clear()
        {
            _p->list.clear();
        }

        const QStringList & ImageTags::tagLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::ImageTags", "Project") <<
                qApp->translate("djv::Graphics::ImageTags", "Creator") <<
                qApp->translate("djv::Graphics::ImageTags", "Description") <<
                qApp->translate("djv::Graphics::ImageTags", "Copyright") <<
                qApp->translate("djv::Graphics::ImageTags", "Time") <<
                qApp->translate("djv::Graphics::ImageTags", "UTC Offset") <<
                qApp->translate("djv::Graphics::ImageTags", "Keycode") <<
                qApp->translate("djv::Graphics::ImageTags", "Timecode");
            DJV_ASSERT(data.count() == TAGS_COUNT);
            return data;
        }

        ImageTags & ImageTags::operator = (const ImageTags & other)
        {
            if (&other != this)
            {
                *_p = *(other._p);
            }
            return *this;
        }

        QString & ImageTags::operator [] (const QString & key)
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

        QString ImageTags::operator [] (const QString & key) const
        {
            return tag(key);
        }

        bool operator == (const ImageTags & a, const ImageTags & b)
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

        bool operator != (const ImageTags & a, const ImageTags & b)
        {
            return !(a == b);
        }

    } // namespace Graphics
} // namespace djv

djvDebug & operator << (djvDebug & debug, const Graphics::ImageTags & tags)
{
    QStringList tmp;
    Q_FOREACH(const QString & key, tags.keys())
    {
        tmp += QString("\"%1 = %2\"").arg(key).arg(tags[key]);
    }    
    return debug << tmp.join(", ");
}

