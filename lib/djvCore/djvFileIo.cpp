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

//! \file djvFileIo.cpp

//#undef DJV_MMAP

#include <djvFileIo.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvFileInfo.h>
#include <djvMath.h>
#include <djvMemory.h>
#include <djvMemoryBuffer.h>

#include <QCoreApplication>
#include <QDir>

#if defined(DJV_WINDOWS)

#define WIN32_LEAN_AND_MEAN

#define NOMINMAX

#include <windows.h>

#endif // DJV_WINDOWS

#if defined(DJV_MMAP)

#if ! defined(DJV_WINDOWS)

#include <sys/mman.h>

#endif // ! DJV_WINDOWS

#endif // DJV_MMAP

#if defined(DJV_WINDOWS)

#include <io.h>

#else // DJV_WINDOWS

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#endif // DJV_WINDOWS

#include <errno.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// djvFileIo::P
//------------------------------------------------------------------------------

struct djvFileIo::P
{
    P() :
#if defined(DJV_WINDOWS)

        f        (INVALID_HANDLE_VALUE),
        mode     (static_cast<MODE>(0)),
        pos      (0),
        size     (0),
        endian   (false),
        mmap     (0),
        mmapStart(0),
        mmapEnd  (0),
        mmapP    (0)
    {}

#else // DJV_WINDOWS

        f        (-1),
        mode     (static_cast<MODE>(0)),
        pos      (0),
        size     (0),
        endian   (false),
        mmap     ((void *) - 1),
        mmapStart(0),
        mmapEnd  (0),
        mmapP    (0)
    {}

#endif // DJV_WINDOWS
#if defined(DJV_WINDOWS)
    HANDLE         f;
#else // DJV_WINDOWS
    int            f;
#endif // DJV_WINDOWS
    QString        fileName;
    MODE           mode;
    quint64        pos;
    quint64        size;
    bool           endian;
    void *         mmap;
    const quint8 * mmapStart;
    const quint8 * mmapEnd;
    const quint8 * mmapP;
    int            mmapReadAhead;
};

//------------------------------------------------------------------------------
// djvFileIo
//------------------------------------------------------------------------------

djvFileIo::djvFileIo() :
    _p(new P)
{}

djvFileIo::~djvFileIo()
{
    close();

    delete _p;
}

