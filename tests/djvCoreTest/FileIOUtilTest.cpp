//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvCoreTest/FileIOUtilTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/FileIO.h>
#include <djvCore/FileIOUtil.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void FileIOUtilTest::run(int &, char **)
        {
            DJV_DEBUG("FileIOUtilTest::run");
            word();
            lines();
        }

        namespace
        {
            const QStringList data = QStringList() <<
                "a" << "bb" << "ccc";

        } // namespace

        void FileIOUtilTest::word()
        {
            DJV_DEBUG("FileIOUtilTest::word");
            const QString fileName = "FileIOUtilTest.test";
            FileIO io;
            io.open(fileName, FileIO::WRITE);
            const QString s = QString("# comment\n") + data.join(" ");
            const QByteArray b = s.toLatin1();
            io.set(b.data(), b.count());
            io.setU8(0);
            io.close();

            io.open(fileName, FileIO::READ);

            char buf[StringUtil::cStringLength];
            int i = 0;
            while (io.isValid())
            {
                FileIOUtil::word(io, buf);
                DJV_DEBUG_PRINT("word = " << buf);
                DJV_ASSERT(data[i++] == QString(buf));
            }
            DJV_ASSERT(data.count() == i);
        }

        void FileIOUtilTest::lines()
        {
            DJV_DEBUG("FileIOUtilTest::lines");
            const QString fileName = "FileIOUtilTest.test";
            FileIO io;
            io.open(fileName, FileIO::WRITE);
            const QString s = data.join("\n");
            const QByteArray b = s.toLatin1();
            io.set(b.data(), b.count());
            io.setU8(0);
            io.close();

            io.open(fileName, FileIO::READ);
            char buf[StringUtil::cStringLength];
            int i = 0;
            while (io.isValid())
            {
                FileIOUtil::line(io, buf);
                DJV_DEBUG_PRINT("line = " << buf);
                DJV_ASSERT(data[i++] == QString(buf));
            }
            DJV_ASSERT(data.count() == i);

            const QStringList list = FileIOUtil::lines(fileName);
            DJV_ASSERT(data.count() == list.count());
            for (i = 0; i < data.count(); ++i)
            {
                DJV_ASSERT(data[i] == list[i]);
            }
        }

    } // namespace CoreTest
} // namespace djv
