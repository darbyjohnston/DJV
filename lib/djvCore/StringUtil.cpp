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

#include <djvCore/StringUtil.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Math.h>

#include <QCoreApplication>

#include <stdio.h>
#include <stdlib.h>

namespace djv
{
    namespace Core
    {
        StringUtil::~StringUtil()
        {}

        QStringList StringUtil::addQuotes(const QStringList & list)
        {
            QStringList tmp;
            Q_FOREACH(const QString & s, list)
            {
                tmp += QString("\"%1\"").arg(s);
            }
            return tmp;
        }

        const QStringList & StringUtil::boolLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Core::StringUtil", "False") <<
                qApp->translate("djv::Core::StringUtil", "True");
            return data;
        }

        int StringUtil::cString(
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
                Math::min(
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

        bool Serialize(QStringList & in, QString & out)
        {
            const int count = in.count();
            if (!count)
                return false;
            out = in.first();
            in.pop_front();
            return true;
        }

        bool Serialize(QStringList & in, unsigned int & out, const QStringList & labels)
        {
            const int count = in.count();
            if (!count)
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

    } // namespace Core

    QStringList & operator >> (QStringList & in, QString & out)
    {
        if (!Core::Serialize(in, out))
            throw in.join(" ");
        return in;
    }

    QStringList & operator >> (QStringList & in, QStringList & out)
    {
        out = in;
        in.clear();
        return in;
    }

    QStringList & operator >> (QStringList & in, bool & out)
    {
        unsigned int i = 0;
        if (!Core::Serialize(in, i, Core::StringUtil::boolLabels()))
            throw in.join(" ");
        out = i != 0;
        return in;
    }

    QStringList & operator >> (QStringList & in, int & out)
    {
        QString tmp;
        in >> tmp;
        out = tmp.toInt();
        return in;
    }

    QStringList & operator >> (QStringList & in, unsigned int & out)
    {
        QString tmp;
        in >> tmp;
        out = tmp.toUInt();
        return in;
    }

    QStringList & operator >> (QStringList & in, qint64 & out)
    {
        QString tmp;
        in >> tmp;
        out = tmp.toLongLong();
        return in;
    }

    QStringList & operator >> (QStringList & in, quint64 & out)
    {
        QString tmp;
        in >> tmp;
        out = tmp.toULongLong();
        return in;
    }

    QStringList & operator >> (QStringList & in, float & out)
    {
        QString tmp;
        in >> tmp;
        out = tmp.toFloat();
        return in;
    }

    QStringList & operator >> (QStringList & in, double & out)
    {
        QString tmp;
        in >> tmp;
        out = tmp.toDouble();
        return in;
    }

    QStringList & operator >> (QStringList & in, QVector<bool> & out)
    {
        while (in.count())
        {
            bool value = false;
            in >> value;
            out += value;
        }
        return in;
    }

    QStringList & operator << (QStringList & out, const char * in)
    {
        out += QString(in);
        return out;
    }

    QStringList & operator << (QStringList & out, bool in)
    {
        out += Core::StringUtil::boolLabels()[in];
        return out;
    }

    QStringList & operator << (QStringList & out, int in)
    {
        char tmp[Core::StringUtil::cStringLength];
        Core::StringUtil::intToString<int>(in, tmp);
        out += tmp;
        return out;
    }

    QStringList & operator << (QStringList & out, unsigned int in)
    {
        char tmp[Core::StringUtil::cStringLength];
        Core::StringUtil::intToString<unsigned int>(in, tmp);
        out += tmp;
        return out;
    }

    QStringList & operator << (QStringList & out, qint64 in)
    {
        char tmp[Core::StringUtil::cStringLength];
        Core::StringUtil::intToString<qint64>(in, tmp);
        out += tmp;
        return out;
    }

    QStringList & operator << (QStringList & out, quint64 in)
    {
        char tmp[Core::StringUtil::cStringLength];
        Core::StringUtil::intToString<quint64>(in, tmp);
        out += tmp;
        return out;
    }

    QStringList & operator << (QStringList & out, float in)
    {
        out += QString::number(in);
        return out;
    }

    QStringList & operator << (QStringList & out, double in)
    {
        out += QString::number(in);
        return out;
    }

    QStringList & operator << (QStringList & out, const QVector<bool>& in)
    {
        for (int i = 0; i < in.count(); ++i)
        {
            out << in[i];
        }
        return out;
    }

} // namespace djv
