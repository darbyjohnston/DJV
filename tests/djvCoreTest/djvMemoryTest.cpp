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

//! \file djvMemoryTest.cpp

#include <djvMemoryTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvMemory.h>
#include <djvMemoryBuffer.h>

#include <QString>

void djvMemoryTest::run(int &, char **)
{
    DJV_DEBUG("djvMemoryTest::run");
    
    members();
}

void djvMemoryTest::members()
{
    DJV_DEBUG("djvMemoryTest::members");
    
    {
        const char * tmp = "1234567890";
        
        char * buf = new char [sizeof(tmp)];
        
        DJV_ASSERT(buf);
        
        djvMemory::copy(tmp, buf, sizeof(tmp));
        
        DJV_ASSERT(0 == djvMemory::compare(tmp, buf, sizeof(tmp)));
        
        djvMemory::zero(buf, sizeof(tmp));
        
        DJV_ASSERT(djvMemory::compare(tmp, buf, sizeof(tmp)) > 0);
        
        delete [] buf;
    }
    
    {
        DJV_DEBUG_PRINT("endian = " << djvMemory::endian());
    }
    
    {
        DJV_ASSERT(djvMemory::LSB == djvMemory::endianOpposite(djvMemory::MSB));
        DJV_ASSERT(djvMemory::MSB == djvMemory::endianOpposite(djvMemory::LSB));
    }
    
    {
        djvMemoryBuffer<quint16> tmp(2);
        tmp()[0] =  0;
        tmp()[1] = -1;
        
        djvMemoryBuffer<quint16> buf(2);
        
        djvMemory::copy(tmp(), buf(), sizeof(quint16) * 2);
        djvMemory::convertEndian(buf(), 2, sizeof(quint16));
        
        const quint8 * a = reinterpret_cast<quint8 *>(tmp());
        const quint8 * b = reinterpret_cast<quint8 *>(buf());
        
        DJV_ASSERT(a[0] == b[1]);
        DJV_ASSERT(a[1] == b[0]);
        DJV_ASSERT(a[2] == b[3]);
        DJV_ASSERT(a[3] == b[2]);
        
        djvMemory::zero(buf(), sizeof(quint16) * 2);
        djvMemory::convertEndian(tmp(), buf(), 2, sizeof(quint16));

        DJV_ASSERT(a[0] == b[1]);
        DJV_ASSERT(a[1] == b[0]);
        DJV_ASSERT(a[2] == b[3]);
        DJV_ASSERT(a[3] == b[2]);
    }
    
    {
        djvMemoryBuffer<quint32> tmp(2);
        tmp()[0] =  0;
        tmp()[1] = -1;
        
        djvMemoryBuffer<quint32> buf(2);
        
        djvMemory::copy(tmp(), buf(), sizeof(quint32) * 2);
        djvMemory::convertEndian(buf(), 2, sizeof(quint32));
        
        const quint8 * a = reinterpret_cast<quint8 *>(tmp());
        const quint8 * b = reinterpret_cast<quint8 *>(buf());
        
        DJV_ASSERT(a[0] == b[3]);
        DJV_ASSERT(a[1] == b[2]);
        DJV_ASSERT(a[2] == b[1]);
        DJV_ASSERT(a[3] == b[0]);
        DJV_ASSERT(a[4] == b[7]);
        DJV_ASSERT(a[5] == b[6]);
        DJV_ASSERT(a[6] == b[5]);
        DJV_ASSERT(a[7] == b[4]);
        
        djvMemory::zero(buf(), sizeof(quint32) * 2);
        djvMemory::convertEndian(tmp(), buf(), 2, sizeof(quint32));

        DJV_ASSERT(a[0] == b[3]);
        DJV_ASSERT(a[1] == b[2]);
        DJV_ASSERT(a[2] == b[1]);
        DJV_ASSERT(a[3] == b[0]);
        DJV_ASSERT(a[4] == b[7]);
        DJV_ASSERT(a[5] == b[6]);
        DJV_ASSERT(a[6] == b[5]);
        DJV_ASSERT(a[7] == b[4]);
    }
    
    {
        djvMemoryBuffer<quint64> tmp(2);
        tmp()[0] =  0;
        tmp()[1] = -1;
        
        djvMemoryBuffer<quint64> buf(2);
        
        djvMemory::copy(tmp(), buf(), sizeof(quint64) * 2);
        djvMemory::convertEndian(buf(), 2, sizeof(quint64));
        
        const quint8 * a = reinterpret_cast<quint8 *>(tmp());
        const quint8 * b = reinterpret_cast<quint8 *>(buf());
        
        DJV_ASSERT(a[ 0] == b[ 7]);
        DJV_ASSERT(a[ 1] == b[ 6]);
        DJV_ASSERT(a[ 2] == b[ 5]);
        DJV_ASSERT(a[ 3] == b[ 4]);
        DJV_ASSERT(a[ 4] == b[ 3]);
        DJV_ASSERT(a[ 5] == b[ 2]);
        DJV_ASSERT(a[ 6] == b[ 1]);
        DJV_ASSERT(a[ 7] == b[ 0]);
        DJV_ASSERT(a[ 8] == b[15]);
        DJV_ASSERT(a[ 9] == b[14]);
        DJV_ASSERT(a[10] == b[13]);
        DJV_ASSERT(a[11] == b[12]);
        DJV_ASSERT(a[12] == b[11]);
        DJV_ASSERT(a[13] == b[10]);
        DJV_ASSERT(a[14] == b[ 9]);
        DJV_ASSERT(a[15] == b[ 8]);
        
        djvMemory::zero(buf(), sizeof(quint64) * 2);
        djvMemory::convertEndian(tmp(), buf(), 2, sizeof(quint64));

        DJV_ASSERT(a[ 0] == b[ 7]);
        DJV_ASSERT(a[ 1] == b[ 6]);
        DJV_ASSERT(a[ 2] == b[ 5]);
        DJV_ASSERT(a[ 3] == b[ 4]);
        DJV_ASSERT(a[ 4] == b[ 3]);
        DJV_ASSERT(a[ 5] == b[ 2]);
        DJV_ASSERT(a[ 6] == b[ 1]);
        DJV_ASSERT(a[ 7] == b[ 0]);
        DJV_ASSERT(a[ 8] == b[15]);
        DJV_ASSERT(a[ 9] == b[14]);
        DJV_ASSERT(a[10] == b[13]);
        DJV_ASSERT(a[11] == b[12]);
        DJV_ASSERT(a[12] == b[11]);
        DJV_ASSERT(a[13] == b[10]);
        DJV_ASSERT(a[14] == b[ 9]);
        DJV_ASSERT(a[15] == b[ 8]);
    }
    
    {
        DJV_DEBUG_PRINT(djvMemory::sizeLabel(djvMemory::terabyte));
        DJV_DEBUG_PRINT(djvMemory::sizeLabel(djvMemory::gigabyte));
        DJV_DEBUG_PRINT(djvMemory::sizeLabel(djvMemory::megabyte));
        DJV_DEBUG_PRINT(djvMemory::sizeLabel(djvMemory::kilobyte));
    }
}

