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

//! \file djvFileIoTest.cpp

#include <djvFileIoTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvFileIo.h>

void djvFileIoTest::run(int &, char **)
{
    DJV_DEBUG("djvFileIoTest::run");
    
    const QString fileName = "djvFileIoTest.test";
    
    for (int i = 0; i < 2; ++i)
    {
        djvFileIo io;
        
		const bool endian = static_cast<bool>(i);
		
		DJV_DEBUG_PRINT("endian = " << endian);
		
        io.setEndian(endian);
        
        DJV_ASSERT(endian == io.endian());
        
        const qint8   write8   = 127;
        const quint8  writeU8  = 127;
        const qint16  write16  = 32767;
        const quint16 writeU16 = 32767;
        const qint32  write32  = 65535;
        const quint32 writeU32 = 65535;
        const float   writeF32 = 0.5f;
        const qint8   rewrite8 = 63;
        
        io.open(fileName, djvFileIo::WRITE);
		
		DJV_DEBUG_PRINT("open");
        
        DJV_ASSERT(fileName == io.fileName());
        DJV_ASSERT(io.isValid());
        
        io.set8  (&write8,   1);
        io.set8  (&write8,   1);
        io.setU8 (&writeU8,  1);
        io.set16 (&write16,  1);
        io.setU16(&writeU16, 1);
        io.set32 (&write32,  1);
        io.setU32(&writeU32, 1);
        io.setF32(&writeF32, 1);
        
		DJV_DEBUG_PRINT("set");

        DJV_ASSERT(
            sizeof(qint8)   +
            sizeof(qint8)   +
            sizeof(quint8)  +
            sizeof(qint16)  +
            sizeof(quint16) +
            sizeof(qint32)  +
            sizeof(quint32) +
            sizeof(float) == io.pos());
		
		DJV_DEBUG_PRINT("set 2");
        
        io.set8  (write8);
        io.setU8 (writeU8);
        io.set16 (write16);
        io.setU16(writeU16);
        io.set32 (write32);
        io.setU32(writeU32);
        io.setF32(writeF32);

#if ! defined(DJV_WINDOWS)

        //! \todo On Windows this causes an error with the message "Cannot
        //! create a file when that file already exists."

        io.setPos(0);
		
		DJV_DEBUG_PRINT("pos");
        
        DJV_ASSERT(0 == io.pos());
		
		DJV_DEBUG_PRINT("rewrite");
        
        io.set8(&rewrite8, 1);
        
#endif // DJV_WINDOWS

        const quint64 size = io.size();
                
        io.close();
        
        qint8   read8   = 0;
        quint8  readU8  = 0;
        qint16  read16  = 0;
        quint16 readU16 = 0;
        qint32  read32  = 0;
        quint32 readU32 = 0;
        float   readF32 = 0.0f;
        
        io.open(fileName, djvFileIo::READ);
        io.readAhead();
                
        DJV_ASSERT(io.isValid());
        DJV_ASSERT(size == io.size());

        io.get8  (&read8);
        io.get8  (&read8);
        io.getU8 (&readU8);
        io.get16 (&read16);
        io.getU16(&readU16);
        io.get32 (&read32);
        io.getU32(&readU32);
        io.getF32(&readF32);
        
        DJV_ASSERT(
            sizeof(qint8)   +
            sizeof(qint8)   +
            sizeof(quint8)  +
            sizeof(qint16)  +
            sizeof(quint16) +
            sizeof(qint32)  +
            sizeof(quint32) +
            sizeof(float) == io.pos());
        
        DJV_ASSERT(write8   == read8);
        DJV_ASSERT(writeU8  == readU8);
        DJV_ASSERT(write16  == read16);
        DJV_ASSERT(writeU16 == readU16);
        DJV_ASSERT(write32  == read32);
        DJV_ASSERT(writeU32 == readU32);
        DJV_ASSERT(writeF32 == readF32);
        
        io.get8  (&read8);
        io.getU8 (&readU8);
        io.get16 (&read16);
        io.getU16(&readU16);
        io.get32 (&read32);
        io.getU32(&readU32);
        io.getF32(&readF32);
        
        DJV_ASSERT(write8   == read8);
        DJV_ASSERT(writeU8  == readU8);
        DJV_ASSERT(write16  == read16);
        DJV_ASSERT(writeU16 == readU16);
        DJV_ASSERT(write32  == read32);
        DJV_ASSERT(writeU32 == readU32);
        DJV_ASSERT(writeF32 == readF32);
        
#if ! defined (DJV_WINDOWS)

        io.setPos(0);
        
        DJV_ASSERT(0 == io.pos());
        
        DJV_ASSERT(io.mmapP() < io.mmapEnd());
        DJV_ASSERT(*reinterpret_cast<const qint8 *>(io.mmapP()) == rewrite8);
        
        io.get8(&read8);

        DJV_ASSERT(rewrite8 == read8);
        
#endif // DJV_WINDOWS

        io.setPos(0);
        io.seek(1);

        io.getU8(&readU8);

        DJV_ASSERT(writeU8 == readU8);
        
        try
        {
            io.setPos(size);
            
            io.get8(&read8);
            
            DJV_ASSERT(0);
        }
        catch (...)
        {}
        
        try
        {
            io.set8(&write8, 1);
            
            DJV_ASSERT(0);
        }
        catch (...)
        {}
        
        try
        {
            io.setPos(size);
            io.seek(1);
            
            DJV_ASSERT(0);
        }
        catch (...)
        {}
    }
}

