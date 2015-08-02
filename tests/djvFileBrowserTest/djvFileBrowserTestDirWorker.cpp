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

djvFileBrowserTestDirWorker::djvFileBrowserTestDirWorker(QObject * parent) :
    QObject(parent),
    _sequence(static_cast<djvSequence::COMPRESS>(0)),
    _id      (0),
    _fd      (-1),
    _buf     (djvMemory::megabyte),
    _timer   (0),
    _cache   (0)
{}
    
djvFileBrowserTestDirWorker::~djvFileBrowserTestDirWorker()
{}

void djvFileBrowserTestDirWorker::dir(
    const QString &       path,
    djvSequence::COMPRESS sequence,
    quint64               id)
{
    //DJV_DEBUG("djvFileBrowserTestDirWorker::dir");
    //DJV_DEBUG_PRINT("path = " << path);
    //DJV_DEBUG_PRINT("sequence = " << sequence);
    //DJV_DEBUG_PRINT("id = " << id);
    
    _path = djvFileInfoUtil::fixPath(path);

    _sequence = sequence;
    
    _id = id;
    
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
    
    _fd = open(_path.toLatin1().data(), O_RDONLY | O_DIRECTORY);
    
    if (_fd != -1)
    {
        _timer = startTimer(0);
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

        if (djvSequence::COMPRESS_RANGE == _sequence)
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

        Q_EMIT dirFinished(_list, _id);
        
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
                    _list.append(djvFileInfo(_path + de->d_name));
                }
                else
                {
                    const djvFileInfo tmp(_path + de->d_name);

                    int i = 0;

                    if (_sequence && _cache)
                    {
                        if (! _cache->addSequence(tmp))
                        {
                            _cache = 0;
                        }
                    }

                    if (_sequence && ! _cache)
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

                    if (! _sequence || i == _list.count())
                    {
                        _list.append(tmp);
                    }
                }
            }
        }
        
        i += de->d_reclen;
    }
}