void djvFileIo::open(const QString & fileName, MODE mode) throw (djvError)
{
    //DJV_DEBUG("djvFileIo::open");
    //DJV_DEBUG_PRINT("file name = " << fileName);
    //DJV_DEBUG_PRINT("mode = " << mode);

	close();

    // Open the file.

#if defined(DJV_WINDOWS)

    _p->f = ::CreateFile(
        fileName.toLatin1().data(),
        (WRITE == mode) ? GENERIC_WRITE : GENERIC_READ,
        (WRITE == mode) ? 0 : FILE_SHARE_READ,
        0,
        (WRITE == mode) ? CREATE_ALWAYS : OPEN_EXISTING,
        //FILE_ATTRIBUTE_NORMAL,
        //FILE_FLAG_NO_BUFFERING |
        FILE_FLAG_SEQUENTIAL_SCAN,
        0);

    if (INVALID_HANDLE_VALUE == _p->f)
    {
        throw djvError(
            "djvFileIo",
            errorLabels()[ERROR_OPEN].
            arg(QDir::toNativeSeparators(fileName)));
    }

#else // DJV_WINDOWS

    int readFlag = 0;

#if defined(DJV_LINUX)

    //readFlag = O_DIRECT;

#endif // DJV_LINUX

    _p->f = ::open(
        fileName.toLatin1().data(),
        (WRITE == mode) ?
        (O_WRONLY | O_CREAT | O_TRUNC) : (O_RDONLY | readFlag),
        (WRITE == mode) ?
        (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) : (0));

    if (-1 == _p->f)
    {
        throw djvError(
            "djvFileIo",
            errorLabels()[ERROR_OPEN].
            arg(QDir::toNativeSeparators(fileName)));
    }

#endif // DJV_WINDOWS

    _p->fileName = fileName;

    // File position and size.

    _p->pos = 0;

#if defined(DJV_WINDOWS)

    _p->size = ::GetFileSize(_p->f, 0);

#else // DJV_WINDOWS

    _p->size = djvFileInfo(fileName).size();

#endif // DJV_WINDOWS

    //DJV_DEBUG_PRINT("size = " << _p->size);

    // I/O mode.

    _p->mode = mode;

    // Memory mapping.

#if defined(DJV_MMAP)

    if (READ == _p->mode && _p->size > 0)
    {
        //DJV_DEBUG_PRINT("mmap");

#if defined(DJV_WINDOWS)

        _p->mmap = ::CreateFileMapping(_p->f, 0, PAGE_READONLY, 0, 0, 0);

        if (! _p->mmap)
        {
            throw djvError(
                "djvFileIo",
                errorLabels()[ERROR_FILE_MAPPING].
                arg(QDir::toNativeSeparators(_p->fileName)).
                arg(djvErrorUtil::lastError()));
        }
        
        _p->mmapStart = reinterpret_cast<const quint8 *>(
            ::MapViewOfFile(_p->mmap, FILE_MAP_READ, 0, 0, 0));

        if (! _p->mmapStart)
        {
            throw djvError(
                "djvFileIo",
                errorLabels()[ERROR_MEMORY_MAP].
                arg(QDir::toNativeSeparators(_p->fileName)).
                arg(djvErrorUtil::lastError()));
        }

        _p->mmapEnd = _p->mmapStart + _p->size;
        _p->mmapP   = _p->mmapStart;

#else // DJV_WINDOWS

        //DJV_DEBUG_PRINT("mmap 2");

        _p->mmap = ::mmap(0, _p->size, PROT_READ, MAP_SHARED, _p->f, 0);

        if (_p->mmap == (void *) - 1)
        {
            throw djvError(
                "djvFileIo",
                errorLabels()[ERROR_MEMORY_MAP].
                arg(QDir::toNativeSeparators(_p->fileName)));
        }

        _p->mmapStart = reinterpret_cast<const quint8 *>(_p->mmap);
        _p->mmapEnd   = _p->mmapStart + _p->size;
        _p->mmapP     = _p->mmapStart;

#endif // DJV_WINDOWS
    }

#endif // DJV_MMAP
}

void djvFileIo::close()
{
    //DJV_DEBUG("djvFileIo::close");

#if defined(DJV_MMAP)

#if defined(DJV_WINDOWS)

    if (_p->mmapStart != 0)
    {
        if (! ::UnmapViewOfFile((void *)_p->mmapStart))
		{
			//DJV_DEBUG_PRINT("UnmapViewOfFile error");
		}
        
        _p->mmapStart = 0;
    }

    if (_p->mmap != 0)
    {
        if (! ::CloseHandle(_p->mmap))
		{
			//DJV_DEBUG_PRINT("CloseHandle error");		
		}
        
        _p->mmap = 0;
	}

#else // DJV_WINDOWS

    _p->mmapStart = 0;

    if (_p->mmap != (void *) - 1)
    {
        //DJV_DEBUG_PRINT("munmap");

        int r = ::munmap(_p->mmap, _p->size);

        if (-1 == r)
        {
            //const QString err(::strerror(errno));
            
            //DJV_DEBUG_PRINT("errno = " << err);
        }

        _p->mmap = (void *)-1;
    }

#endif // DJV_WINDOWS

    _p->mmapEnd = 0;
    _p->mmapP   = 0;

#endif // DJV_MMAP

    _p->fileName.clear();

#if defined(DJV_WINDOWS)

    if (_p->f != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(_p->f);
        
        _p->f = INVALID_HANDLE_VALUE;
    }

#else // DJV_WINDOWS

    if (_p->f != -1)
    {
        //DJV_DEBUG_PRINT("close");

        int r = ::close(_p->f);

        if (-1 == r)
        {
            const QString err(::strerror(errno));
            
            //DJV_DEBUG_PRINT("errno = " << err);
        }

        _p->f = -1;
    }

#endif // DJV_WINDOWS

    _p->pos  = 0;
    _p->size = 0;
    _p->mode = static_cast<MODE>(0);
}

