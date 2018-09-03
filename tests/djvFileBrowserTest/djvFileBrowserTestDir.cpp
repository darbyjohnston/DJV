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

#include <djvFileBrowserTestDir.h>

#include <djvDebug.h>
#include <djvFileInfoUtil.h>
#include <djvMemory.h>
#include <djvMemoryBuffer.h>
#include <djvSequenceUtil.h>

#include <QMutexLocker>

#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//------------------------------------------------------------------------------
// djvFileBrowserTestDirRequest
//------------------------------------------------------------------------------

djvFileBrowserTestDirRequest::djvFileBrowserTestDirRequest() :
    sequence     (static_cast<djvSequence::COMPRESS>(0)),
    showHidden   (false),
    reverseSort  (false),
    sortDirsFirst(false),
    reload       (false),
    id           (0)
{}

//------------------------------------------------------------------------------
// djvFileBrowserTestDirResult
//------------------------------------------------------------------------------

djvFileBrowserTestDirResult::djvFileBrowserTestDirResult() :
    id(0)
{}

//------------------------------------------------------------------------------
// djvFileBrowserTestDirWorker::Private
//------------------------------------------------------------------------------

struct djvFileBrowserTestDirWorker::Private
{
    Private() :
        fd   (-1),
        buf  (djvMemory::megabyte),
        timer(0),
        cache(0)
    {}

    djvFileBrowserTestDirRequest request;
    djvFileInfoList              list;
    int                          fd;
    djvMemoryBuffer<quint8>      buf;
    int                          timer;
    djvFileInfo *                cache;
};

//------------------------------------------------------------------------------
// djvFileBrowserTestDirWorker
//------------------------------------------------------------------------------

djvFileBrowserTestDirWorker::djvFileBrowserTestDirWorker(QObject * parent) :
    djvFileBrowserTestAbstractWorker(parent),
    _p(new Private)
{
    //DJV_DEBUG("djvFileBrowserTestDirWorker::djvFileBrowserTestDirWorker");
}

djvFileBrowserTestDirWorker::~djvFileBrowserTestDirWorker()
{
    //DJV_DEBUG("djvFileBrowserTestDirWorker::~djvFileBrowserTestDirWorker");    
}

void djvFileBrowserTestDirWorker::request(const djvFileBrowserTestDirRequest & request)
{
    //DJV_DEBUG("djvFileBrowserTestDirWorker::request");
    //DJV_DEBUG_PRINT("path = " << request.path);
    //DJV_DEBUG_PRINT("sequence = " << request.sequence);
    //DJV_DEBUG_PRINT("id = " << request.id);
    
    const QString tmp = djvFileInfoUtil::fixPath(request.path);
    const bool readDir =
        tmp              != _p->request.path     ||
        request.sequence != _p->request.sequence ||
        request.reload;
    //DJV_DEBUG_PRINT("readDir = " << readDir);
    _p->request      = request;
    _p->request.path = tmp;
    if (readDir)
    {
        // Clean up.
        _p->list.clear();
        if (_p->fd != -1)
        {
            close(_p->fd);
            _p->fd = -1;
        }
        if (_p->timer)
        {
            killTimer(_p->timer);
            _p->timer = 0;
        }
        _p->cache = 0;

        // Open the directory.
        _p->fd = open(_p->request.path.toLatin1().data(), O_RDONLY | O_DIRECTORY);
        if (-1 == _p->fd)
        {
            Q_EMIT result(result());
            return;
        }
        
        // Start reading the directory.
        _p->timer = startTimer(0);
    }
    else
    {
        djvFileBrowserTestDirResult result = this->result();
        process(result);
        Q_EMIT this->result(result);
    }
}

void djvFileBrowserTestDirWorker::finish()
{
    //DJV_DEBUG("djvFileBrowserTestDirWorker::finish");
    if (_p->timer)
    {
        killTimer(_p->timer);
        _p->timer = 0;
    }
    if (_p->fd != -1)
    {
        close(_p->fd);
        _p->fd = -1;
    }
    _p->cache = 0;
}

namespace
{

struct direntLinux64
{
   ino64_t        d_ino;
   off64_t        d_off;
   unsigned short d_reclen;
   unsigned char  d_type;
   char           d_name[];
};

inline bool isDotDir(const char * p, size_t l)
{
    if (1 == l && '.' == p[0] && 0 == p[1])
        return true;
    else if (2 == l && '.' == p[0] && '.' == p[1] && 0 == p[2])
        return true;
    return false;
}

} // namespace

