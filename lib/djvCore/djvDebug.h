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

//! \file djvDebug.h

#ifndef DJV_DEBUG_H
#define DJV_DEBUG_H

#include <djvUtil.h>

#include <QVector>

struct djvDebugPrivate;

class QString;
class QStringList;

//! \addtogroup djvCoreMisc
//@{

//------------------------------------------------------------------------------
//! \class djvDebug
//!
//! This class provides debugging messages.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvDebug
{
public:

    //! Constructor.

    djvDebug(const QString & prefix, const QString &);

    //! Destructor.

    ~djvDebug();

    //! Add a message.

    void add(const QString &);

    //! This enumeration provides the line beginning and ending.

    enum LINE
    {
        LINE_BEGIN,
        LINE_END
    };

    //! Convert bits to a string.

    static QString bitsU8(quint8);

    //! Convert bits to a string.

    static QString bitsU16(quint16);

    //! Convert bits to a string.

    static QString bitsU32(quint32);

    djvDebug & operator << (LINE);

private:

    void init(const QString &);

    DJV_PRIVATE_COPY(djvDebug);
    
    djvDebugPrivate * _p;
};

//------------------------------------------------------------------------------

//! Start a debugging message block.

#define DJV_DEBUG(in) \
    \
    djvDebug _debug(__FILE__, in)

//! Print a debugging message.

#define DJV_DEBUG_PRINT(in) \
    \
    _debug << djvDebug::LINE_BEGIN << in << djvDebug::LINE_END

//! Convenience macro for declaring debugging operators.

#define DJV_DEBUG_OPERATOR(EXPORT, TYPE) \
    \
    EXPORT djvDebug & operator << (djvDebug &, const TYPE &)

DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, const char *);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, bool);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, int);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, unsigned int);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, qint64);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, quint64);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, double);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, QString);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, QStringList);
template<class T>
inline djvDebug & operator << (djvDebug &, const QVector<T> &);

//@} // djvCoreMisc

#include <djvDebugInline.h>

#endif // DJV_DEBUG_H