const QString & djvFileIo::fileName() const
{
    return _p->fileName;
}

bool djvFileIo::isValid() const
{
    return
#if defined(DJV_WINDOWS)
        _p->f != INVALID_HANDLE_VALUE &&
#else
        _p->f != -1 &&
#endif
        (_p->size ? _p->pos < _p->size : true);
}

quint64 djvFileIo::size() const
{
    return _p->size;
}

void djvFileIo::get(void * in, quint64 size, int wordSize) throw (djvError)
{
    //DJV_DEBUG("djvFileIo::get");
    //DJV_DEBUG_PRINT("size = " << size);
    //DJV_DEBUG_PRINT("word size = " << wordSize);

#if defined(DJV_MMAP)

    const quint8 * p = _p->mmapP + size * wordSize;

    if (p > _p->mmapEnd)
    {
        throw djvError(
            "djvFileIo",
            errorLabels()[ERROR_READ].
            arg(QDir::toNativeSeparators(_p->fileName)));
    }

    if (_p->endian && wordSize > 1)
    {
        djvMemory::convertEndian(_p->mmapP, in, size, wordSize);
    }
    else
    {
        djvMemory::copy(_p->mmapP, in, size * wordSize);
    }

    _p->mmapP = p;

#else // DJV_MMAP

#if defined(DJV_WINDOWS)

    DWORD n;

    if (! ::ReadFile(_p->f, in, size * wordSize, &n, 0))
    {
        throw djvError(
            "djvFileIo",
            errorLabels()[ERROR_READ].
            arg(QDir::toNativeSeparators(_p->fileName)));
    }

#else // DJV_WINDOWS

    //if (-1 == ::read(_p->f, in, size * wordSize) == (size * wordSize))
    //{
    //    throw djvError(
    //        "djvFileIo",
    //        errorLabels()[ERROR_READ].
    //        arg(QDir::toNativeSeparators(_p->fileName)));
    //}
    
    ::read(_p->f, in, size * wordSize);

    if (_p->endian && wordSize > 1)
    {
        djvMemory::convertEndian(in, size, wordSize);
    }

#endif // DJV_WINDOWS

#endif // DJV_MMAP

    _p->pos += size * wordSize;
}

void djvFileIo::set(const void * in, quint64 size, int wordSize) throw (djvError)
{
    //DJV_DEBUG("djvFileIo::set");
    //DJV_DEBUG_PRINT("size = " << size);
    //DJV_DEBUG_PRINT("word size = " << wordSize);
    //DJV_DEBUG_PRINT("endian = " << _p->endian);

    quint8 * p = (quint8 *)in;

    djvMemoryBuffer<quint8> tmp;

    if (_p->endian && wordSize > 1)
    {
        tmp.setSize(size * wordSize);
        p = tmp();
        djvMemory::convertEndian(in, p, size, wordSize);
    }

#if defined(DJV_WINDOWS)

    DWORD n;

    if (! ::WriteFile(_p->f, p, static_cast<DWORD>(size * wordSize), &n, 0))
    {
        throw djvError(
            "djvFileIo",
            errorLabels()[ERROR_WRITE].
            arg(QDir::toNativeSeparators(_p->fileName)));
    }

#else

    if (::write(_p->f, p, size * wordSize) == -1)
    {
        throw djvError(
            "djvFileIo",
            errorLabels()[ERROR_WRITE].
            arg(QDir::toNativeSeparators(_p->fileName)));
    }

#endif

    _p->pos += size * wordSize;
    
    _p->size = djvMath::max(_p->pos, _p->size);
}

void djvFileIo::readAhead()
{
#if defined(DJV_MMAP)

#if defined(DJV_LINUX)

    ::madvise((void *)_p->mmapStart, _p->size, MADV_WILLNEED);

#endif // DJV_LINUX

#else // DJV_MMAP

#if defined(DJV_LINUX)

    ::posix_fadvise(_p->f, 0, _p->size, POSIX_FADV_NOREUSE);
    ::posix_fadvise(_p->f, 0, _p->size, POSIX_FADV_WILLNEED);

#endif // DJV_LINUX

#endif // DJV_MMAP
}

