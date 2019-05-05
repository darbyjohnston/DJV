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

#include <djvCore/FileInfoUtil.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Error.h>
#include <djvCore/FileIO.h>
#include <djvCore/ListUtil.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>
#include <djvCore/Sequence.h>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QRegExp>

#include <algorithm>

#if defined(DJV_WINDOWS)
#include <windows.h>
/*#elif defined(DJV_LINUX)
#include <sys/syscall.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
*/
#else // DJV_WINDOWS
#include <dirent.h>
#endif // DJV_WINDOWS

namespace djv
{
    namespace Core
    {
        FileInfoUtil::~FileInfoUtil()
        {}

        namespace
        {
            bool isPathSeparator(const QChar & c)
            {
                return c == '/' || c == '\\';
            }

            bool isSequenceChar(const QChar & c)
            {
                bool out = QChar(c).isDigit() || '#' == c;
                if (Sequence::isNegativeEnabled())
                {
                    out |= '-' == c;
                }
                return out;
            }

            bool isSequenceSeparatorChar(const QChar & c)
            {
                return '-' == c || ',' == c;
            }

            bool matchPadding(const QString & a, const QString & b)
            {
                //DJV_DEBUG("matchPadding");
                //DJV_DEBUG_PRINT("a = " << a);
                //DJV_DEBUG_PRINT("b = " << b);
                return
                    ((a.length() > 1 && '0' == a[0]) || (b.length() > 1 && '0' == b[0])) ?
                    (a.length() == b.length()) :
                    true;
            }

        } // namespace

        void FileInfoUtil::split(
            const QString & in,
            QString &       path,
            QString &       base,
            QString &       number,
            QString &       extension)
        {
            //DJV_DEBUG("FileInfoUtil::split");
            //DJV_DEBUG_PRINT("in = " << in);

            path.clear();
            base.clear();
            number.clear();
            extension.clear();
            
            const int l = in.length();
            int i = l - 1;
            for (; i > 0 && in[i] != '.' && !isPathSeparator(in[i]); --i)
                ;
            if (i > 0 && !isPathSeparator(in[i]) && !isPathSeparator(in[i - 1]))
            {
                extension = in.mid(i);
                //DJV_DEBUG_PRINT("extension = " << extension);
            }
            else
            {
                i = l;
            }
            
            int j = i;
            for (; i > 0 && (isSequenceChar(in[i - 1]) || isSequenceSeparatorChar(in[i - 1])); --i)
                ;
            if (i != j)
            {
                for (; i < j && ! isSequenceChar(in[i]); ++i)
                    ;
                number = in.mid(i, j - i);
                //DJV_DEBUG_PRINT("number = " << number);
            }
            
            j = i;
            for (; i > 0 && ! isPathSeparator(in[i - 1]); --i)
                ;
            if (i != j)
            {
                base = in.mid(i, j - i);
                //DJV_DEBUG_PRINT("base = " << base);
            }
            if (i > 0)
            {
                path = in.mid(0, i);
                //DJV_DEBUG_PRINT("path = " << path);
            }
        }

        bool FileInfoUtil::exists(const FileInfo & in)
        {
            //DJV_DEBUG("FileInfoUtil::exists");
            //DJV_DEBUG_PRINT("in = " << in);

            QStringList list;
            if (FileInfo::SEQUENCE == in.type())
            {
                list = expandSequence(in);
            }
            else
            {
                list += in;
            }
            //DJV_DEBUG_PRINT("list = " << list);

            for (int i = 0; i < list.count(); ++i)
            {
                try
                {
                    FileIO().open(list[i], FileIO::READ);
                    return true;
                }
                catch (const Error &)
                {
                    //DJV_DEBUG_PRINT("error = " << error.string());
                }
            }

            
            return false;
        }

        namespace
        {
            bool isDotDir(const QString & value)
            {
                const int l = value.length();
                if (1 == l && '.' == value[0])
                    return true;
                else if (2 == l && '.' == value[0] && '.' == value[1])
                    return true;
                return false;
            }

        } // namespace

