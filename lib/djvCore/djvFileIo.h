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

//! \file djvFileIo.h

#ifndef DJV_FILE_IO_H
#define DJV_FILE_IO_H

#include <djvError.h>
#include <djvStringUtil.h>
#include <djvUtil.h>

#include <QMetaType>

struct djvFileIoPrivate;

//! \addtogroup djvCoreFile
//@{

//------------------------------------------------------------------------------
//! \class djvFileIo
//!
//! This class provides file I/O.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvFileIo
{
    Q_GADGET
    Q_ENUMS(MODE)
    
public:

    //! Constructor.

    djvFileIo();

    //! Destructor.

    ~djvFileIo();

    //! This enumeration provides the file modes.

    enum MODE
    {
        READ,
        WRITE
    };

    //! Open a file.

    void open(const QString & fileName, MODE) throw (djvError);

    //! Close the file.

    void close();

    //! Get the file name.

    const QString & fileName() const;

    //! Get whether the file is open.

    bool isValid() const;

    //! Get the file size.

    quint64 size() const;

    //! Get data.

    void get(void *, quint64, int wordSize = 1) throw (djvError);

    inline void get8  (qint8 *,   quint64 = 1) throw (djvError);
    inline void getU8 (quint8 *,  quint64 = 1) throw (djvError);
    inline void get16 (qint16 *,  quint64 = 1) throw (djvError);
    inline void getU16(quint16 *, quint64 = 1) throw (djvError);
    inline void get32 (qint32 *,  quint64 = 1) throw (djvError);
    inline void getU32(quint32 *, quint64 = 1) throw (djvError);
    inline void getF32(float *,   quint64 = 1) throw (djvError);

    //! Set data.

    void set(const void *, quint64, int wordSize = 1) throw (djvError);

    inline void set8  (const qint8 *,   quint64) throw (djvError);
    inline void setU8 (const quint8 *,  quint64) throw (djvError);
    inline void set16 (const qint16 *,  quint64) throw (djvError);
    inline void setU16(const quint16 *, quint64) throw (djvError);
    inline void set32 (const qint32 *,  quint64) throw (djvError);
    inline void setU32(const quint32 *, quint64) throw (djvError);
    inline void setF32(const float *,   quint64) throw (djvError);

    inline void set8  (const qint8 &) throw (djvError);
    inline void setU8 (const quint8 &) throw (djvError);
    inline void set16 (const qint16 &) throw (djvError);
    inline void setU16(const quint16 &) throw (djvError);
    inline void set32 (const qint32 &) throw (djvError);
    inline void setU32(const quint32 &) throw (djvError);
    inline void setF32(const float &) throw (djvError);

    //! Start an asynchronous read-ahead. This allows the operating system to
    //! cache the file by the time we need it.

    void readAhead();

    //! Get the current memory-map position.

    const quint8 * mmapP() const;

    //! Get a pointer to the end of the memory-map.

    const quint8 * mmapEnd() const;

    //! Get the file position.

    quint64 pos() const;

    //! Set the file position.

    void setPos(quint64) throw (djvError);

    //! Move the file position.

    void seek(quint64) throw (djvError);

    //! Get whether endian conversion is performed when using the data
    //! functions.

    bool endian() const;

    //! Set whether endian conversion is performed when using the data
    //! functions.

    void setEndian(bool);
    
    //! This enumeration provides error codes.
    
    enum ERROR
    {
        ERROR_OPEN,
#       if defined(DJV_WINDOWS)
        ERROR_FILE_MAPPING,
        ERROR_MEMORY_MAP,
#       else // DJV_WINDOWS
        ERROR_MEMORY_MAP,
#       endif // DJV_WINDOWS
        ERROR_READ,
        ERROR_WRITE,
        ERROR_SET_POS,
        
        ERROR_COUNT
    };
    
    //! Get the error code labels.
    
    static const QStringList & errorLabels();

private:

    void setPos(quint64, bool seek) throw (djvError);
	
    DJV_PRIVATE_COPY(djvFileIo);
    
    djvFileIoPrivate * _p;
};

//@} // djvCoreFile

#include <djvFileIoInline.h>

#endif // DJV_FILE_IO_H

