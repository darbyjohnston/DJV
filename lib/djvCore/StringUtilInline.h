//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <QStringList>

namespace djv
{
    namespace Core
    {
        inline bool StringUtil::find(
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

        inline bool StringUtil::findNoCase(
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

        template <typename T>
        inline bool Serialize(QStringList & in, T & out, const QStringList & label)
        {
            uint i = 0;
            if (!Serialize(in, i, label))
                return false;
            out = static_cast<T>(i);
            return true;
        }

        template <typename T>
        inline QStringList Serialize(T in, const QStringList & labels)
        {
            const int index = static_cast<int>(in);
            if (index >= labels.count())
            {
                return QStringList();
            }
            return QStringList() << labels[index];
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
        inline T StringUtil::stringToInt(const char * string, int maxLen)
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
        inline T StringUtil::stringToUInt(const char * string, int maxLen)
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
        inline int StringUtil::intToString(T value, char * out, int maxLen)
        {
            //DJV_ASSERT(maxLen > 0);
            const int max = maxLen - 1;
            const bool negative = !(0 == value || value > 0);
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

        inline std::wstring StringUtil::qToStdWString(const QString & value)
        {
#ifdef _MSC_VER
            return std::wstring((const wchar_t*)value.utf16());
#else
            return value.toStdWString();
#endif
        }

        inline QString StringUtil::stdWToQString(const std::wstring & value)
        {
#ifdef _MSC_VER
            return QString::fromUtf16((const ushort*)value.c_str());
#else
            return QString::fromStdWString(value);
#endif
        }

    } // namespace Core
} // namspace djv