        FileInfoList FileInfoUtil::list(
            const QString &  path,
            Sequence::FORMAT format)
        {
            //DJV_DEBUG("FileInfoUtil::list");
            //DJV_DEBUG_PRINT("path = " << path);
            //DJV_DEBUG_PRINT("format = " << format);

            FileInfoList out;
            FileInfo * cache = 0;
            QString fixedPath = fixPath(path);

#if defined(DJV_WINDOWS)
            WIN32_FIND_DATAW data;
            HANDLE h = FindFirstFileExW(
                StringUtil::qToStdWString(QString(fixedPath + "*")).data(),
                FindExInfoBasic,
                &data,
                FindExSearchNameMatch,
                NULL,
                FIND_FIRST_EX_LARGE_FETCH);
            if (h != INVALID_HANDLE_VALUE)
            {
                QString fileName = QString::fromWCharArray(data.cFileName);
                if (!isDotDir(fileName))
                {
                    out.append(FileInfo(fixedPath + fileName));
                }
                while (FindNextFileW(h, &data))
                {
                    fileName = QString::fromWCharArray(data.cFileName);
                    if (!isDotDir(fileName))
                    {
                        if (!out.count())
                        {
                            out.append(FileInfo(fixedPath + fileName));
                        }
                        else
                        {
                            const FileInfo tmp(fixedPath + fileName);
                            int i = 0;
                            if (format && cache)
                            {
                                if (!cache->addSequence(tmp))
                                {
                                    cache = 0;
                                }
                            }
                            if (format && !cache)
                            {
                                for (; i < out.count(); ++i)
                                {
                                    if (out[i].addSequence(tmp))
                                    {
                                        cache = &out[i];
                                        break;
                                    }
                                }
                            }
                            if (!format || i == out.count())
                            {
                                out.append(tmp);
                            }
                        }
                    }
                }
                FindClose(h);
            }
#else // DJV_WINDOWS
            DIR * dir = ::opendir(path.toUtf8().data());
            if (dir)
            {
                struct dirent * de = 0;
                while ((de = ::readdir(dir)) != 0)
                {
                    QString fileName = QString::fromUtf8(de->d_name);
                    if (!isDotDir(fileName))
                    {
                        if (!out.count())
                        {
                            out.append(FileInfo(fixedPath + fileName));
                        }
                        else
                        {
                            const FileInfo tmp(fixedPath + fileName);
                            int i = 0;
                            if (format && cache)
                            {
                                if (!cache->addSequence(tmp))
                                {
                                    cache = 0;
                                }
                            }
                            if (format && !cache)
                            {
                                for (; i < out.count(); ++i)
                                {
                                    if (out[i].addSequence(tmp))
                                    {
                                        cache = &out[i];
                                        break;
                                    }
                                }
                            }
                            if (!format || i == out.count())
                            {
                                out.append(tmp);
                            }
                        }
                    }
                }
                closedir(dir);
            }
#endif // DJV_WINDOWS
            for (int i = 0; i < out.count(); ++i)
            {
                out[i]._sequence.sort();
            }
            if (Sequence::FORMAT_RANGE == format)
            {
                for (int i = 0; i < out.count(); ++i)
                {
                    if (out[i]._sequence.frames.count())
                    {
                        out[i]._sequence.setFrames(
                            out[i]._sequence.start(),
                            out[i]._sequence.end());
                    }
                }
            }
            for (int i = 0; i < out.count(); ++i)
            {
                if (FileInfo::SEQUENCE == out[i].type())
                {
                    out[i]._number = Sequence::sequenceToString(out[i]._sequence);
                }
            }
            return out;
        }

        const FileInfo & FileInfoUtil::sequenceWildcardMatch(
            const FileInfo &     in,
            const FileInfoList & list)
        {
            for (int i = 0; i < list.count(); ++i)
            {
                const FileInfo & tmp = list[i];
                if (
                    in.base() == tmp.base() &&
                    in.extension() == tmp.extension())
                {
                    return tmp;
                }
            }
            return in;
        }

