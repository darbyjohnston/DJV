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

#include <djvCore/FileInfo.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>
#include <djvCore/SequenceUtil.h>

#include <QCoreApplication>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>

namespace djv
{
    namespace Core
    {
        QSet<QString> FileInfo::sequenceExtensions;

        FileInfo::FileInfo()
        {}

        FileInfo::FileInfo(const QString & fileName, bool stat)
        {
            setFileName(fileName, stat);
        }

        QString FileInfo::fileName(qint64 frame, bool path) const
        {
            QString out;
            if (path)
            {
                out += _path;
            }
            out += _base;
            if (SEQUENCE == _type && frame != -1)
            {
                out += SequenceUtil::frameToString(frame, _sequence.pad);
            }
            else if (SEQUENCE == _type && _sequence.frames.count())
            {
                out += SequenceUtil::sequenceToString(_sequence);
            }
            else
            {
                out += _number;
            }
            out += _extension;
            return out;
        }

        void FileInfo::setFileName(const QString & in, bool stat)
        {
            //DJV_DEBUG("FileInfo::setFileName");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("stat = " << stat);

            _exists = false;
            _dotFile = false;
            _type = FILE;
            _size = 0;
            _user = 0;
            _permissions = 0;
            _time = 0;

            // Split file name into pieces.
            FileInfoUtil::split(
                in,
                _path,
                _base,
                _number,
                _extension);

            //DJV_DEBUG_PRINT("path = " << _path);
            //DJV_DEBUG_PRINT("base = " << _base);
            //DJV_DEBUG_PRINT("number = " << _number);
            //DJV_DEBUG_PRINT("extension = " << _extension);

            // Get information from the file system.
            if (stat)
            {
                this->stat();
            }

            // Does this file start with a '.'?
            if (FileInfoUtil::dot == _base)
                ;
            else if (FileInfoUtil::dotDot == _base)
                ;
            else if (_base.length() > 0)
            {
                _dotFile = _base[0] == '.';
            }

            // File sequencing.
            if (sequenceExtensions.contains(_extension))
            {
                _sequence = SequenceUtil::stringToSequence(_number);
            }

            //DJV_DEBUG_PRINT("sequence = " << _sequence);
        }

        void FileInfo::setPath(const QString & in)
        {
            _path = in;
        }

        void FileInfo::setBase(const QString & in)
        {
            _base = in;
        }

        void FileInfo::setNumber(const QString & in)
        {
            _number = in;
        }

        void FileInfo::setExtension(const QString & in)
        {
            _extension = in;
        }

        const QStringList & FileInfo::typeLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Core::FileInfo", "File") <<
                qApp->translate("djv::Core::FileInfo", "Seq") <<
                qApp->translate("djv::Core::FileInfo", "Dir");
            DJV_ASSERT(data.count() == FileInfo::TYPE_COUNT);
            return data;
        }

        const QStringList & FileInfo::typeIcons()
        {
            static const QStringList data = QStringList() <<
                ":djv::Core::FileIcon.png" <<
                ":djv::Core::SequenceIcon.png" <<
                ":djv::Core::DirIcon.png";
            DJV_ASSERT(data.count() == FileInfo::TYPE_COUNT);
            return data;
        }

        const QStringList & FileInfo::typeImages()
        {
            static const QStringList data = QStringList() <<
                ":djv::Core::File.svg" <<
                ":djv::Core::Sequence.svg" <<
                ":djv::Core::Dir.svg";
            DJV_ASSERT(data.count() == FileInfo::TYPE_COUNT);
            return data;
        }

        void FileInfo::setType(TYPE in)
        {
            _type = in;
        }

        void FileInfo::setSize(quint64 in)
        {
            _size = in;
        }

        void FileInfo::setUser(uid_t in)
        {
            _user = in;
        }

        const QStringList & FileInfo::permissionsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Core::FileInfo", "r") <<
                qApp->translate("djv::Core::FileInfo", "w") <<
                qApp->translate("djv::Core::FileInfo", "x");
            DJV_ASSERT(data.count() == FileInfo::PERMISSIONS_COUNT);
            return data;
        }

        const QString & FileInfo::permissionsLabel(int in)
        {
            static const QString data[] =
            {
                qApp->translate("djv::Core::FileInfo", ""),
                qApp->translate("djv::Core::FileInfo", "r"),
                qApp->translate("djv::Core::FileInfo", "w"),
                qApp->translate("djv::Core::FileInfo", "rw"),
                qApp->translate("djv::Core::FileInfo", "x"),
                qApp->translate("djv::Core::FileInfo", "rx"),
                qApp->translate("djv::Core::FileInfo", "wx"),
                qApp->translate("djv::Core::FileInfo", "rwx")
            };
            return data[in];
        }

        void FileInfo::setPermissions(int in)
        {
            _permissions = in;
        }

        void FileInfo::setTime(time_t in)
        {
            _time = in;
        }

