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

#include <djvCoreTest/MemoryTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Memory.h>

#include <QString>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void MemoryTest::run(int &, char **)
        {
            DJV_DEBUG("MemoryTest::run");
            members();
        }

        void MemoryTest::members()
        {
            DJV_DEBUG("MemoryTest::members");
            {
                const char * tmp = "1234567890";
                char * buf = new char[10];
                DJV_ASSERT(buf);
                memcpy(buf, tmp, 10);
                DJV_ASSERT(0 == memcmp(tmp, buf, 10));
                memset(buf, 0, 10);
                DJV_ASSERT(memcmp(tmp, buf, 10) > 0);
                delete[] buf;
            }
            {
                DJV_DEBUG_PRINT("endian = " << Memory::endian());
            }
            {
                DJV_ASSERT(Memory::LSB == Memory::endianOpposite(Memory::MSB));
                DJV_ASSERT(Memory::MSB == Memory::endianOpposite(Memory::LSB));
            }
            {
                std::vector<quint16> tmp(2);
                tmp[0] = 0;
                tmp[1] = -1;
                std::vector<quint16> buf(2);
                memcpy(buf.data(), tmp.data(), sizeof(quint16) * 2);
                Memory::convertEndian(buf.data(), 2, sizeof(quint16));
                const quint8 * a = reinterpret_cast<quint8 *>(tmp.data());
                const quint8 * b = reinterpret_cast<quint8 *>(buf.data());
                DJV_ASSERT(a[0] == b[1]);
                DJV_ASSERT(a[1] == b[0]);
                DJV_ASSERT(a[2] == b[3]);
                DJV_ASSERT(a[3] == b[2]);
                memset(buf.data(), 0, sizeof(quint16) * 2);
                Memory::convertEndian(tmp.data(), buf.data(), 2, sizeof(quint16));
                DJV_ASSERT(a[0] == b[1]);
                DJV_ASSERT(a[1] == b[0]);
                DJV_ASSERT(a[2] == b[3]);
                DJV_ASSERT(a[3] == b[2]);
            }
            {
                std::vector<quint32> tmp(2);
                tmp[0] = 0;
                tmp[1] = -1;
                std::vector<quint32> buf(2);
                memcpy(buf.data(), tmp.data(), sizeof(quint32) * 2);
                Memory::convertEndian(buf.data(), 2, sizeof(quint32));
                const quint8 * a = reinterpret_cast<quint8 *>(tmp.data());
                const quint8 * b = reinterpret_cast<quint8 *>(buf.data());
                DJV_ASSERT(a[0] == b[3]);
                DJV_ASSERT(a[1] == b[2]);
                DJV_ASSERT(a[2] == b[1]);
                DJV_ASSERT(a[3] == b[0]);
                DJV_ASSERT(a[4] == b[7]);
                DJV_ASSERT(a[5] == b[6]);
                DJV_ASSERT(a[6] == b[5]);
                DJV_ASSERT(a[7] == b[4]);
                memset(buf.data(), 0, sizeof(quint32) * 2);
                Memory::convertEndian(tmp.data(), buf.data(), 2, sizeof(quint32));
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
                std::vector<quint64> tmp(2);
                tmp[0] = 0;
                tmp[1] = -1;
                std::vector<quint64> buf(2);
                memcpy(buf.data(), tmp.data(), sizeof(quint64) * 2);
                Memory::convertEndian(buf.data(), 2, sizeof(quint64));
                const quint8 * a = reinterpret_cast<quint8 *>(tmp.data());
                const quint8 * b = reinterpret_cast<quint8 *>(buf.data());
                DJV_ASSERT(a[0] == b[7]);
                DJV_ASSERT(a[1] == b[6]);
                DJV_ASSERT(a[2] == b[5]);
                DJV_ASSERT(a[3] == b[4]);
                DJV_ASSERT(a[4] == b[3]);
                DJV_ASSERT(a[5] == b[2]);
                DJV_ASSERT(a[6] == b[1]);
                DJV_ASSERT(a[7] == b[0]);
                DJV_ASSERT(a[8] == b[15]);
                DJV_ASSERT(a[9] == b[14]);
                DJV_ASSERT(a[10] == b[13]);
                DJV_ASSERT(a[11] == b[12]);
                DJV_ASSERT(a[12] == b[11]);
                DJV_ASSERT(a[13] == b[10]);
                DJV_ASSERT(a[14] == b[9]);
                DJV_ASSERT(a[15] == b[8]);
                memset(buf.data(), 0, sizeof(quint64) * 2);
                Memory::convertEndian(tmp.data(), buf.data(), 2, sizeof(quint64));
                DJV_ASSERT(a[0] == b[7]);
                DJV_ASSERT(a[1] == b[6]);
                DJV_ASSERT(a[2] == b[5]);
                DJV_ASSERT(a[3] == b[4]);
                DJV_ASSERT(a[4] == b[3]);
                DJV_ASSERT(a[5] == b[2]);
                DJV_ASSERT(a[6] == b[1]);
                DJV_ASSERT(a[7] == b[0]);
                DJV_ASSERT(a[8] == b[15]);
                DJV_ASSERT(a[9] == b[14]);
                DJV_ASSERT(a[10] == b[13]);
                DJV_ASSERT(a[11] == b[12]);
                DJV_ASSERT(a[12] == b[11]);
                DJV_ASSERT(a[13] == b[10]);
                DJV_ASSERT(a[14] == b[9]);
                DJV_ASSERT(a[15] == b[8]);
            }
            {
                DJV_DEBUG_PRINT(Memory::sizeLabel(Memory::terabyte));
                DJV_DEBUG_PRINT(Memory::sizeLabel(Memory::gigabyte));
                DJV_DEBUG_PRINT(Memory::sizeLabel(Memory::megabyte));
                DJV_DEBUG_PRINT(Memory::sizeLabel(Memory::kilobyte));
            }
        }

    } // namespace CoreTest
} // namespace djv
