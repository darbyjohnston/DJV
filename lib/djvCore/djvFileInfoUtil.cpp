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

//! \file djvFileInfoUtil.cpp

#include <djvFileInfoUtil.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvError.h>
#include <djvFileIo.h>
#include <djvListUtil.h>
#include <djvMath.h>
#include <djvMemory.h>
#include <djvSequenceUtil.h>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QRegExp>

#include <algorithm>

#if defined(DJV_WINDOWS)
#include <windows.h>
#else // DJV_WINDOWS
#include <dirent.h>
#endif // DJV_WINDOWS

//------------------------------------------------------------------------------
// djvFileInfoUtil
//------------------------------------------------------------------------------

djvFileInfoUtil::~djvFileInfoUtil()
{}

namespace
{

inline bool isPathSeparator(const QChar & c)
{
    return c == '/' || c == '\\';
}

inline bool isSequenceValid(const QChar & c)
{
    return QChar(c).isDigit() || '-' == c || '#' == c;
}

inline bool seqSeparator(const QChar & c)
{
    return '-' == c || ',' == c;
}

inline bool matchPadding(const QString & a, const QString & b)
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

void djvFileInfoUtil::split(
    const QString & in,
    QString &       path,
    QString &       base,
    QString &       number,
    QString &       extension)
{
    //DJV_DEBUG("djvFileInfoUtil::split");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("length = " << in.length());

    path.resize(0);
    base.resize(0);
    number.resize(0);
    extension.resize(0);

    const int length = in.length();

    if (! length)
        return;

    // Extension.

    int i = length - 1;
    int tmp = i;

    for (; in[i] != '.' && ! isPathSeparator(in[i]) && i > 0; --i) ;

    if (i > 0 && '.' == in[i] && ! isPathSeparator(in[i - 1]))
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
            if (! isSequenceValid(in[i - 1]) || seqSeparator(in[i - 1]))
            {
                if (separator != -1 &&
                    ! matchPadding(in.mid(i, separator - i), word))
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

            if (! (isSequenceValid(in[i - 1]) || seqSeparator(in[i - 1])))
                break;
        }

        number = in.mid(i, tmp - i + 1);
        
        --i;
    }

    //DJV_DEBUG_PRINT("number = " << number);

    // Base.

    if (i >= 0 && ! isPathSeparator(in[i]))
    {
        tmp = i;

        for (; i > 0 && ! isPathSeparator(in[i - 1]); --i) ;

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

bool djvFileInfoUtil::exists(const djvFileInfo & in)
{
    //DJV_DEBUG("djvFileInfoUtil::exists");
    //DJV_DEBUG_PRINT("in = " << in);

    QStringList list;

    if (djvFileInfo::SEQUENCE == in.type())
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
            djvFileIo().open(list[i], djvFileIo::READ);

            return true;
        }
        catch (const djvError & error)
        {
            //DJV_DEBUG_PRINT("error = " << error.string());
        }
    }

    return false;
}

namespace
{

inline bool isDotDir(const char * p, size_t l)
{
    if (1 == l && '.' == p[0] && 0 == p[1])
        return true;
    else if (2 == l && '.' == p[0] && '.' == p[1] && 0 == p[2])
        return true;
    return false;
}

} // namespace

djvFileInfoList djvFileInfoUtil::list(
    const QString &       path,
    djvSequence::COMPRESS compress)
{
    //DJV_DEBUG("djvFileInfoUtil::list");
    //DJV_DEBUG_PRINT("path = " << path);
    //DJV_DEBUG_PRINT("compress = " << compress);
    
    djvFileInfoList out;

    djvFileInfo * cache = 0;

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

        if (! isDotDir(p, l))
            out += djvFileInfo(fixedPath + QString(p));

        while (FindNextFile(h, &data))
        {
            p = data.cFileName;
            l = strlen(p);

            if (! isDotDir(p, l))
            {
                if (! out.count())
                {
                    out += djvFileInfo(fixedPath + QString(p));
                }
                else
                {
                    const djvFileInfo tmp(fixedPath + QString(p));

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
                        out += tmp;
                }
            }
        }

        FindClose(h);
    }

