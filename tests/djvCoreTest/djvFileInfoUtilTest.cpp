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

//! \file djvFileInfoUtilTest.cpp

#include <djvFileInfoUtilTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvFileInfo.h>
#include <djvFileIo.h>
#include <djvFileInfoUtil.h>
#include <djvMemory.h>

#include <QDir>

void djvFileInfoUtilTest::run(int &, char **)
{
    DJV_DEBUG("djvFileInfoUtilTest::run");
    
    split();
    exists();
    list();
    match();
    compress();
    expand();
    filter();
    sort();
    fix();
    recent();
    operators();
}

void djvFileInfoUtilTest::split()
{
    DJV_DEBUG("djvFileInfoUtilTest::split");

    const struct Data
    {
        QString in, path, base, number, extension;
    }
        data [] =
    {
        { "/tmp/image.1-100.tiff", "/tmp/", "image.", "1-100", ".tiff" },
        { "image.1-100.tiff", "", "image.", "1-100", ".tiff" },
        { "image.001-100.tiff", "", "image.", "001-100", ".tiff" },
        { "image1-100.tiff", "", "image", "1-100", ".tiff" },
        { "image001-100.tiff", "", "image", "001-100", ".tiff" },
        { "image_01-100.tiff", "", "image_01-", "100", ".tiff" },
        { "image.-100--1.tiff", "", "image.", "-100--1", ".tiff" },
        { "image-100--1.tiff", "", "image", "-100--1", ".tiff" },
        { "1-100.tiff", "", "", "1-100", ".tiff" },
        { "1--100.tiff", "", "", "1--100", ".tiff" },
        { "-1--100.tiff", "", "", "-1--100", ".tiff" },
        { "-1.tiff", "", "", "-1", ".tiff" },
        { "1-100", "", "", "1-100", "" },
        { "1", "", "", "1", "" },
        { ".hidden", "", ".hidden", "", "" },
        { "", "", "", "", "" },
        { "/", "/", "", "", "" },
        { "//", "//", "", "", "" },
        { "/.", "/", ".", "", "" },
        { "./", "./", "", "", "" },
        { "C:\\Documents and Settings\\darby\\Desktop\\movie.mov",
          "C:\\Documents and Settings\\darby\\Desktop\\", "movie", "", ".mov"}
    };
    
    const int dataCount = sizeof(data) / sizeof(Data);

    for (int i = 0; i < dataCount; ++i)
    {
        const djvFileInfo tmp(data[i].in);
        
        DJV_DEBUG_PRINT(data[i].in << " = " <<
            QString("'%1' '%2' '%3' '%4'").
            arg(tmp.path()).
            arg(tmp.base()).
            arg(tmp.number()).
            arg(tmp.extension()));
        
        DJV_ASSERT(
            tmp.path()      == data[i].path      &&
            tmp.base()      == data[i].base      &&
            tmp.number()    == data[i].number    &&
            tmp.extension() == data[i].extension);
    }
}

void djvFileInfoUtilTest::exists()
{
    DJV_DEBUG("djvFileInfoUtilTest::exists");
    
    {
        const QString fileName("djvFileInfoUtilTest.test");

        djvFileIo io;
        io.open(fileName, djvFileIo::WRITE);
        io.close();

        DJV_ASSERT(djvFileInfoUtil::exists(djvFileInfo(fileName)));

        DJV_ASSERT(! djvFileInfoUtil::exists(djvFileInfo(fileName + ".test")));
    }
    
    {
        const QString fileName("djvFileInfoUtilTest.%1.test");
        
        {
            djvFileIo io;
            io.open(fileName.arg(1), djvFileIo::WRITE);
            io.close();
            io.open(fileName.arg(2), djvFileIo::WRITE);
            io.close();
            io.open(fileName.arg(3), djvFileIo::WRITE);
            io.close();
        }
        
        djvFileInfo fileInfo(fileName.arg("1-3"));
        fileInfo.setType(djvFileInfo::SEQUENCE);
        
        DJV_ASSERT(djvFileInfoUtil::exists(fileInfo));
    }
}

