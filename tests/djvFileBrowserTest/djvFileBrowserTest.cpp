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

//! \file djvFileBrowserTest.cpp

#define _GNU_SOURCE

#include <djvFileBrowserTest.h>

#include <djvError.h>
#include <djvMemoryBuffer.h>

#include <QTimer>

#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

djvFileBrowserTestApplication::djvFileBrowserTestApplication(int & argc, char ** argv) :
    QCoreApplication(argc, argv)
{
    _context.reset(new djvCoreContext);
    
    if (argc != 2)
    {
        _context->printMessage("Usage: djvFileBrowserTest (path)");
        
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
        _path = argv[1];
        
        QTimer::singleShot(0, this, SLOT(work()));
    }
}

void djvFileBrowserTestApplication::commandLineExit()
{
    exit(1);
}

struct linux_dirent64
{
   ino64_t        d_ino;
   off64_t        d_off;
   unsigned short d_reclen;
   unsigned char  d_type;
   char           d_name[];
};

void djvFileBrowserTestApplication::work()
{
    int r = 0;
    
    djvMemoryBuffer<quint8> buf(djvMemory::megabyte);
    
    int fd = 0;
    
    do
    {
        fd = open(_path.toLatin1().data(), O_RDONLY | O_DIRECTORY);
        
        if (-1 == fd)
        {
            printf("Cannot open: %s\n", _path.toLatin1().data());
            
            r = 1;
            
            break;
        }

        quint8 * p = buf.data();

        while (1)
        {
            int readCount = syscall(SYS_getdents64, fd, p, buf.size());
                                    
            if (-1 == readCount)
            {
                printf("Error reading: %s\n", _path.toLatin1().data());
                
                r = 1;
                
                break;
            }
            
            if (0 == readCount)
                break;
            
            for (int i = 0; i < readCount;)
            {
                struct linux_dirent64 * de = (struct linux_dirent64 *)(p + i);
                
                if (de->d_ino != DT_UNKNOWN)
                {
                    printf("%s\n", de->d_name);
                }
                
                i += de->d_reclen;
            }
        }

    } while (0);
    
    close(fd);
    
    exit(r);
}

int main(int argc, char ** argv)
{
    return djvFileBrowserTestApplication(argc, argv).exec();
}