void djvFileBrowserTestDirWorker::timerEvent(QTimerEvent *)
{
    //DJV_DEBUG("djvFileBrowserTestDirWorker::timerEvent");

    // Get the next block of directory entries.    
    quint8 * bp = _p->buf.data();
    int count = 0;
    count = syscall(SYS_getdents64, _p->fd, bp, _p->buf.size());
    //DJV_DEBUG_PRINT("count = " << count);
    if (-1 == count)
    {
        killTimer(_p->timer);
        _p->timer = 0;
        Q_EMIT result(result());
        return;
    }
    
    // If there are no more directory entries we are finished, so process
    // the entries we have collected and emit a signal that we are done.
    if (0 == count)
    {
        for (int i = 0; i < _p->list.count(); ++i)
        {
            _p->list[i]._sequence.sort();
        }
        if (djvSequence::COMPRESS_RANGE == _p->request.sequence)
        {
            for (int i = 0; i < _p->list.count(); ++i)
            {
                if (_p->list[i]._sequence.frames.count())
                {
                    _p->list[i]._sequence.setFrames(
                        _p->list[i]._sequence.start(),
                        _p->list[i]._sequence.end());
                }
            }
        }
        for (int i = 0; i < _p->list.count(); ++i)
        {
            if (djvFileInfo::SEQUENCE == _p->list[i].type())
            {
                _p->list[i]._number = djvSequenceUtil::sequenceToString(
                    _p->list[i]._sequence);
            }
        }
        killTimer(_p->timer);
        _p->timer = 0;
        djvFileBrowserTestDirResult result = this->result();
        process(result);
        Q_EMIT this->result(result);
        return;
    }
    
    // Add the block of directory entries to our list.
    for (int i = 0; i < count;)
    {
        struct direntLinux64 * de = (struct direntLinux64 *)(bp + i);
        if (de->d_ino != DT_UNKNOWN)
        {
            size_t l = strlen(de->d_name);
            if (! isDotDir(de->d_name, l))
            {
                if (! _p->list.count())
                {
                    _p->list.append(djvFileInfo(_p->request.path + de->d_name));
                }
                else
                {
                    const djvFileInfo tmp(_p->request.path + de->d_name);
                    int i = 0;
                    if (_p->request.sequence && _p->cache)
                    {
                        if (! _p->cache->addSequence(tmp))
                        {
                            _p->cache = 0;
                        }
                    }
                    if (_p->request.sequence && ! _p->cache)
                    {
                        for (; i < _p->list.count(); ++i)
                        {
                            if (_p->list[i].addSequence(tmp))
                            {
                                _p->cache = &_p->list[i];
                                break;
                            }
                        }
                    }
                    if (! _p->request.sequence || i == _p->list.count())
                    {
                        _p->list.append(tmp);
                    }
                }
            }
        }
        i += de->d_reclen;
    }
}

void djvFileBrowserTestDirWorker::process(djvFileBrowserTestDirResult & result)
{
    result.list = _p->list;

    // Filter directory contents.
    if (_p->request.filterText.length() > 0 || ! _p->request.showHidden)
    {
        const djvFileInfoUtil::FILTER filter =
            ! _p->request.showHidden ?
            djvFileInfoUtil::FILTER_HIDDEN :
            djvFileInfoUtil::FILTER_NONE;

        djvFileInfoUtil::filter(result.list, filter, _p->request.filterText);
    }

    // Sort directory contents.
    djvFileInfoUtil::SORT sort = static_cast<djvFileInfoUtil::SORT>(0);

    switch (_p->request.sort)
    {
        case djvFileBrowserTestUtil::NAME:
            sort = djvFileInfoUtil::SORT_NAME;
            break;
        case djvFileBrowserTestUtil::SIZE:
            sort = djvFileInfoUtil::SORT_SIZE;
            break;
#if ! defined(DJV_WINDOWS)
        case djvFileBrowserTestUtil::USER:
            sort = djvFileInfoUtil::SORT_USER;
            break;
#endif
        case djvFileBrowserTestUtil::PERMISSIONS:
            sort = djvFileInfoUtil::SORT_PERMISSIONS;
            break;
        case djvFileBrowserTestUtil::TIME:
            sort = djvFileInfoUtil::SORT_TIME;
            break;
        default: break;
    }
    djvFileInfoUtil::sort(result.list, sort, _p->request.reverseSort);
    if (_p->request.sortDirsFirst)
    {
        djvFileInfoUtil::sortDirsFirst(result.list);
    }
}

djvFileBrowserTestDirResult djvFileBrowserTestDirWorker::result() const
{
    djvFileBrowserTestDirResult result;
    result.id = _p->request.id;
    return result; 
}

//------------------------------------------------------------------------------
// djvFileBrowserTestDir
//------------------------------------------------------------------------------

djvFileBrowserTestDir::djvFileBrowserTestDir(QObject * parent) :
    QObject(parent),
    _worker(new djvFileBrowserTestDirWorker)
{
    connect(
        _worker.data(),
        SIGNAL(result(const djvFileBrowserTestDirResult &)),
        SIGNAL(result(const djvFileBrowserTestDirResult &)));
    _worker->connect(
        this,
        SIGNAL(requestDir(const djvFileBrowserTestDirRequest &)),
        SLOT(request(const djvFileBrowserTestDirRequest &)));
    _worker->connect(
        &_thread,
        SIGNAL(started()),
        SLOT(start()));
    _worker->connect(
        &_thread,
        SIGNAL(finished()),
        SLOT(finish()));
    _worker->moveToThread(&_thread);
    _thread.start();
}

djvFileBrowserTestDir::~djvFileBrowserTestDir()
{
    _thread.quit();
    _thread.wait();
}

void djvFileBrowserTestDir::request(const djvFileBrowserTestDirRequest & request)
{
    Q_EMIT requestDir(request);
}

void djvFileBrowserTestDir::setId(quint64 id)
{
    QMutexLocker locker(_worker->mutex());
    _worker->setId(id);
}

    