void djvFileInfoUtilTest::list()
{
    DJV_DEBUG("djvFileInfoUtilTest::list");
    
    const QString fileName("djvFileInfoUtilTest.%1.test");
    
    {
        djvFileIo io;
        io.open(fileName.arg(1), djvFileIo::WRITE);
        io.close();
        io.open(fileName.arg(3), djvFileIo::WRITE);
        io.close();
    }

    djvFileInfoList list = djvFileInfoUtil::list(".", djvSequence::COMPRESS_OFF);
    
    DJV_ASSERT(list.indexOf(djvFileInfo(fileName.arg(1))));

    list = djvFileInfoUtil::list(".", djvSequence::COMPRESS_SPARSE);
    
    DJV_ASSERT(list.indexOf(djvFileInfo(fileName.arg("1,3"))));

    list = djvFileInfoUtil::list(".", djvSequence::COMPRESS_RANGE);
    
    DJV_ASSERT(list.indexOf(djvFileInfo(fileName.arg("1-3"))));
}

void djvFileInfoUtilTest::match()
{
    DJV_DEBUG("djvFileInfoUtilTest::match");
    
    const QString fileName("image.%1.dpx");
    
    const djvFileInfoList list = djvFileInfoList() <<
        djvFileInfo(fileName.arg("1")) <<
        djvFileInfo(fileName.arg("2")) <<
        djvFileInfo(fileName.arg("3"));
        
    DJV_ASSERT(list[0] == djvFileInfoUtil::sequenceWildcardMatch(list[2], list));
    
    const djvFileInfo tmp("image.1.cin");
    
    DJV_ASSERT(tmp == djvFileInfoUtil::sequenceWildcardMatch(tmp, list));
}

void djvFileInfoUtilTest::compress()
{
    DJV_DEBUG("djvFileInfoUtilTest::compress");
    
    const QString fileName("image.%1.dpx");
    
    const djvFileInfoList list = djvFileInfoList() <<
        djvFileInfo(fileName.arg(1)) <<
        djvFileInfo(fileName.arg(3)) <<
        djvFileInfo(fileName.arg(4));
    
    djvFileInfoList tmp = list;
    
    djvFileInfoUtil::compressSequence(tmp, djvSequence::COMPRESS_OFF);
    
    DJV_ASSERT(tmp[0] == list[0]);
    
    tmp = list;
    
    djvFileInfoUtil::compressSequence(tmp, djvSequence::COMPRESS_SPARSE);
        
    DJV_ASSERT(tmp[0].number() == "1,3-4");
    
    tmp = list;
    
    djvFileInfoUtil::compressSequence(tmp, djvSequence::COMPRESS_RANGE);
    
    DJV_ASSERT(tmp[0].number() == "1-4");
}
    
void djvFileInfoUtilTest::expand()
{
    DJV_DEBUG("djvFileInfoUtilTest::expand");
    
    const QString fileName("image.%1.dpx");
    
    djvFileInfo fileInfo(fileName.arg("1,3"));
    
    QStringList list = djvFileInfoUtil::expandSequence(fileInfo);
    
    DJV_ASSERT(fileInfo.fileName() == list[0]);
    
    fileInfo.setType(djvFileInfo::SEQUENCE);
    
    list = djvFileInfoUtil::expandSequence(fileInfo);
    
    DJV_ASSERT(fileName.arg(1) == list[0]);
    DJV_ASSERT(fileName.arg(3) == list[1]);
}

    
void djvFileInfoUtilTest::filter()
{
    DJV_DEBUG("djvFileInfoUtilTest::filter");
    
    const QString fileName("image.%1.dpx");
    
    const djvFileInfoList list = djvFileInfoList() <<
        djvFileInfo(fileName.arg(1)) <<
        djvFileInfo(fileName.arg(3)) <<
        djvFileInfo(fileName.arg(4));
    
    djvFileInfoList tmp = list;
    
    djvFileInfoUtil::filter(tmp, djvFileInfoUtil::FILTER_NONE);
    
    DJV_ASSERT(tmp == list);
    
    tmp = list;
    
    djvFileInfoUtil::filter(tmp, djvFileInfoUtil::FILTER_FILES);
    
    DJV_ASSERT(! tmp.count());
    
    tmp = list;
    tmp[0].setType(djvFileInfo::DIRECTORY);
    
    djvFileInfoUtil::filter(tmp, djvFileInfoUtil::FILTER_DIRECTORIES);
    
    DJV_ASSERT(list[1] == tmp[0]);
    
    tmp = list;
    tmp += djvFileInfo(".hidden");
    
    djvFileInfoUtil::filter(tmp, djvFileInfoUtil::FILTER_HIDDEN);
    
    DJV_ASSERT(tmp.count() == list.count());
    
    tmp = list;
    
    djvFileInfoUtil::filter(tmp, 0, "1");
    
    DJV_ASSERT(list[0] == tmp[0]);
    
    tmp = list;
    
    djvFileInfoUtil::filter(tmp, 0, QString(), QStringList() << "*1*" << "*3*");
    
    DJV_ASSERT(list[0] == tmp[0]);
    DJV_ASSERT(list[1] == tmp[1]);
}
    
