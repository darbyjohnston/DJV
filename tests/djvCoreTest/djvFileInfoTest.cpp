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

//! \file djvFileInfoTest.cpp

#include <djvFileInfoTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvFileInfo.h>
#include <djvFileInfoUtil.h>
#include <djvFileIo.h>

void djvFileInfoTest::run(int &, char **)
{
    DJV_DEBUG("djvFileInfoTest::run");
    
    ctors(); 
    members();
    operators();
}

void djvFileInfoTest::ctors()
{
    DJV_DEBUG("djvFileInfoTest::ctors");
    
    {
        djvFileInfo fileInfo;
        
        DJV_ASSERT(fileInfo.isEmpty());
    }
    
    {
        const QString fileName("djvFileInfoTest.test");
        
        djvFileInfo fileInfo(fileName);
        
        DJV_ASSERT(fileName == fileInfo.fileName());
    }
}

void djvFileInfoTest::members()
{
    DJV_DEBUG("djvFileInfoTest::members");
    
    {
        const QString fileName("image.0001-0100.dpx");
        
        djvFileInfo fileInfo(fileName);
        
        DJV_ASSERT(fileName == fileInfo.fileName());
        DJV_ASSERT(fileName == fileInfo.fileName(1));

        fileInfo.setType(djvFileInfo::SEQUENCE);
        
        DJV_ASSERT(fileName == fileInfo.fileName());
        DJV_ASSERT("image.0001.dpx" == fileInfo.fileName(1));
    }

    {
        const QString fileName("image.1.dpx");
        
        djvFileInfo fileInfo(fileName);
        
        fileInfo.setPath("/");
        
        DJV_ASSERT("/image.1.dpx" == fileInfo.fileName());
        
        fileInfo.setBase("movie");

        DJV_ASSERT("/movie1.dpx" == fileInfo.fileName());
        
        fileInfo.setNumber("100");

        DJV_ASSERT("/movie100.dpx" == fileInfo.fileName());
        
        fileInfo.setExtension(".cin");

        DJV_ASSERT("/movie100.cin" == fileInfo.fileName());
    }
    
    {
        const QString fileName("djvFileInfoTest.test");

        {
            djvFileIo io;
            io.open(fileName, djvFileIo::WRITE);
            io.close();
        }

        DJV_ASSERT(djvFileInfo(fileName).exists());
    }
    
    {
        djvFileInfo fileInfo;
        fileInfo.setType(djvFileInfo::SEQUENCE);
        fileInfo.setSize(1);
        fileInfo.setUser(2);
        fileInfo.setPermissions(3);
        fileInfo.setTime(4);
        
        DJV_ASSERT(! fileInfo.isDotFile());   
        DJV_ASSERT(djvFileInfo::SEQUENCE == fileInfo.type());     
        DJV_ASSERT(1 == fileInfo.size()); 
        DJV_ASSERT(2 == fileInfo.user()); 
        DJV_ASSERT(3 == fileInfo.permissions()); 
        DJV_ASSERT(4 == fileInfo.time()); 
    }

    {
        djvFileInfo fileInfo("image.dpx");
        
        const djvSequence sequence(1, 3);
        
        fileInfo.setSequence(sequence);
        
        DJV_ASSERT(sequence == fileInfo.sequence());
        DJV_ASSERT(fileInfo.isSequenceValid());
        DJV_ASSERT("image1-3.dpx" == fileInfo.fileName());
        
        fileInfo.setSequence(djvSequence(djvFrameList() << 3 << 2 << 1));
        fileInfo.sortSequence();
        
        DJV_ASSERT(sequence == fileInfo.sequence());
        DJV_ASSERT(fileInfo.isSequenceValid());
        DJV_ASSERT("image1-3.dpx" == fileInfo.fileName());
    }

    {
        djvFileInfo fileInfo("image.dpx");
        
        DJV_ASSERT(! fileInfo.addSequence(djvFileInfo("image1.dpx")));
    }
    
    {
        djvFileInfo fileInfo("image.1.dpx");
        
        DJV_ASSERT(! fileInfo.addSequence(djvFileInfo("image.cin")));
        DJV_ASSERT(! fileInfo.addSequence(djvFileInfo("movie.dpx")));
        DJV_ASSERT(! fileInfo.addSequence(djvFileInfo("image.dpx")));
        DJV_ASSERT(! fileInfo.addSequence(djvFileInfo("image..dpx")));
        
        DJV_ASSERT(fileInfo.addSequence(djvFileInfo("image.2.dpx")));
        
        DJV_ASSERT(djvSequence(1, 2) == fileInfo.sequence());
        
        DJV_ASSERT(fileInfo.addSequence(djvFileInfo("image.0003.dpx")));
        
        DJV_ASSERT(djvSequence(1, 3, 4) == fileInfo.sequence());
        
        djvFileInfo tmp("image.0004.dpx");
        tmp.setSize(1);
        tmp.setUser(2);
        tmp.setTime(3);
        fileInfo.addSequence(tmp);
        
        DJV_ASSERT(1 == fileInfo.size());
        DJV_ASSERT(2 == fileInfo.user());
        DJV_ASSERT(3 == fileInfo.time());
    }
    
    {
        DJV_DEBUG_PRINT("permissions = " << djvFileInfo::permissionsLabels());
        
        DJV_DEBUG_PRINT("rwx = " << djvFileInfo::permissionsLabel(
            djvFileInfo::READ  |
            djvFileInfo::WRITE |
            djvFileInfo::EXEC));
    }
    
    {
        const QString fileName("image.1.dpx");
        
        djvFileInfo fileInfo(fileName);
        
        DJV_ASSERT(fileName == (QString)fileInfo);
    }
}

void djvFileInfoTest::operators()
{
    DJV_DEBUG("djvFileInfoTest::operators");
    
    {
        const djvFileInfo::TYPE a = djvFileInfo::DIRECTORY;
        
        QStringList tmp;
        
        tmp << a;
        
        djvFileInfo::TYPE b = static_cast<djvFileInfo::TYPE>(0);
        
        tmp >> b;
        
        DJV_ASSERT(a == b);
    }
    
    {
        DJV_DEBUG_PRINT("type = " << djvFileInfo::DIRECTORY);
        
        DJV_DEBUG_PRINT(djvFileInfo());
    }
}