const quint8 * djvFileIo::mmapP() const
{
    return _p->mmapP;
}

const quint8 * djvFileIo::mmapEnd() const
{
    return _p->mmapEnd;
}

quint64 djvFileIo::pos() const
{
    return _p->pos;
    
    /*quint64 out = 0;

    switch (_p->mode)
    {
        case READ:
#if defined(DJV_MMAP)
            out = static_cast<quint64>(
                _p->mmapP - reinterpret_cast<const quint8 *>(_p->mmapStart));
            break;
#else
            // Fall through...
#endif

        case WRITE:
#if defined(DJV_WINDOWS)
            out = ::SetFilePointer(_p->f, 0, 0, FILE_CURRENT);
#else
            out = ::lseek(_p->f, 0, SEEK_CUR);
#endif
            break;
    }

    return out;*/
}

void djvFileIo::setPos(quint64 in) throw (djvError)
{
    setPos(in, false);
}

void djvFileIo::seek(quint64 in) throw (djvError)
{
    setPos(in, true);
}

bool djvFileIo::endian() const
{
    return _p->endian;
}

void djvFileIo::setEndian(bool in)
{
    _p->endian = in;
}

void djvFileIo::setPos(quint64 in, bool seek) throw (djvError)
{
    //DJV_DEBUG("djvFileIo::setPos");
    //DJV_DEBUG_PRINT("in = " << static_cast<int>(in));
    //DJV_DEBUG_PRINT("seek = " << seek);

    switch (_p->mode)
    {
        case READ:
        
#if defined(DJV_MMAP)

            if (! seek)
            {
                _p->mmapP = reinterpret_cast<const quint8 *>(_p->mmapStart) + in;
            }
            else
            {
                _p->mmapP += in;
            }

            if (_p->mmapP > _p->mmapEnd)
            {
                throw djvError(
                    "djvFileIo",
                    errorLabels()[ERROR_SET_POS].
                    arg(QDir::toNativeSeparators(_p->fileName)));
            }

            break;

#else
            // Fall through...
#endif

        case WRITE:
        {
        
#if defined(DJV_WINDOWS)

            if (! ::SetFilePointer(
                    _p->f,
                    static_cast<LONG>(in),
                    0,
                    ! seek ? FILE_BEGIN : FILE_CURRENT))
            {
                throw djvError(
                    "djvFileIo",
                    errorLabels()[ERROR_SET_POS].
                    arg(QDir::toNativeSeparators(_p->fileName)));
            }

#else // DJV_WINDOWS

            if (::lseek(_p->f, in, ! seek ? SEEK_SET : SEEK_CUR) == (off_t) - 1)
            {
                throw djvError(
                    "djvFileIo",
                    errorLabels()[ERROR_SET_POS].
                    arg(QDir::toNativeSeparators(_p->fileName)));
            }

#endif // DJV_WINDOWS

        }
        break;
    }
    
    if (! seek)
    {
        _p->pos = in;
    }
    else
    {
        _p->pos += in;
    }
}

const QStringList & djvFileIo::errorLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFileIo", "Cannot open file: \"%1\"") <<
#       if defined(DJV_WINDOWS)
        qApp->translate("djvFileIo", "Cannot create file mapping: \"%1\" (%2)") <<
        qApp->translate("djvFileIo", "Cannot memory-map file: \"%1\" (%2)") <<
#       else // DJV_WINDOWS
        qApp->translate("djvFileIo", "Cannot memory-map file: \"%1\"") <<
#       endif // DJV_WINDOWS
        qApp->translate("djvFileIo", "Error reading file: \"%1\"") <<
        qApp->translate("djvFileIo", "Error writing file: \"%1\"") <<
        qApp->translate("djvFileIo", "Cannot set file position: \"%1\"");
    
    DJV_ASSERT(ERROR_COUNT == data.count());
    
    return data;
}