        void FileInfoUtil::sequence(
            FileInfoList &   items,
            Sequence::FORMAT format)
        {
            if (Sequence::FORMAT_OFF == format)
                return;
            
            //DJV_DEBUG("FileInfoUtil::sequence");
            //DJV_DEBUG_PRINT("count = " << items.count());

            int  cache = 0;
            bool cacheValid = false;
            int  i = 0;
            for (int j = 0; j < items.count(); ++j)
            {
                //DJV_DEBUG_PRINT("item = " << items[j]);
                //DJV_DEBUG_PRINT("item seq = " << items[j].sequence());

                const bool sequence = items[j].isSequenceValid();
                //DJV_DEBUG_PRINT("sequence = " << sequence);
                if (sequence)
                {
                    if (cacheValid)
                    {
                        if (items[cache].addSequence(items[j]))
                        {
                            continue;
                        }
                    }
                    cacheValid = false;
                    for (int k = 0; k < i; ++k)
                    {
                        if (FileInfo::SEQUENCE == items[k].type())
                        {
                            if (items[k].addSequence(items[j]))
                            {
                                cache = k;
                                cacheValid = true;
                                continue;
                            }
                        }
                    }
                }
                if (!sequence || !cacheValid)
                {
                    items[i] = items[j];
                    if (sequence)
                    {
                        items[i].setType(FileInfo::SEQUENCE);
                    }
                    ++i;
                }
            }
            //DJV_DEBUG_PRINT("count = " << i);

            items.resize(i);
            for (int i = 0; i < items.count(); ++i)
            {
                items[i]._sequence.sort();
            }

            if (Sequence::FORMAT_RANGE == format)
            {
                for (int i = 0; i < items.count(); ++i)
                {
                    if (items[i]._sequence.frames.count())
                    {
                        items[i]._sequence.setFrames(
                            items[i]._sequence.start(),
                            items[i]._sequence.end());
                    }
                }
            }

            for (int i = 0; i < items.count(); ++i)
            {
                if (FileInfo::SEQUENCE == items[i].type())
                {
                    items[i]._number = Sequence::sequenceToString(items[i]._sequence);
                }
            }
        }

        QStringList FileInfoUtil::expandSequence(const FileInfo & in)
        {
            //DJV_DEBUG("FileInfoUtil::expandSequence");
            //DJV_DEBUG_PRINT("in = " << in);
            QStringList out;
            const Sequence & sequence = in.sequence();
            //DJV_DEBUG_PRINT("sequence = " << sequence);
            const int count = sequence.frames.count();
            //DJV_DEBUG_PRINT("count = " << count);
            if (FileInfo::SEQUENCE == in.type() && count)
            {
                for (int i = 0; i < count; ++i)
                {
                    out += in.fileName(sequence.frames[i]);
                }
            }
            else
            {
                out += in;
            }
            return out;
        }

        void FileInfoUtil::filter(
            FileInfoList &      items,
            int                 filter,
            const QString &     filterText,
            const QStringList & glob)
        {
            //DJV_DEBUG("FileInfoUtil::filter");
            //DJV_DEBUG_PRINT("filter = " << filter);
            //DJV_DEBUG_PRINT("filterText = " << filterText);
            //DJV_DEBUG_PRINT("glob = " << glob);

            int i = 0;
            for (int j = 0; j < items.count(); ++j)
            {
                const QString name = items[j].fileName(-1, false);
                //DJV_DEBUG_PRINT("name = " << name);

                bool valid = true;
                const FileInfo::TYPE type = items[j].type();
                if ((filter & FILTER_FILES) &&
                    (FileInfo::FILE == type || FileInfo::SEQUENCE == type))
                {
                    valid = false;
                }
                if ((filter & FILTER_DIRECTORIES) && FileInfo::DIRECTORY == type)
                {
                    valid = false;
                }
                if ((filter & FILTER_HIDDEN) && items[j].isDotFile())
                {
                    valid = false;
                }
                if (filterText.length() > 0)
                {
                    if (!name.contains(filterText, Qt::CaseInsensitive))
                        valid = false;
                }
                if (glob.count())
                {
                    int k = 0;
                    for (; k < glob.count(); ++k)
                    {
                        QRegExp re(glob[k]);
                        re.setPatternSyntax(QRegExp::Wildcard);
                        if (re.exactMatch(name))
                            break;
                    }
                    if (glob.count() == k)
                    {
                        valid = false;
                    }
                }
                //DJV_DEBUG_PRINT("valid = " << valid);
                if (valid)
                {
                    if (i != j)
                    {
                        items[i] = items[j];
                    }
                    ++i;
                }
            }
            //DJV_DEBUG_PRINT("count = " << i);
            items.resize(i);
        }

