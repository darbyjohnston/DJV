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

//! \file djvStringUtil.h

#ifndef DJV_STRING_UTIL_H
#define DJV_STRING_UTIL_H

#include <djvConfig.h>
#include <djvCoreExport.h>

#include <QString>
#include <QVector>

//! \addtogroup djvCoreMisc
//@{

//------------------------------------------------------------------------------
//! \class djvStringUtil
//!
//! This class provides string utilities.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvStringUtil
{
public:

    //! Destructor.
    
    virtual ~djvStringUtil() = 0;
    
    //! Find a string in a list.

    static inline bool find(
        const QString &     string,
        const QStringList & list,
        int *               index  = 0);

    //! Find a string in a list, case insensitive.

    static inline bool findNoCase(
        const QString &     string,
        const QStringList & list,
        int *               index = 0);

    //! Add double quotes to each string in a list.
    
    static QStringList addQuotes(const QStringList &);
    
    //! Get a label for a value.

    template<typename T>
    static inline QStringList label(const T &);

    //! Get the boolean labels.

    static const QStringList & boolLabels();

    //! C-string conversion.

    static int cString(
        const QString & string,
        char *          cString,
        int             maxLen    = 0,
        bool            terminate = true);

    //! Default C-string length.

    static const int cStringLength = 256;

    //! Convert a string to an integer.

    template<typename T>
    static inline T stringToInt(
        const char * string,
        int          maxLen = cStringLength);

    //! Convert a string to an unsigned integer.

    template<typename T>
    static inline T stringToUInt(
        const char * string,
        int          maxLen = cStringLength);

    //! Convert an integer to a string.

    template<typename T>
    static inline int intToString(
        T      value,
        char * string,
        int    maxLen = cStringLength);
};

//------------------------------------------------------------------------------

//! This macro provides string formatting.

#if defined(DJV_WINDOWS)

#define SNPRINTF ::sprintf_s

#else // DJV_WINDOWS

#define SNPRINTF ::snprintf

#endif // DJV_WINDOWS

DJV_CORE_EXPORT bool djvSerialize(QStringList &, QString &);
DJV_CORE_EXPORT bool djvSerialize(QStringList &, unsigned int &, const QStringList & labels);
template <typename T>
inline bool djvSerialize(QStringList &, T &, const QStringList & labels);
template <typename T>
inline QStringList djvSerialize(T, const QStringList & labels);

DJV_CORE_EXPORT QStringList & operator >> (QStringList &, QString &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &, QStringList &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &, bool &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &, int &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &, unsigned int &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &, qint64 &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &, quint64 &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &, double &) throw (QString);
template <typename T>
inline QStringList & operator >> (QStringList &, QVector<T> &) throw (QString);

DJV_CORE_EXPORT QStringList & operator << (QStringList &, const char *);
DJV_CORE_EXPORT QStringList & operator << (QStringList &, bool);
DJV_CORE_EXPORT QStringList & operator << (QStringList &, int);
DJV_CORE_EXPORT QStringList & operator << (QStringList &, unsigned int);
DJV_CORE_EXPORT QStringList & operator << (QStringList &, qint64);
DJV_CORE_EXPORT QStringList & operator << (QStringList &, quint64);
DJV_CORE_EXPORT QStringList & operator << (QStringList &, double);
template <typename T>
inline QStringList & operator << (QStringList &, const QVector<T> &);

//! This macros provides string serialize operators.

#define DJV_STRING_OPERATOR(EXPORT, TYPE) \
    \
    EXPORT QStringList & operator >> (QStringList &, TYPE &) throw (QString); \
    \
    EXPORT QStringList & operator << (QStringList &, const TYPE &)
    
//! This macro provides string serialize operators.

#define _DJV_STRING_OPERATOR_LABEL(TYPE, LABEL) \
    \
    QStringList & operator >> (QStringList & in, TYPE & out) \
        throw (QString) \
    { \
        if (! djvSerialize(in, out, LABEL)) \
        { \
            throw in.count() ? in[0] : QString(); \
        } \
        \
        return in; \
    } \
    \
    QStringList & operator << (QStringList & out, const TYPE & in) \
    { \
        return out << djvSerialize(in, LABEL); \
    }

//@} // djvCoreMisc

#include <djvStringUtilInline.h>

#endif // DJV_STRING_H

