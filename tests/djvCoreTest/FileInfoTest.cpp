//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvCoreTest/FileInfoTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void FileInfoTest::run(int &, char **)
        {
            DJV_DEBUG("FileInfoTest::run");
            ctors();
            members();
            operators();
        }

        void FileInfoTest::ctors()
        {
            DJV_DEBUG("FileInfoTest::ctors");
            {
                FileInfo fileInfo;
                DJV_ASSERT(fileInfo.isEmpty());
            }
            {
                const QString fileName("FileInfoTest.test");
                FileInfo fileInfo(fileName);
                DJV_ASSERT(fileName == fileInfo.fileName());
            }
        }

        void FileInfoTest::members()
        {
            DJV_DEBUG("FileInfoTest::members");
            FileInfo::sequenceExtensions.insert(".ppm");
            {
                const QString fileName("image.0001-0100.ppm");
                FileInfo fileInfo(fileName);
                DJV_ASSERT(fileName == fileInfo.fileName());
                DJV_ASSERT(fileName == fileInfo.fileName(1));

                fileInfo.setType(FileInfo::SEQUENCE);
                DJV_ASSERT(fileName == fileInfo.fileName());
                DJV_ASSERT("image.0001.ppm" == fileInfo.fileName(1));
            }
            {
                const QString fileName("image.1.ppm");
                FileInfo fileInfo(fileName);
                fileInfo.setPath("/");
                DJV_ASSERT("/image.1.ppm" == fileInfo.fileName());
                fileInfo.setBase("movie");

                DJV_ASSERT("/movie1.ppm" == fileInfo.fileName());
                fileInfo.setNumber("100");

                DJV_ASSERT("/movie100.ppm" == fileInfo.fileName());
                fileInfo.setExtension(".pic");

                DJV_ASSERT("/movie100.pic" == fileInfo.fileName());
            }
            {
                const QString fileName("FileInfoTest.test");
                {
                    FileIO io;
                    io.open(fileName, FileIO::WRITE);
                    io.close();
                }
                DJV_ASSERT(FileInfo(fileName).exists());
            }
            {
                FileInfo fileInfo;
                fileInfo.setType(FileInfo::SEQUENCE);
                fileInfo.setSize(1);
                fileInfo.setUser(2);
                fileInfo.setPermissions(3);
                fileInfo.setTime(4);
                DJV_ASSERT(!fileInfo.isDotFile());
                DJV_ASSERT(FileInfo::SEQUENCE == fileInfo.type());
                DJV_ASSERT(1 == fileInfo.size());
                DJV_ASSERT(2 == fileInfo.user());
                DJV_ASSERT(3 == fileInfo.permissions());
                DJV_ASSERT(4 == fileInfo.time());
            }
            {
                FileInfo fileInfo("image.ppm");
                const Sequence sequence(1, 3);
                fileInfo.setSequence(sequence);
                DJV_ASSERT(sequence == fileInfo.sequence());
                DJV_ASSERT(fileInfo.isSequenceValid());
                DJV_ASSERT("image1-3.ppm" == fileInfo.fileName());
                fileInfo.setSequence(Sequence(FrameList() << 3 << 2 << 1));
                fileInfo.sortSequence();
                DJV_ASSERT(sequence == fileInfo.sequence());
                DJV_ASSERT(fileInfo.isSequenceValid());
                DJV_ASSERT("image1-3.ppm" == fileInfo.fileName());
            }
            {
                FileInfo fileInfo("image.ppm");
                DJV_ASSERT(!fileInfo.addSequence(FileInfo("image1.ppm")));
            }
            {
                FileInfo fileInfo("image.1.ppm");
                DJV_ASSERT(!fileInfo.addSequence(FileInfo("image.pic")));
                DJV_ASSERT(!fileInfo.addSequence(FileInfo("movie.ppm")));
                DJV_ASSERT(!fileInfo.addSequence(FileInfo("image.ppm")));
                DJV_ASSERT(!fileInfo.addSequence(FileInfo("image..ppm")));
                DJV_ASSERT(fileInfo.addSequence(FileInfo("image.2.ppm")));
                DJV_ASSERT(Sequence(1, 2) == fileInfo.sequence());
                DJV_ASSERT(fileInfo.addSequence(FileInfo("image.0003.ppm")));
                DJV_ASSERT(Sequence(1, 3, 4) == fileInfo.sequence());
                FileInfo tmp("image.0004.ppm");
                tmp.setSize(1);
                tmp.setUser(2);
                tmp.setTime(3);
                fileInfo.addSequence(tmp);
                DJV_ASSERT(1 == fileInfo.size());
                DJV_ASSERT(2 == fileInfo.user());
                DJV_ASSERT(3 == fileInfo.time());
            }
            {
                DJV_DEBUG_PRINT("permissions = " << FileInfo::permissionsLabels());
                DJV_DEBUG_PRINT("rwx = " << FileInfo::permissionsLabel(
                    FileInfo::READ |
                    FileInfo::WRITE |
                    FileInfo::EXEC));
            }
            {
                const QString fileName("image.1.ppm");
                FileInfo fileInfo(fileName);
                DJV_ASSERT(fileName == (QString)fileInfo);
            }
        }

        void FileInfoTest::operators()
        {
            DJV_DEBUG("FileInfoTest::operators");
            {
                const FileInfo::TYPE a = FileInfo::DIRECTORY;
                QStringList tmp;
                tmp << a;
                FileInfo::TYPE b = static_cast<FileInfo::TYPE>(0);
                tmp >> b;
                DJV_ASSERT(a == b);
            }
            {
                DJV_DEBUG_PRINT("type = " << FileInfo::DIRECTORY);
                DJV_DEBUG_PRINT(FileInfo());
            }
        }

    } // namespace CoreTest
} // namespace djv