        const QStringList & FileInfoUtil::sortLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Core::FileInfoUtil", "Name") <<
                qApp->translate("djv::Core::FileInfoUtil", "Type") <<
                qApp->translate("djv::Core::FileInfoUtil", "Size") <<
                qApp->translate("djv::Core::FileInfoUtil", "User") <<
                qApp->translate("djv::Core::FileInfoUtil", "Permissions") <<
                qApp->translate("djv::Core::FileInfoUtil", "Time");
            DJV_ASSERT(data.count() == SORT_COUNT);
            return data;
        }

        namespace
        {
            bool compareName(const FileInfo & a, const FileInfo & b)
            {
                return a.fileName() < b.fileName();
            }

            bool compareType(const FileInfo & a, const FileInfo & b)
            {
                return a.type() < b.type();
            }

            bool compareSize(const FileInfo & a, const FileInfo & b)
            {
                return a.size() < b.size();
            }

            bool compareUser(const FileInfo & a, const FileInfo & b)
            {
                return a.user() < b.user();
            }

            bool comparePermissions(const FileInfo & a, const FileInfo & b)
            {
                return a.permissions() < b.permissions();
            }

            bool compareTime(const FileInfo & a, const FileInfo & b)
            {
                return a.time() < b.time();
            }

            bool compareNameReverse(const FileInfo & a, const FileInfo & b)
            {
                return compareName(b, a);
            }

            bool compareTypeReverse(const FileInfo & a, const FileInfo & b)
            {
                return compareType(b, a);
            }

            bool compareSizeReverse(const FileInfo & a, const FileInfo & b)
            {
                return compareSize(b, a);
            }

            bool compareUserReverse(const FileInfo & a, const FileInfo & b)
            {
                return compareUser(b, a);
            }

            bool comparePermissionsReverse(const FileInfo & a, const FileInfo & b)
            {
                return comparePermissions(b, a);
            }

            bool compareTimeReverse(const FileInfo & a, const FileInfo & b)
            {
                return compareTime(b, a);
            }

        } // namespace

        void FileInfoUtil::sort(FileInfoList & items, SORT sort, bool reverse)
        {
            //DJV_DEBUG("FileInfoUtil::sort");
            //DJV_DEBUG_PRINT("items = " << items.count());
            //DJV_DEBUG_PRINT("sort = " << sort);
            //DJV_DEBUG_PRINT("reverse = " << reverse);

            typedef bool (Compare)(const FileInfo &, const FileInfo &);
            Compare * compare = 0;
            switch (sort)
            {
            case SORT_NAME:
                compare = reverse ? compareNameReverse : compareName;
                break;
            case SORT_TYPE:
                compare = reverse ? compareTypeReverse : compareType;
                break;
            case SORT_SIZE:
                compare = reverse ? compareSizeReverse : compareSize;
                break;
            case SORT_USER:
                compare = reverse ? compareUserReverse : compareUser;
                break;
            case SORT_PERMISSIONS:
                compare = reverse ? comparePermissionsReverse : comparePermissions;
                break;
            case SORT_TIME:
                compare = reverse ? compareTimeReverse : compareTime;
                break;
            default: break;
            }
            qSort(items.begin(), items.end(), compare);
        }

        void FileInfoUtil::sortDirsFirst(FileInfoList & in)
        {
            FileInfoList dirs, files;
            const int count = in.count();
            for (int i = 0; i < count; ++i)
            {
                if (in[i].type() == FileInfo::DIRECTORY)
                {
                    dirs += in[i];
                }
                else
                {
                    files += in[i];
                }
            }
            in.clear();
            in += dirs;
            in += files;
        }

