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

//! \file djvStringUtilInline.h

#include <QStringList>

//------------------------------------------------------------------------------
// djvStringUtil
//------------------------------------------------------------------------------

inline bool djvStringUtil::find(
    const QString &     a,
    const QStringList & b,
    int *               index)
{
    const int count = b.count();

    for (int i = 0; i < count; ++i)
    {
        if (a == b[i])
        {
            if (index)
            {
                *index = i;
            }
            
            return true;
        }
    }

    return false;
}

inline bool djvStringUtil::findNoCase(
    const QString &     a,
    const QStringList & b,
    int *               index)
{
    const int count = b.count();

    const QString tmp = a.toLower();

    for (int i = 0; i < count; ++i)
    {
        if (tmp == b[i].toLower())
        {
            if (index)
            {
                *index = i;
            }

            return true;
        }
    }
    
    return false;
}

template<typename T>
inline QStringList djvStringUtil::label(const T & value)
{
    QStringList out;

    out << value;
    
    return out;
}

//------------------------------------------------------------------------------

template <typename T>
inline bool djvSerialize(QStringList & in, T & out, const QStringList & label)
{
    uint i = 0;

    if (! djvSerialize(in, i, label))
        return false;

    out = static_cast<T>(i);

    return true;
}

template <typename T>
inline QStringList djvSerialize(T in, const QStringList & labels)
{
    const int index = static_cast<int>(in);

    if (index >= labels.count())
    {
        return QStringList();
    }

    return QStringList() << labels[index];
}

template <typename T>
inline QStringList & operator >> (QStringList & in, QVector<T> & out) throw (QString)
{
    while (in.count())
    {
        T value;
        in >> value;
        out += value;
    }

    return in;
}

template <typename T>
inline QStringList & operator << (QStringList & out, const QVector<T> & in)
{
    for (int i = 0; i < in.count(); ++i)
    {
        out << in[i];
    }

    return out;
}

namespace
{

// Valid character lookup tables.

const int _stringToInt[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0
};

} // namespace

template<typename T>
inline T djvStringUtil::stringToInt(const char * string, int maxLen)
{
    bool negative = false;

    if (*string)
    {
        if ('-' == *string)
        {
            negative = true;

            ++string;
        }
    }

    T out = 0;

    while (_stringToInt[static_cast<int>(*string)] && maxLen--)
    {
        out *= 10;
        out += *string++ - '0';
    }

    return negative ? -out : out;
}

template<typename T>
inline T djvStringUtil::stringToUInt(const char * string, int maxLen)
{
    T out = 0;

    while (_stringToInt[static_cast<int>(*string)] && maxLen--)
    {
        out *= 10;
        out += *string++ - '0';
    }

    return out;
}

template<typename T>
inline int djvStringUtil::intToString(T value, char * out, int maxLen)
{
    //DJV_ASSERT(maxLen > 0);

    const int max = maxLen - 1;

    const bool negative = ! (0 == value || value > 0);

    int count = 0;
    T tmp = value;

    do
    {
        ++count;
        tmp /= 10;
    } while (tmp && count < max);

    if (negative)
    {
        ++count;
        out[0] = '-';
    }

    if (count > 0)
    {
        const int end = negative ? 1 : 0;

        for (int i = count - 1; i >= end; --i, value /= 10)
        {
            if (negative)
            {
                out[i] = '0' - (value % 10);
            }
            else
            {
                out[i] = '0' + (value % 10);
            }
        }
    }

    out[count] = 0;

    return count;
}
