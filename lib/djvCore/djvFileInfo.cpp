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

//! \file djvFileInfo.cpp

#include <djvFileInfo.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvFileInfoUtil.h>
#include <djvMath.h>
#include <djvMemory.h>
#include <djvSequenceUtil.h>

#include <QCoreApplication>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// djvFileInfo
//------------------------------------------------------------------------------

void djvFileInfo::init()
{
    _exists      = false;
    _dotFile     = false;
    _type        = FILE;
    _size        = 0;
    _user        = 0;
    _permissions = 0;
    _time        = 0;
}

djvFileInfo::djvFileInfo()
{
    init();
}

djvFileInfo::djvFileInfo(const QString & fileName, bool stat)
{
    init();
    
    setFileName(fileName, stat);
}

QString djvFileInfo::fileName(qint64 frame, bool path) const
{
    QString out;

    // Reconstruct pieces.

    if (path)
    {
        out += _path;
    }

    out += _base;

    if (SEQUENCE == _type && frame != -1)
    {
        out += djvSequenceUtil::frameToString(frame, _sequence.pad);
    }
    else if (SEQUENCE == _type && _sequence.frames.count())
    {
        out += djvSequenceUtil::sequenceToString(_sequence);
    }
    else
    {
        out += _number;
    }

    out += _extension;

    return out;
}

void djvFileInfo::setFileName(const QString & in, bool stat)
{
    //DJV_DEBUG("djvFileInfo::setFileName");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("stat = " << stat);

    init();

    // Split file name into pieces.

    djvFileInfoUtil::split(
        in,
        _path,
        _base,
        _number,
        _extension);

    //DJV_DEBUG_PRINT("path = " << _path);
    //DJV_DEBUG_PRINT("base = " << _base);
    //DJV_DEBUG_PRINT("number = " << _number);
    //DJV_DEBUG_PRINT("extension = " << _extension);

    // Information.

    if (stat)
    {
        this->stat();
    }
    
    if (djvFileInfoUtil::dot == _base)
        ;
    else if (djvFileInfoUtil::dotDot == _base)
        ;
    else if (_base.length() > 0)
    {
        _dotFile = _base[0] == '.';
    }

    // Sequence.

    _sequence = djvSequenceUtil::stringToSequence(_number);

    //DJV_DEBUG_PRINT("sequence = " << _sequence);
}

void djvFileInfo::setPath(const QString & in)
{
    _path = in;
}

void djvFileInfo::setBase(const QString & in)
{
    _base = in;
}

void djvFileInfo::setNumber(const QString & in)
{
    _number = in;
}

void djvFileInfo::setExtension(const QString & in)
{
    _extension = in;
}

const QStringList & djvFileInfo::typeLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFileInfo", "File") <<
        qApp->translate("djvFileInfo", "Seq") <<
        qApp->translate("djvFileInfo", "Dir");

    DJV_ASSERT(data.count() == djvFileInfo::TYPE_COUNT);

    return data;
}

void djvFileInfo::setType(TYPE in)
{
    _type = in;
}

void djvFileInfo::setSize(quint64 in)
{
    _size = in;
}

void djvFileInfo::setUser(uid_t in)
{
    _user = in;
}

const QStringList & djvFileInfo::permissionsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFileInfo", "r") <<
        qApp->translate("djvFileInfo", "w") <<
        qApp->translate("djvFileInfo", "x");

    DJV_ASSERT(data.count() == djvFileInfo::PERMISSIONS_COUNT);

    return data;
}

const QString & djvFileInfo::permissionsLabel(int in)
{
    static const QString data[] =
    {
        qApp->translate("djvFileInfo", ""),
        qApp->translate("djvFileInfo", "r"),
        qApp->translate("djvFileInfo", "w"),
        qApp->translate("djvFileInfo", "rw"),
        qApp->translate("djvFileInfo", "x"),
        qApp->translate("djvFileInfo", "rx"),
        qApp->translate("djvFileInfo", "wx"),
        qApp->translate("djvFileInfo", "rwx")
    };

    return data[in];
}

void djvFileInfo::setPermissions(int in)
{
    _permissions = in;
}

void djvFileInfo::setTime(time_t in)
{
    _time = in;
}

#if defined(DJV_WINDOWS)

#define _STAT struct ::_stati64
#define _STAT_FNC    ::_stati64

#elif (defined(DJV_FREEBSD) || defined(DJV_OSX))

//! \todo OS X doesn't have stat64?