        void FileInfoUtil::recent(
            const FileInfo & fileInfo,
            FileInfoList &   list,
            int              max)
        {
            const int index = list.indexOf(fileInfo);
            if (-1 == index)
            {
                // Insert new item at front of list.
                list.push_front(fileInfo);
                while (list.count() > max)
                {
                    list.pop_back();
                }
            }
            else
            {
                // Move existing item to front of list.
                list.remove(list.indexOf(fileInfo));
                list.push_front(fileInfo);
            }
        }

        void FileInfoUtil::recent(
            const QString & fileName,
            QStringList &   list,
            int             max)
        {
            const int index = list.indexOf(fileName);
            if (-1 == index)
            {
                // Insert new item at front of list.
                list.push_front(fileName);
                while (list.count() > max)
                {
                    list.pop_back();
                }
            }
            else
            {
                // Move existing item to front of list.
                list.removeAt(list.indexOf(fileName));
                list.push_front(fileName);
            }
        }

        QString FileInfoUtil::fixPath(const QString & in)
        {
            //DJV_DEBUG("FileInfoUtil::fixPath");
            //DJV_DEBUG_PRINT("in = " << in);

            QString out;
            const QFileInfo qFileInfo(in);
            if (qFileInfo.exists())
            {
                out = qFileInfo.absoluteFilePath();
                //DJV_DEBUG_PRINT("out = " << out);
                if (qFileInfo.isDir() &&
                    out.length() > 0 &&
                    out[out.length() - 1] != '/')
                {
                    //DJV_DEBUG_PRINT("add separator");
                    out += '/';
                }
            }
            else
            {
                if (qFileInfo.isAbsolute())
                {
                    out = qFileInfo.filePath();
                }
                else
                {
                    out = QDir::currentPath() + '/' + qFileInfo.filePath();
                }
            }
            //DJV_DEBUG_PRINT("out = " << out);
            return out;
        }

        const QList<QChar> FileInfoUtil::listSeparators = QList<QChar>() << ':' << ';';

        const QString FileInfoUtil::dot(".");
        const QString FileInfoUtil::dotDot("..");

        FileInfo FileInfoUtil::parse(
            const QString &  fileName,
            Sequence::FORMAT format,
            bool             autoSequence)
        {
            //DJV_DEBUG("FileInfoUtil::parse");
            //DJV_DEBUG_PRINT("fileName = " << fileName);
            //DJV_DEBUG_PRINT("format = " << format);
            //DJV_DEBUG_PRINT("autoSequence = " << autoSequence);

            FileInfo fileInfo(fileName, false);

            // Match wildcards.
            if (format && fileInfo.isSequenceWildcard())
            {
                fileInfo = FileInfoUtil::sequenceWildcardMatch(
                    fileInfo,
                    FileInfoUtil::list(fileInfo.path(), format));
                //DJV_DEBUG_PRINT("  wildcard match = " << fileInfo);
            }

            // Is this is a sequence?
            if (format && fileInfo.isSequenceValid())
            {
                //DJV_DEBUG_PRINT("sequence");
                fileInfo.setType(FileInfo::SEQUENCE);
            }

            // Find other files in the directory that match this sequence.
            if (format && autoSequence)
            {
                //DJV_DEBUG_PRINT("auto sequence");
                const FileInfoList items = FileInfoUtil::list(fileInfo.path(), format);
                for (int i = 0; i < items.count(); ++i)
                {
                    if (items[i].isSequenceValid() &&
                        items[i].extension() == fileInfo.extension() &&
                        items[i].base() == fileInfo.base())
                    {
                        fileInfo.setType(FileInfo::SEQUENCE);
                        fileInfo.setSequence(items[i].sequence());
                        break;
                    }
                }
            }

            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            return fileInfo;
        }

    } // namespace Core

    _DJV_STRING_OPERATOR_LABEL(Core::FileInfoUtil::SORT, Core::FileInfoUtil::sortLabels())

} // namespace djv

