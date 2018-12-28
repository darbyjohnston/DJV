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

#include <djvCoreTest/FileInfoUtilTest.h>

#include <djvAV/AVContext.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileIO.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/Memory.h>

#include <QDir>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void FileInfoUtilTest::run(int & argc, char ** argv)
        {
            DJV_DEBUG("FileInfoUtilTest::run");
            
            //! \todo Some of the tests rely on FileInfo::sequenceExtensions being
            //! initialized by the image I/O plugins.
            AV::AVContext context(argc, argv);

            split();
            exists();
            list();
            match();
            sequence();
            expand();
            filter();
            sort();
            fix();
            recent();
            operators();
        }

        void FileInfoUtilTest::split()
        {
            DJV_DEBUG("FileInfoUtilTest::split");

            //! \todo Add tests for negative frame numbers.
            const struct Data
            {
                QString in, path, base, number, extension;
            }
            data[] =
            {
                { "/tmp/image.1-100.ppm", "/tmp/", "image.", "1-100", ".ppm" },
                { "image.1-100.ppm", "", "image.", "1-100", ".ppm" },
                { "image.001-100.ppm", "", "image.", "001-100", ".ppm" },
                { "image1-100.ppm", "", "image", "1-100", ".ppm" },
                { "image001-100.ppm", "", "image", "001-100", ".ppm" },
                { "image-001-100.ppm", "", "image-", "001-100", ".ppm" },
                { "image_01-100.ppm", "", "image_", "01-100", ".ppm" },
                { "image1_001-100.ppm", "", "image1_", "001-100", ".ppm" },
                //{ "image.-100--1.ppm", "", "image.", "-100--1", ".ppm" },
                //{ "image-100--1.ppm", "", "image", "-100--1", ".ppm" },
                { "1-100.ppm", "", "", "1-100", ".ppm" },
                //{ "1--100.ppm", "", "", "1--100", ".ppm" },
                //{ "-1--100.ppm", "", "", "-1--100", ".ppm" },
                //{ "-1.ppm", "", "", "-1", ".ppm" },
                { "1-100", "", "", "1-100", "" },
                { "1", "", "", "1", "" },
                { ".hidden", "", ".hidden", "", "" },
                { "", "", "", "", "" },
                { "/", "/", "", "", "" },
                { "//", "//", "", "", "" },
                { "/.", "/", ".", "", "" },
                { "./", "./", "", "", "" },
                { "C:\\Documents and Settings\\darby\\Desktop\\movie.mov",
                  "C:\\Documents and Settings\\darby\\Desktop\\", "movie", "", ".mov"},
                { "C:\\Documents and Settings\\darby\\Desktop\\movie-.mov",
                  "C:\\Documents and Settings\\darby\\Desktop\\", "movie-", "", ".mov"}
            };
            const int dataCount = sizeof(data) / sizeof(Data);
            for (int i = 0; i < dataCount; ++i)
            {
                const FileInfo tmp(data[i].in);
                DJV_DEBUG_PRINT(data[i].in << " = " <<
                    QString("'%1' '%2' '%3' '%4'").
                    arg(tmp.path()).
                    arg(tmp.base()).
                    arg(tmp.number()).
                    arg(tmp.extension()));
                DJV_ASSERT(
                    tmp.path() == data[i].path      &&
                    tmp.base() == data[i].base      &&
                    tmp.number() == data[i].number    &&
                    tmp.extension() == data[i].extension);
            }
        }

        void FileInfoUtilTest::exists()
        {
            DJV_DEBUG("FileInfoUtilTest::exists");
            {
                const QString fileName("FileInfoUtilTest.test");
                FileIO io;
                io.open(fileName, FileIO::WRITE);
                io.close();
                DJV_ASSERT(FileInfoUtil::exists(FileInfo(fileName)));
                DJV_ASSERT(!FileInfoUtil::exists(FileInfo(fileName + ".ppm")));
            }
            {
                const QString fileName("FileInfoUtilTest.%1.ppm");
                {
                    FileIO io;
                    io.open(fileName.arg(1), FileIO::WRITE);
                    io.close();
                    io.open(fileName.arg(2), FileIO::WRITE);
                    io.close();
                    io.open(fileName.arg(3), FileIO::WRITE);
                    io.close();
                }
                FileInfo fileInfo(fileName.arg("1-3"));
                fileInfo.setType(FileInfo::SEQUENCE);
                DJV_ASSERT(FileInfoUtil::exists(fileInfo));
            }
        }

        void FileInfoUtilTest::list()
        {
            DJV_DEBUG("FileInfoUtilTest::list");
            const QString fileName("FileInfoUtilTest.%1.test");
            {
                FileIO io;
                io.open(fileName.arg(1), FileIO::WRITE);
                io.close();
                io.open(fileName.arg(3), FileIO::WRITE);
                io.close();
            }
            FileInfoList list = FileInfoUtil::list(".", Sequence::FORMAT_OFF);
            DJV_ASSERT(list.indexOf(FileInfo(fileName.arg(1))));
            list = FileInfoUtil::list(".", Sequence::FORMAT_SPARSE);
            DJV_ASSERT(list.indexOf(FileInfo(fileName.arg("1,3"))));
            list = FileInfoUtil::list(".", Sequence::FORMAT_RANGE);
            DJV_ASSERT(list.indexOf(FileInfo(fileName.arg("1-3"))));
        }

        void FileInfoUtilTest::match()
        {
            DJV_DEBUG("FileInfoUtilTest::match");
            const QString fileName("image.%1.ppm");
            const FileInfoList list = FileInfoList() <<
                FileInfo(fileName.arg("1")) <<
                FileInfo(fileName.arg("2")) <<
                FileInfo(fileName.arg("3"));
            DJV_ASSERT(list[0] == FileInfoUtil::sequenceWildcardMatch(list[2], list));
            const FileInfo tmp("image.1.pic");
            DJV_ASSERT(tmp == FileInfoUtil::sequenceWildcardMatch(tmp, list));
        }

        void FileInfoUtilTest::sequence()
        {
            DJV_DEBUG("FileInfoUtilTest::sequence");
            const QString fileName("image.%1.ppm");
            const FileInfoList list = FileInfoList() <<
                FileInfo(fileName.arg(1)) <<
                FileInfo(fileName.arg(3)) <<
                FileInfo(fileName.arg(4));
            FileInfoList tmp = list;
            FileInfoUtil::sequence(tmp, Sequence::FORMAT_OFF);
            DJV_ASSERT(tmp[0] == list[0]);
            tmp = list;
            FileInfoUtil::sequence(tmp, Sequence::FORMAT_SPARSE);
            DJV_ASSERT(tmp[0].number() == "1,3-4");
            tmp = list;
            FileInfoUtil::sequence(tmp, Sequence::FORMAT_RANGE);
            DJV_ASSERT(tmp[0].number() == "1-4");
        }

        void FileInfoUtilTest::expand()
        {
            DJV_DEBUG("FileInfoUtilTest::expand");
            const QString fileName("image.%1.ppm");
            FileInfo fileInfo(fileName.arg("1,3"));
            QStringList list = FileInfoUtil::expandSequence(fileInfo);
            DJV_ASSERT(fileInfo.fileName() == list[0]);
            fileInfo.setType(FileInfo::SEQUENCE);
            list = FileInfoUtil::expandSequence(fileInfo);
            DJV_ASSERT(fileName.arg(1) == list[0]);
            DJV_ASSERT(fileName.arg(3) == list[1]);
        }

        void FileInfoUtilTest::filter()
        {
            DJV_DEBUG("FileInfoUtilTest::filter");
            const QString fileName("image.%1.ppm");
            const FileInfoList list = FileInfoList() <<
                FileInfo(fileName.arg(1)) <<
                FileInfo(fileName.arg(3)) <<
                FileInfo(fileName.arg(4));
            FileInfoList tmp = list;
            FileInfoUtil::filter(tmp, FileInfoUtil::FILTER_NONE);
            DJV_ASSERT(tmp == list);
            tmp = list;
            FileInfoUtil::filter(tmp, FileInfoUtil::FILTER_FILES);
            DJV_ASSERT(!tmp.count());
            tmp = list;
            tmp[0].setType(FileInfo::DIRECTORY);
            FileInfoUtil::filter(tmp, FileInfoUtil::FILTER_DIRECTORIES);
            DJV_ASSERT(list[1] == tmp[0]);
            tmp = list;
            tmp += FileInfo(".hidden");
            FileInfoUtil::filter(tmp, FileInfoUtil::FILTER_HIDDEN);
            DJV_ASSERT(tmp.count() == list.count());
            tmp = list;
            FileInfoUtil::filter(tmp, 0, "1");
            DJV_ASSERT(list[0] == tmp[0]);
            tmp = list;
            FileInfoUtil::filter(tmp, 0, QString(), QStringList() << "*1*" << "*3*");
            DJV_ASSERT(list[0] == tmp[0]);
            DJV_ASSERT(list[1] == tmp[1]);
        }

        void FileInfoUtilTest::sort()
        {
            DJV_DEBUG("FileInfoUtilTest::sort");
            const QString fileName("image.%1.ppm");
            const FileInfoList list = FileInfoList() <<
                FileInfo(fileName.arg(4)) <<
                FileInfo(fileName.arg(3)) <<
                FileInfo(fileName.arg(1));

            FileInfoList tmp = list;
            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_NAME);
            DJV_ASSERT(tmp[0] == list[2]);

            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_NAME, true);
            DJV_ASSERT(tmp[2] == list[2]);

            tmp = list;
            tmp[0].setType(FileInfo::DIRECTORY);
            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_TYPE);
            DJV_ASSERT(tmp[2].fileName() == list[0].fileName());

            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_TYPE, true);
            DJV_ASSERT(tmp[0].fileName() == list[0].fileName());

            tmp = list;
            tmp[0].setSize(1);
            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_SIZE);
            DJV_ASSERT(tmp[2].fileName() == list[0].fileName());

            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_SIZE, true);
            DJV_ASSERT(tmp[0].fileName() == list[0].fileName());

            tmp = list;
            tmp[0].setUser(1);
            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_USER);
            DJV_ASSERT(tmp[2].fileName() == list[0].fileName());

            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_USER, true);
            DJV_ASSERT(tmp[0].fileName() == list[0].fileName());

            tmp = list;
            tmp[0].setPermissions(1);
            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_PERMISSIONS);
            DJV_ASSERT(tmp[2].fileName() == list[0].fileName());

            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_PERMISSIONS, true);
            DJV_ASSERT(tmp[0].fileName() == list[0].fileName());

            tmp = list;
            tmp[0].setTime(1);
            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_TIME);
            DJV_ASSERT(tmp[2].fileName() == list[0].fileName());

            FileInfoUtil::sort(tmp, FileInfoUtil::SORT_TIME, true);
            DJV_ASSERT(tmp[0].fileName() == list[0].fileName());

            tmp = list;
            tmp[2].setType(FileInfo::DIRECTORY);
            FileInfoUtil::sortDirsFirst(tmp);
            DJV_ASSERT(tmp[0].fileName() == list[2].fileName());
        }

        void FileInfoUtilTest::recent()
        {
            DJV_DEBUG("FileInfoUtilTest::recent");
            const QString fileName("image.%1.ppm");
            const FileInfoList list = FileInfoList() <<
                fileName.arg(4) <<
                fileName.arg(3) <<
                fileName.arg(1);
            FileInfoList tmp;
            for (int i = 0; i < list.count(); ++i)
            {
                FileInfoUtil::recent(list[i], tmp, list.count() - 1);
            }
            DJV_ASSERT(tmp.count() == list.count() - 1);
            DJV_ASSERT(tmp[0] == list[2]);
            FileInfoUtil::recent(list[2], tmp, list.count() - 1);
            DJV_ASSERT(tmp.count() == list.count() - 1);
            DJV_ASSERT(tmp[0] == list[2]);
        }

        void FileInfoUtilTest::fix()
        {
            DJV_DEBUG("FileInfoUtilTest::fix");
            const struct Data
            {
                QString path;
                QString fixed;
            }
            data[] =
            {
                { "/", QDir::rootPath() }
            };
            const int dataCount = sizeof(data) / sizeof(Data);
            for (int i = 0; i < dataCount; ++i)
            {
                const QString fixed = FileInfoUtil::fixPath(data[i].path);
                DJV_DEBUG_PRINT(data[i].path << " = " << fixed);
                DJV_ASSERT(fixed == data[i].fixed);
            }
        }

        void FileInfoUtilTest::operators()
        {
            DJV_DEBUG("FileInfoUtilTest::operators");
            {
                DJV_DEBUG_PRINT(FileInfoUtil::SORT_NAME);
            }
        }

    } // namespace CoreTest
} // namespace djv