#define _STAT struct ::stat
#define _STAT_FNC    ::stat

#else

#define _STAT struct ::stat64
#define _STAT_FNC    ::stat64

#endif // DJV_WINDOWS

bool djvFileInfo::stat(const QString & path)
{
    //DJV_DEBUG("djvFileInfo::stat");
    //DJV_DEBUG_PRINT("path = " << path);
    
    _exists      = false;
    _type        = static_cast<TYPE>(0);
    _size        = 0;
    _user        = 0;
    _permissions = 0;
    _time        = time_t();

    _STAT info;
    
    djvMemory::fill<quint8>(0, &info, sizeof(_STAT));

    const QString fileName =
        djvFileInfoUtil::fixPath(path.length() ? path : _path) +
        this->fileName(-1, false);
    
    //DJV_DEBUG_PRINT("fileName = " << fileName);

    //! \todo Try to stat multiple variations of the path name to find the
    //! correct one. Is this still necessary?

    const QString fileName2 = fileName + '/';
    const QString fileName3 = fileName.mid(0, fileName.length() - 1);

    //DJV_DEBUG_PRINT("fileName2 = " << fileName2);
    //DJV_DEBUG_PRINT("fileName3 = " << fileName3);

    if (0 == _STAT_FNC(fileName.toLatin1().data(), &info))
    {
        //DJV_DEBUG_PRINT("fileName");
    }
    else if (
        0 == _STAT_FNC(fileName2.toLatin1().data(), &info))
    {
        //DJV_DEBUG_PRINT("fileName2");
    }
    else if (
        0 == _STAT_FNC(fileName3.toLatin1().data(), &info))
    {
        //DJV_DEBUG_PRINT("fileName3");
    }
    else
    {
        QString err;

#if defined(DJV_WINDOWS)

        char tmp[djvStringUtil::cStringLength] = "";
        ::strerror_s(tmp, djvStringUtil::cStringLength, errno);
        err = tmp;

#endif // DJV_WINDOWS

        //DJV_DEBUG_PRINT("errno = " << err);

        return false;
    }

    _exists      = true;
    _size        = info.st_size;
    _user        = info.st_uid;
    _permissions = 0;
    _time        = info.st_mtime;
    _type        = FILE;
    _permissions = 0;
    
    //DJV_DEBUG_PRINT("size = " << _size);

#if defined(DJV_WINDOWS)

    if (info.st_mode & _S_IFDIR)
    {
        _type = DIRECTORY;
    }

    _permissions |= (info.st_mode & _S_IREAD ) ? djvFileInfo::READ : 0;
    _permissions |= (info.st_mode & _S_IWRITE) ? djvFileInfo::WRITE : 0;
    _permissions |= (info.st_mode & _S_IEXEC ) ? djvFileInfo::EXEC : 0;

#else // DJV_WINDOWS

    if (S_ISDIR(info.st_mode))
    {
        _type = DIRECTORY;
    }

    _permissions |= (info.st_mode & S_IRUSR) ? READ  : 0;
    _permissions |= (info.st_mode & S_IWUSR) ? WRITE : 0;
    _permissions |= (info.st_mode & S_IXUSR) ? EXEC  : 0;

#endif // DJV_WINDOWS
    
    //DJV_DEBUG_PRINT("type = " << _type);

    return true;
}

void djvFileInfo::setSequence(const djvSequence & in)
{
    _sequence = in;
    
    _number = djvSequenceUtil::sequenceToString(_sequence);
}


void djvFileInfo::sortSequence()
{
    _sequence.sort();
    
    _number = djvSequenceUtil::sequenceToString(_sequence);
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvFileInfo::TYPE, djvFileInfo::typeLabels())

QStringList & operator >> (QStringList & in, djvFileInfo & out) throw (QString)
{
    QString           fileName;
    djvFileInfo::TYPE type     = static_cast<djvFileInfo::TYPE>(0);
    
    in >> fileName;
    in >> type;
    
    out = fileName;
    out.setType(type);
    
    return in;
}

QStringList & operator << (QStringList & out, const djvFileInfo & in)
{
    out << in.fileName();
    out << in.type();
    
    return out;
}

djvDebug & operator << (djvDebug & debug, const djvFileInfo::TYPE & in)
{
    QStringList tmp;
    tmp << in;
    
    return debug << tmp;
}

djvDebug & operator << (djvDebug & debug, const djvFileInfo & in)
{
    return debug << in.fileName();
}

