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

//! \file djvFileBrowserTestDirWorker.cpp

#include <djvFileBrowserTestDirWorker.h>

#include <djvDebug.h>
#include <djvFileInfoUtil.h>
#include <djvMemory.h>
#include <djvSequenceUtil.h>

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
    sequence(static_cast<djvSequence::COMPRESS>(0)),
    id      (0)
{}
    
//------------------------------------------------------------------------------
// djvFileBrowserTestDirResult
//------------------------------------------------------------------------------

djvFileBrowserTestDirResult::djvFileBrowserTestDirResult() :
    id(0)
{}
    
//------------------------------------------------------------------------------
// djvFileBrowserTestDirWorker
//------------------------------------------------------------------------------

djvFileBrowserTestDirWorker::djvFileBrowserTestDirWorker(QObject * parent) :
    djvFileBrowserTestAbstractWorker(parent),
    _fd   (-1),
    _buf  (djvMemory::megabyte),
    _timer(0),
    _cache(0)
{}
    
djvFileBrowserTestDirWorker::~djvFileBrowserTestDirWorker()
{}

void djvFileBrowserTestDirWorker::request(const djvFileBrowserTestDirRequest & request)
{
    //DJV_DEBUG("djvFileBrowserTestDirWorker::request");
    //DJV_DEBUG_PRINT("path = " << request.path);
    //DJV_DEBUG_PRINT("sequence = " << request.sequence);
    //DJV_DEBUG_PRINT("id = " << request.id);
    
    const QString tmp = djvFileInfoUtil::fixPath(request.path);
    
    const bool list =
        tmp != _request.path ||
        request.sequence != _request.sequence;
    
    _request = request;
    _request.path = tmp;
    
    if (list)
    {
        // Clean up.
        
        _list.clear();

        if (_fd != -1)
        {
            close(_fd);
        
            _fd = -1;
        }

        if (_timer)
        {
            killTimer(_timer);

            _timer = 0;
        }
        
        _cache = 0;

        // Open the new directory.
        
        _fd = open(_request.path.toLatin1().data(), O_RDONLY | O_DIRECTORY);
        
        if (_fd != -1)
        {
            _timer = startTimer(0);
        }
    }
    else
    {
        process();
    }
}

void djvFileBrowserTestDirWorker::finish()
{
    //DJV_DEBUG("djvFileBrowserTestDirWorker::finish");

    if (_timer)
    {
        killTimer(_timer);

        _timer = 0;
    }
    
    if (_fd != -1)
    {
        close(_fd);
        
        _fd = -1;
    }
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

    // Get the next block of directory entries from the OS.
    
    quint8 * bp = _buf.data();
    
    int count = 0;
    
    count = syscall(SYS_getdents64, _fd, bp, _buf.size());
    
    //DJV_DEBUG_PRINT("count = " << count);
    
    if (-1 == count)
    {
        killTimer(_timer);
        
        _timer = 0;
        
        return;
    }
    
    // If there are no more directory entries we are finished, so process
    // the entries we have collected and emit a signal that we are done.
    
    if (0 == count)
    {
        for (int i = 0; i < _list.count(); ++i)
        {
            _list[i]._sequence.sort();
        }

        if (djvSequence::COMPRESS_RANGE == _request.sequence)
        {
            for (int i = 0; i < _list.count(); ++i)
            {
                if (_list[i]._sequence.frames.count())
                {
                    _list[i]._sequence.setFrames(
                        _list[i]._sequence.start(),
                        _list[i]._sequence.end());
                }
            }
        }

        for (int i = 0; i < _list.count(); ++i)
        {
            if (djvFileInfo::SEQUENCE == _list[i].type())
            {
                _list[i]._number = djvSequenceUtil::sequenceToString(_list[i]._sequence);
            }
        }
        
        killTimer(_timer);
        
        _timer = 0;

        process();

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
                if (! _list.count())
                {
                    _list.append(djvFileInfo(_request.path + de->d_name));
                }
                else
                {
                    const djvFileInfo tmp(_request.path + de->d_name);

                    int i = 0;

                    if (_request.sequence && _cache)
                    {
                        if (! _cache->addSequence(tmp))
                        {
                            _cache = 0;
                        }
                    }

                    if (_request.sequence && ! _cache)
                    {
                        for (; i < _list.count(); ++i)
                        {
                            if (_list[i].addSequence(tmp))
                            {
                                _cache = &_list[i];

                                break;
                            }
                        }
                    }

                    if (! _request.sequence || i == _list.count())
                    {
                        _list.append(tmp);
                    }
                }
            }
        }
        
        i += de->d_reclen;
    }
}

void djvFileBrowserTestDirWorker::process()
{
    djvFileBrowserTestDirResult result;
    result.list = _list;
    result.id   = _request.id;

    // Filter directory contents.
    
    if (_request.filterText.length() > 0 || ! _request.showHidden)
    {
        const djvFileInfoUtil::FILTER filter =
            ! _request.showHidden ?
            djvFileInfoUtil::FILTER_HIDDEN :
            djvFileInfoUtil::FILTER_NONE;

        djvFileInfoUtil::filter(result.list, filter, _request.filterText);
    }

    // Sort directory contents.

    djvFileInfoUtil::SORT sort = static_cast<djvFileInfoUtil::SORT>(0);

    switch (_request.sort)
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

    djvFileInfoUtil::sort(result.list, sort, _request.reverseSort);

    if (_request.sortDirsFirst)
    {
        djvFileInfoUtil::sortDirsFirst(result.list);
    }

    Q_EMIT this->result(result);
}

