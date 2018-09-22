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

#include <djvGraphicsTest/ColorProfileTest.h>
#include <djvGraphicsTest/ColorTest.h>
#include <djvGraphicsTest/ColorUtilTest.h>
#include <djvGraphicsTest/GraphicsContextTest.h>
#include <djvGraphicsTest/ImageIOFormatsTest.h>
#include <djvGraphicsTest/ImageIOTest.h>
#include <djvGraphicsTest/ImageTagsTest.h>
#include <djvGraphicsTest/ImageTest.h>
#include <djvGraphicsTest/OpenGLImageTest.h>
#include <djvGraphicsTest/OpenGLTest.h>
#include <djvGraphicsTest/PixelDataTest.h>
#include <djvGraphicsTest/PixelDataUtilTest.h>
#include <djvGraphicsTest/PixelTest.h>

#include <djvCoreTest/BoxTest.h>
#include <djvCoreTest/BoxUtilTest.h>
#include <djvCoreTest/CoreContextTest.h>
#include <djvCoreTest/DebugTest.h>
#include <djvCoreTest/ErrorTest.h>
#include <djvCoreTest/FileInfoTest.h>
#include <djvCoreTest/FileInfoUtilTest.h>
#include <djvCoreTest/FileIOTest.h>
#include <djvCoreTest/FileIOUtilTest.h>
#include <djvCoreTest/ListUtilTest.h>
#include <djvCoreTest/MathTest.h>
#include <djvCoreTest/MemoryTest.h>
#include <djvCoreTest/RangeTest.h>
#include <djvCoreTest/SequenceTest.h>
#include <djvCoreTest/SequenceUtilTest.h>
#include <djvCoreTest/SignalBlockerTest.h>
#include <djvCoreTest/SpeedTest.h>
#include <djvCoreTest/StringUtilTest.h>
#include <djvCoreTest/SystemTest.h>
#include <djvCoreTest/TimeTest.h>
#include <djvCoreTest/TimerTest.h>
#include <djvCoreTest/UserTest.h>
#include <djvCoreTest/VectorUtilTest.h>

#include <QApplication>
#include <QVector>

using namespace djv;

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    
    QVector<TestLib::AbstractTest *> tests = QVector<TestLib::AbstractTest *>() <<
        new CoreTest::BoxTest <<
        new CoreTest::BoxUtilTest <<
        new CoreTest::CoreContextTest <<
        new CoreTest::DebugTest <<
        new CoreTest::ErrorTest <<
        new CoreTest::FileInfoTest <<
        new CoreTest::FileInfoUtilTest <<
        new CoreTest::FileIOTest <<
        new CoreTest::FileIOUtilTest <<
        new CoreTest::ListUtilTest <<
        new CoreTest::MathTest <<
        new CoreTest::MemoryTest <<
        new CoreTest::RangeTest <<
        new CoreTest::SequenceTest <<
        new CoreTest::SequenceUtilTest <<
        new CoreTest::SignalBlockerTest <<
        new CoreTest::SpeedTest <<
        new CoreTest::StringUtilTest <<
        new CoreTest::SystemTest <<
        new CoreTest::TimeTest <<
        new CoreTest::TimerTest <<
        new CoreTest::UserTest <<
        new CoreTest::VectorUtilTest <<

        new GraphicsTest::ColorProfileTest <<
        new GraphicsTest::ColorTest <<
        new GraphicsTest::ColorUtilTest <<
        new GraphicsTest::GraphicsContextTest <<
        new GraphicsTest::ImageIOFormatsTest <<
        new GraphicsTest::ImageIOTest <<
        new GraphicsTest::ImageTagsTest <<
        new GraphicsTest::ImageTest <<
        new GraphicsTest::OpenGLImageTest <<
        new GraphicsTest::OpenGLTest <<
        new GraphicsTest::PixelDataTest <<
        new GraphicsTest::PixelDataUtilTest <<
        new GraphicsTest::PixelTest;

    for (int i = 0; i < tests.count(); ++i)
    {
        int     argcCopy = argc;
        char ** argvCopy = 0;
        argvCopy = (char **)malloc(sizeof(char **) * argcCopy);
        for (int j = 0; j < argc; ++j)
        {
            argvCopy[j] = (char *)malloc(sizeof(char) * (strlen(argv[j]) + 1));
            strcpy(argvCopy[j], argv[j]);
        }
        tests[i]->run(argcCopy, argvCopy);
        for (int j = 0; j < argc; ++j)
        {
            free(argvCopy[j]);
        }
        free(argvCopy);
    }
    
    qDeleteAll(tests);    
    return 0;
}