#if defined(DJV_WINDOWS)
#define _STAT struct ::_stati64
#define _STAT_FNC    ::_wstati64
#elif (defined(DJV_FREEBSD) || defined(DJV_OSX))
        //! \todo OS X doesn't have stat64?
#define _STAT struct ::stat
#define _STAT_FNC    ::stat
#else
#define _STAT struct ::stat64
#define _STAT_FNC    ::stat64
#endif // DJV_WINDOWS

        bool FileInfo::stat(const QString & path)
        {
            //DJV_DEBUG("FileInfo::stat");
            //DJV_DEBUG_PRINT("path = " << path);

            _exists = false;
            _type = static_cast<TYPE>(0);
            _size = 0;
            _user = 0;
            _permissions = 0;
            _time = time_t();

            const QString fileName =
                FileInfoUtil::fixPath(path.length() ? path : _path) +
                this->fileName(-1, false);
            //DJV_DEBUG_PRINT("fileName = " << fileName);
            _STAT info;
            Memory::fill<quint8>(0, &info, sizeof(_STAT));
            if (_STAT_FNC(StringUtil::qToStdWString(fileName).data(), &info) != 0)
            {
                QString err;
#if defined(DJV_WINDOWS)
                char tmp[StringUtil::cStringLength] = "";
                ::strerror_s(tmp, StringUtil::cStringLength, errno);
                err = tmp;
#endif // DJV_WINDOWS
                //DJV_DEBUG_PRINT("errno = " << err);
                return false;
            }

            _exists = true;
            _size = info.st_size;
            _user = info.st_uid;
            _permissions = 0;
            _time = info.st_mtime;
            _type = FILE;
            _permissions = 0;

            //DJV_DEBUG_PRINT("size = " << _size);

#if defined(DJV_WINDOWS)
            if (info.st_mode & _S_IFDIR)
            {
                _type = DIRECTORY;
            }
            _permissions |= (info.st_mode & _S_IREAD) ? FileInfo::READ : 0;
            _permissions |= (info.st_mode & _S_IWRITE) ? FileInfo::WRITE : 0;
            _permissions |= (info.st_mode & _S_IEXEC) ? FileInfo::EXEC : 0;
#else // DJV_WINDOWS
            if (S_ISDIR(info.st_mode))
            {
                _type = DIRECTORY;
            }
            _permissions |= (info.st_mode & S_IRUSR) ? READ : 0;
            _permissions |= (info.st_mode & S_IWUSR) ? WRITE : 0;
            _permissions |= (info.st_mode & S_IXUSR) ? EXEC : 0;
#endif // DJV_WINDOWS
            //DJV_DEBUG_PRINT("type = " << _type);

            return true;
        }

        void FileInfo::setSequence(const Sequence & in)
        {
            _sequence = in;
            _number = SequenceUtil::sequenceToString(_sequence);
        }


        void FileInfo::sortSequence()
        {
            _sequence.sort();
            _number = SequenceUtil::sequenceToString(_sequence);
        }

    } // namespace Core

    _DJV_STRING_OPERATOR_LABEL(Core::FileInfo::TYPE, Core::FileInfo::typeLabels());

    Core::Debug & operator << (Core::Debug & debug, const Core::FileInfo::TYPE & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    Core::Debug & operator << (Core::Debug & debug, const Core::FileInfo & in)
    {
        return debug << in.fileName();
    }

    QStringList & operator >> (QStringList & in, Core::FileInfo & out)
    {
        QString fileName;
        Core::FileInfo::TYPE type = static_cast<Core::FileInfo::TYPE>(0);
        in >> fileName;
        in >> type;
        out = fileName;
        out.setType(type);
        return in;
    }

    QStringList & operator << (QStringList & out, const Core::FileInfo & in)
    {
        out << in.fileName();
        out << in.type();
        return out;
    }

    QStringList & operator >> (QStringList & in, Core::FileInfoList & out)
    {
        while (in.count())
        {
            Core::FileInfo value;
            in >> value;
            out += value;
        }
        return in;
    }

    QStringList & operator << (QStringList & out, const Core::FileInfoList & in)
    {
        for (int i = 0; i < in.count(); ++i)
        {
            out << in[i];
        }
        return out;
    }

} // namespace djv
