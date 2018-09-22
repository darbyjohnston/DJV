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

#include <djvCore/FileInfoUtil.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Error.h>
#include <djvCore/FileIO.h>
#include <djvCore/ListUtil.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>
#include <djvCore/SequenceUtil.h>

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

            bool isSequenceValid(const QChar & c)
            {
                return QChar(c).isDigit() || '-' == c || '#' == c;
            }

            bool seqSeparator(const QChar & c)
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
            //DJV_DEBUG_PRINT("length = " << in.length());

            path.resize(0);
            base.resize(0);
            number.resize(0);
            extension.resize(0);

            const int length = in.length();
            if (!length)
                return;

            // Extension.
            int i = length - 1;
            int tmp = i;
            for (; in[i] != '.' && !isPathSeparator(in[i]) && i > 0; --i);
            if (i > 0 && '.' == in[i] && !isPathSeparator(in[i - 1]))
            {
                extension = in.mid(i, tmp - i + 1);

                --i;
            }
            else
            {
                i = length - 1;
            }
            //DJV_DEBUG_PRINT("extension = " << extension);

            // Number.
            if (i >= 0 && isSequenceValid(in[i]))
            {
                tmp = i;
                int separator = -1;
                QString word;
                for (; i > 0; --i)
                {
                    if (!isSequenceValid(in[i - 1]) || seqSeparator(in[i - 1]))
                    {
                        if (separator != -1 &&
                            !matchPadding(in.mid(i, separator - i), word))
                        {
                            i = separator + 1;
                            break;
                        }
                        else
                        {
                            word = in.mid(
                                i,
                                -1 == separator ? (tmp - i + 1) : (separator - i));
                            separator = i - 1;
                        }
                    }
                    if (!(isSequenceValid(in[i - 1]) || seqSeparator(in[i - 1])))
                        break;
                }
                number = in.mid(i, tmp - i + 1);
                --i;
            }
            //DJV_DEBUG_PRINT("number = " << number);

            // Base.
            if (i >= 0 && !isPathSeparator(in[i]))
            {
                tmp = i;
                for (; i > 0 && !isPathSeparator(in[i - 1]); --i);
                base = in.mid(i, tmp - i + 1);
                --i;
            }
            //DJV_DEBUG_PRINT("base = " << base);

            // Path.
            if (i >= 0)
            {
                path = in.mid(0, i + 1);
            }
            //DJV_DEBUG_PRINT("path = " << path);
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
            bool isDotDir(const char * p, size_t l)
            {
                if (1 == l && '.' == p[0] && 0 == p[1])
                    return true;
                else if (2 == l && '.' == p[0] && '.' == p[1] && 0 == p[2])
                    return true;
                return false;
            }

        } // namespace

        FileInfoList FileInfoUtil::list(
            const QString &    path,
            Sequence::COMPRESS compress)
        {
            //DJV_DEBUG("FileInfoUtil::list");
            //DJV_DEBUG_PRINT("path = " << path);
            //DJV_DEBUG_PRINT("compress = " << compress);

            FileInfoList out;
            FileInfo * cache = 0;
            QString fixedPath = fixPath(path);

#if defined(DJV_WINDOWS)
            WIN32_FIND_DATA data;
            HANDLE h = FindFirstFileEx(
                QString(fixedPath + "*").toLatin1().data(),
                FindExInfoBasic,
                &data,
                FindExSearchNameMatch,
                NULL,
                FIND_FIRST_EX_LARGE_FETCH);
            if (h != INVALID_HANDLE_VALUE)
            {
                const char * p = data.cFileName;
                size_t l = strlen(p);
                if (!isDotDir(p, l))
                {
                    out.append(FileInfo(fixedPath + QString(p)));
                }
                while (FindNextFile(h, &data))
                {
                    p = data.cFileName;
                    l = strlen(p);
                    if (!isDotDir(p, l))
                    {
                        if (!out.count())
                        {
                            out.append(FileInfo(fixedPath + QString(p)));
                        }
                        else
                        {
                            const FileInfo tmp(fixedPath + QString(p));
                            int i = 0;
                            if (compress && cache)
                            {
                                if (!cache->addSequence(tmp))
                                {
                                    cache = 0;
                                }
                            }
                            if (compress && !cache)
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
                            if (!compress || i == out.count())
                            {
                                out.append(tmp);
                            }
                        }
                    }
                }
                FindClose(h);
            }
            /*#elif defined(DJV_LINUX)
                struct linux_dirent64
                {
                   ino64_t        d_ino;
                   off64_t        d_off;
                   unsigned short d_reclen;
                   unsigned char  d_type;
                   char           d_name[];
                };
                int fd = open(path.toLatin1().data(), O_RDONLY | O_DIRECTORY);
                if (fd != -1)
                {
                    std::vector<quint8> buf(Memory::megabyte);
                    quint8 * p = buf.data();
                    while (1)
                    {
                        int readCount = syscall(SYS_getdents64, fd, p, buf.size());
                        if (-1 == readCount)
                            break;
                        if (0 == readCount)
                            break;
                        for (int i = 0; i < readCount;)
                        {
                            struct linux_dirent64 * de = (struct linux_dirent64 *)(p + i);
                            if (de->d_ino != DT_UNKNOWN)
                            {
                                size_t l = strlen(de->d_name);
                                if (! isDotDir(de->d_name, l))
                                {
                                    if (! out.count())
                                    {
                                        out.append(FileInfo(fixedPath + de->d_name));
                                    }
                                    else
                                    {
                                        const FileInfo tmp(fixedPath + de->d_name);
                                        int i = 0;
                                        if (compress && cache)
                                        {
                                            if (! cache->addSequence(tmp))
                                            {
                                                cache = 0;
                                            }
                                        }
                                        if (compress && ! cache)
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
                                        if (! compress || i == out.count())
                                        {
                                            out.append(tmp);
                                        }
                                    }
                                }
                            }
                            i += de->d_reclen;
                        }
                    }
                    close (fd);
                }
            */
#else // DJV_WINDOWS

            DIR * dir = opendir(path.toLatin1().data());
            if (dir)
            {
                struct dirent * de = 0;
                while ((de = readdir(dir)) != 0)
                {
                    const char * p = de->d_name;
                    const int l = strlen(p);
                    if (!isDotDir(p, l))
                    {
                        if (!out.count())
                        {
                            out.append(FileInfo(fixedPath + QString(p)));
                        }
                        else
                        {
                            const FileInfo tmp(fixedPath + QString(p));
                            int i = 0;
                            if (compress && cache)
                            {
                                if (!cache->addSequence(tmp))
                                {
                                    cache = 0;
                                }
                            }
                            if (compress && !cache)
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
                            if (!compress || i == out.count())
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
            if (Sequence::COMPRESS_RANGE == compress)
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
                    out[i]._number = SequenceUtil::sequenceToString(out[i]._sequence);
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

        void FileInfoUtil::compressSequence(
            FileInfoList &     items,
            Sequence::COMPRESS compress)
        {
            if (Sequence::COMPRESS_OFF == compress)
                return;

            //DJV_DEBUG("FileInfoUtil::compressSequence");
            //DJV_DEBUG_PRINT("count = " << items.count());

            int  cache = 0;
            bool cacheValid = false;
            int  i = 0;
            for (int j = 0; j < items.count(); ++j)
            {
                //DJV_DEBUG_PRINT("in = " << in[j]);
                //DJV_DEBUG_PRINT("in seq = " << in[j].seq());

                const bool seq = items[j].isSequenceValid();
                //DJV_DEBUG_PRINT("seq = " << seq);
                if (seq)
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
                if (!seq || !cacheValid)
                {
                    items[i] = items[j];
                    if (seq)
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

            if (Sequence::COMPRESS_RANGE == compress)
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
                    items[i]._number = SequenceUtil::sequenceToString(items[i]._sequence);
                }
            }
        }

        QStringList FileInfoUtil::expandSequence(const FileInfo & in)
        {
            QStringList out;
            const Sequence & sequence = in.sequence();
            const int count = sequence.frames.count();
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

        QList<QChar> FileInfoUtil::listSeparators = QList<QChar>() << ':' << ';';

        const QString FileInfoUtil::dot(".");

        const QString FileInfoUtil::dotDot("..");

        FileInfo FileInfoUtil::parse(
            const QString &    fileName,
            Sequence::COMPRESS sequence,
            bool               autoSequence)
        {
            //DJV_DEBUG("FileInfoUtil::parse");
            //DJV_DEBUG_PRINT("fileName = " << fileName);
            //DJV_DEBUG_PRINT("sequence = " << sequence);
            //DJV_DEBUG_PRINT("autoSequence = " << autoSequence);

            FileInfo fileInfo(fileName, false);

            // Match wildcards.
            if (sequence && fileInfo.isSequenceWildcard())
            {
                fileInfo = FileInfoUtil::sequenceWildcardMatch(
                    fileInfo,
                    FileInfoUtil::list(fileInfo.path(), sequence));
                //DJV_DEBUG_PRINT("  wildcard match = " << fileInfo);
            }

            // Is this is a sequence?
            if (sequence && fileInfo.isSequenceValid())
            {
                //DJV_DEBUG_PRINT("sequence");
                fileInfo.setType(FileInfo::SEQUENCE);
            }

            // Find other files in the directory that match this sequence.
            if (sequence && autoSequence)
            {
                //DJV_DEBUG_PRINT("auto sequence");
                const FileInfoList items = FileInfoUtil::list(fileInfo.path());
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