void djvFileInfoUtilTest::sort()
{
    DJV_DEBUG("djvFileInfoUtilTest::sort");
    
    const QString fileName("image.%1.dpx");
    
    const djvFileInfoList list = djvFileInfoList() <<
        djvFileInfo(fileName.arg(4)) <<
        djvFileInfo(fileName.arg(3)) <<
        djvFileInfo(fileName.arg(1));

    djvFileInfoList tmp = list;
    
    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_NAME);
    
    DJV_ASSERT(tmp[0] == list[2]);

    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_NAME, true);
    
    DJV_ASSERT(tmp[2] == list[2]);

    tmp = list;
    tmp[0].setType(djvFileInfo::DIRECTORY);
    
    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_TYPE);
    
    DJV_ASSERT(tmp[2].fileName() == list[0].fileName());

    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_TYPE, true);
    
    DJV_ASSERT(tmp[0].fileName() == list[0].fileName());

    tmp = list;
    tmp[0].setSize(1);
    
    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_SIZE);
    
    DJV_ASSERT(tmp[2].fileName() == list[0].fileName());

    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_SIZE, true);
    
    DJV_ASSERT(tmp[0].fileName() == list[0].fileName());

    tmp = list;
    tmp[0].setUser(1);
    
    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_USER);
    
    DJV_ASSERT(tmp[2].fileName() == list[0].fileName());

    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_USER, true);
    
    DJV_ASSERT(tmp[0].fileName()== list[0].fileName());

    tmp = list;
    tmp[0].setPermissions(1);
    
    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_PERMISSIONS);
    
    DJV_ASSERT(tmp[2].fileName() == list[0].fileName());

    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_PERMISSIONS, true);
    
    DJV_ASSERT(tmp[0].fileName() == list[0].fileName());

    tmp = list;
    tmp[0].setTime(1);
    
    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_TIME);
    
    DJV_ASSERT(tmp[2].fileName() == list[0].fileName());

    djvFileInfoUtil::sort(tmp, djvFileInfoUtil::SORT_TIME, true);
    
    DJV_ASSERT(tmp[0].fileName() == list[0].fileName());

    tmp = list;
    tmp[2].setType(djvFileInfo::DIRECTORY);
    
    djvFileInfoUtil::sortDirsFirst(tmp);
    
    DJV_ASSERT(tmp[0].fileName() == list[2].fileName());
}

void djvFileInfoUtilTest::recent()
{
    DJV_DEBUG("djvFileInfoUtilTest::recent");
    
    const QString fileName("image.%1.dpx");
    
    const djvFileInfoList list = djvFileInfoList() <<
        fileName.arg(4) <<
        fileName.arg(3) <<
        fileName.arg(1);

    djvFileInfoList tmp;

    for (int i = 0; i < list.count(); ++i)
    {
        djvFileInfoUtil::recent(list[i], tmp, list.count() - 1);
    }

    DJV_ASSERT(tmp.count() == list.count() - 1);
    DJV_ASSERT(tmp[0] == list[2]);

    djvFileInfoUtil::recent(list[2], tmp, list.count() - 1);
    
    DJV_ASSERT(tmp.count() == list.count() - 1);
    DJV_ASSERT(tmp[0] == list[2]);
}

void djvFileInfoUtilTest::fix()
{
    DJV_DEBUG("djvFileInfoUtilTest::fix");

    const struct Data
    {
        QString path;
        QString fixed;
    }
        data [] =
    {
        { "/", QDir::rootPath() }
    };
    
    const int dataCount = sizeof(data) / sizeof(Data);

    for (int i = 0; i < dataCount; ++i)
    {
        const QString fixed = djvFileInfoUtil::fixPath(data[i].path);
        
        DJV_DEBUG_PRINT(data[i].path << " = " << fixed);
        
        DJV_ASSERT(fixed == data[i].fixed);
    }
}

void djvFileInfoUtilTest::operators()
{
    DJV_DEBUG("djvFileInfoUtilTest::operators");

    {
        DJV_DEBUG_PRINT(djvFileInfoUtil::SORT_NAME);
    }
}