#else // DJV_WINDOWS

    DIR * dir = opendir(path.toLatin1().data());
    
    if (dir)
    {
        struct dirent * de = 0;
        
        while ((de = readdir(dir)) != 0)
        {
            const char * p = de->d_name;
            const int l = strlen(p);
            
            //if (1 == l && '.' == p[0] && 0 == p[1])
            //    ;
            //else if (2 == l && '.' == p[0] && '.' == p[1] && 0 == p[2])
            //    ;
            //else
            //    out += djvFileInfo(tmp + QString(p));

            if (! isDotDir(p, l))
            {
                if (! out.count())
                {
                    out += djvFileInfo(fixedPath + QString(p));
                }
                else
                {
                    const djvFileInfo tmp(fixedPath + QString(p));

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
                        out += tmp;
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

    if (djvSequence::COMPRESS_RANGE == compress)
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
        if (djvFileInfo::SEQUENCE == out[i].type())
        {
            out[i]._number = djvSequenceUtil::sequenceToString(out[i]._sequence);
        }
    }

    return out;
}

const djvFileInfo & djvFileInfoUtil::sequenceWildcardMatch(
    const djvFileInfo &     in,
    const djvFileInfoList & list)
{
    for (int i = 0; i < list.count(); ++i)
    {
        const djvFileInfo & tmp = list[i];

        if (
            in.base()      == tmp.base() &&
            in.extension() == tmp.extension())
        {
            return tmp;
        }
    }

    return in;
}

void djvFileInfoUtil::compressSequence(
    djvFileInfoList &     items,
    djvSequence::COMPRESS compress)
{
    if (djvSequence::COMPRESS_OFF == compress)
        return;

    //DJV_DEBUG("djvFileInfoUtil::compressSequence");
    //DJV_DEBUG_PRINT("count = " << items.count());

    int  cache      = 0;
    bool cacheValid = false;
    int  i          = 0;

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
                if (djvFileInfo::SEQUENCE == items[k].type())
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

        if (! seq || ! cacheValid)
        {
            items[i] = items[j];

            if (seq)
            {
                items[i].setType(djvFileInfo::SEQUENCE);
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

    if (djvSequence::COMPRESS_RANGE == compress)
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
        if (djvFileInfo::SEQUENCE == items[i].type())
        {
            items[i]._number = djvSequenceUtil::sequenceToString(items[i]._sequence);
        }
    }
}

QStringList djvFileInfoUtil::expandSequence(const djvFileInfo & in)
{
    QStringList out;

    const djvSequence & sequence = in.sequence();
    const int           count    = sequence.frames.count();

    if (djvFileInfo::SEQUENCE == in.type() && count)
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

void djvFileInfoUtil::filter(
    djvFileInfoList &   items,
    int                 filter,
    const QString &     filterText,
    const QStringList & glob)
{
    //DJV_DEBUG("djvFileInfoUtil::filter");
    //DJV_DEBUG_PRINT("filter = " << filter);
    //DJV_DEBUG_PRINT("filterText = " << filterText);
    //DJV_DEBUG_PRINT("glob = " << glob);

    int i = 0;

    for (int j = 0; j < items.count(); ++j)
    {
        const QString name = items[j].fileName(-1, false);

        //DJV_DEBUG_PRINT("name = " << name);

        bool valid = true;

        const djvFileInfo::TYPE type = items[j].type();

        if ((filter & FILTER_FILES) &&
            (djvFileInfo::FILE == type || djvFileInfo::SEQUENCE == type))
        {
            valid = false;
        }

        if ((filter & FILTER_DIRECTORIES) && djvFileInfo::DIRECTORY == type)
        {
            valid = false;
        }

        if ((filter & FILTER_HIDDEN) && items[j].isDotFile())
        {
            valid = false;
        }

        if (filterText.length() > 0)
        {
            if (! name.contains(filterText, Qt::CaseInsensitive))
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

const QStringList & djvFileInfoUtil::sortLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFileInfoUtil", "Name") <<
        qApp->translate("djvFileInfoUtil", "Type") <<
        qApp->translate("djvFileInfoUtil", "Size") <<
        qApp->translate("djvFileInfoUtil", "User") <<
        qApp->translate("djvFileInfoUtil", "Permissions") <<
        qApp->translate("djvFileInfoUtil", "Time");

    DJV_ASSERT(data.count() == SORT_COUNT);

    return data;
}

namespace
{

inline bool compareName(const djvFileInfo & a, const djvFileInfo & b)
{
    return a.fileName() < b.fileName();
}

inline bool compareType(const djvFileInfo & a, const djvFileInfo & b)
{
    return a.type() < b.type();
}

inline bool compareSize(const djvFileInfo & a, const djvFileInfo & b)
{
    return a.size() < b.size();
}

inline bool compareUser(const djvFileInfo & a, const djvFileInfo & b)
{
    return a.user() < b.user();
}

inline bool comparePermissions(const djvFileInfo & a, const djvFileInfo & b)
{
    return a.permissions() < b.permissions();
}

inline bool compareTime(const djvFileInfo & a, const djvFileInfo & b)
{
    return a.time() < b.time();
}

inline bool compareNameReverse(const djvFileInfo & a, const djvFileInfo & b)
{
    return compareName(b, a);
}

inline bool compareTypeReverse(const djvFileInfo & a, const djvFileInfo & b)
{
    return compareType(b, a);
}

inline bool compareSizeReverse(const djvFileInfo & a, const djvFileInfo & b)
{
    return compareSize(b, a);
}

inline bool compareUserReverse(const djvFileInfo & a, const djvFileInfo & b)
{
    return compareUser(b, a);
}

inline bool comparePermissionsReverse(const djvFileInfo & a, const djvFileInfo & b)
{
    return comparePermissions(b, a);
}

inline bool compareTimeReverse(const djvFileInfo & a, const djvFileInfo & b)
{
    return compareTime(b, a);
}

} // namespace

void djvFileInfoUtil::sort(djvFileInfoList & items, SORT sort, bool reverse)
{
    //DJV_DEBUG("djvFileInfoUtil::sort");
    //DJV_DEBUG_PRINT("items = " << items.count());
    //DJV_DEBUG_PRINT("sort = " << sort);
    //DJV_DEBUG_PRINT("reverse = " << reverse);
    
    typedef bool (Compare)(const djvFileInfo &, const djvFileInfo &);

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

void djvFileInfoUtil::sortDirsFirst(djvFileInfoList & in)
{
    djvFileInfoList dirs, files;
    
    const int count = in.count();
    
    for (int i = 0; i < count; ++i)
    {
        if (in[i].type() == djvFileInfo::DIRECTORY)
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

void djvFileInfoUtil::recent(
    const djvFileInfo & fileInfo,
    djvFileInfoList &   list,
    int                 max)
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

void djvFileInfoUtil::recent(
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

QString djvFileInfoUtil::fixPath(const QString & in)
{
    //DJV_DEBUG("djvFileInfoUtil::fixPath");
    //DJV_DEBUG_PRINT("in = " << in);

    QString out;
    
    const QFileInfo qFileInfo(in);
    
    if (qFileInfo.exists())
    {
        out = qFileInfo.absoluteFilePath();
        
        //DJV_DEBUG_PRINT("out = " << out);
    
        if (qFileInfo.isDir() &&
            out.length() > 0  &&
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

QList<QChar> djvFileInfoUtil::listSeparators = QList<QChar>() << ':' << ';';

const QString djvFileInfoUtil::dot(".");

const QString djvFileInfoUtil::dotDot("..");

djvFileInfo djvFileInfoUtil::commandLine(
    const QString &       fileName,
    djvSequence::COMPRESS sequence)
{
    //DJV_DEBUG("djvFileInfoUtil::commandLine");
    //DJV_DEBUG_PRINT("file name = " << fileName);
    
    djvFileInfo fileInfo(fileName, false);

    // Match wildcards.

    if (sequence && fileInfo.isSequenceWildcard())
    {
        fileInfo = djvFileInfoUtil::sequenceWildcardMatch(
            fileInfo,
            djvFileInfoUtil::list(fileInfo.path(), sequence));

        //DJV_DEBUG_PRINT("  wildcard match = " << fileInfo);
    }

    // Is this is a sequence?

    if (sequence && fileInfo.isSequenceValid())
    {
        fileInfo.setType(djvFileInfo::SEQUENCE);

        //DJV_DEBUG_PRINT("sequence = " << fileInfo);
    }
    
    return fileInfo;
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvFileInfoUtil::SORT, djvFileInfoUtil::sortLabels())

