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

#include <djvAVTest/AudioDataTest.h>
#include <djvAVTest/AudioTest.h>
#include <djvAVTest/AVContextTest.h>
#include <djvAVTest/ColorProfileTest.h>
#include <djvAVTest/ColorTest.h>
#include <djvAVTest/ColorUtilTest.h>
#include <djvAVTest/ImageIOFormatsTest.h>
#include <djvAVTest/ImageIOTest.h>
#include <djvAVTest/ImageTest.h>
#include <djvAVTest/OpenGLImageTest.h>
#include <djvAVTest/OpenGLTest.h>
#include <djvAVTest/PixelDataTest.h>
#include <djvAVTest/PixelDataUtilTest.h>
#include <djvAVTest/PixelTest.h>
#include <djvAVTest/TagsTest.h>

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
#include <djvCoreTest/SignalBlockerTest.h>
#include <djvCoreTest/SpeedTest.h>
#include <djvCoreTest/StringUtilTest.h>
#include <djvCoreTest/SystemTest.h>
#include <djvCoreTest/TimeTest.h>
#include <djvCoreTest/TimerTest.h>
#include <djvCoreTest/UserTest.h>
#include <djvCoreTest/VectorUtilTest.h>

#include <djvCore/CoreContext.h>

#include <QApplication>
#include <QVector>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        Core::CoreContext::initLibPaths(argc, argv);
        QApplication app(argc, argv);
        
        QVector<TestLib::AbstractTest *> tests = QVector<TestLib::AbstractTest *>() <<
            /*new CoreTest::BoxTest <<
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
            new CoreTest::SignalBlockerTest <<
            new CoreTest::SpeedTest <<
            new CoreTest::StringUtilTest <<
            new CoreTest::SystemTest <<
            new CoreTest::TimeTest <<
            new CoreTest::TimerTest <<
            new CoreTest::UserTest <<
            new CoreTest::VectorUtilTest <<*/

            new AVTest::AudioDataTest <<
            new AVTest::AudioTest/* <<
            new AVTest::AVContextTest <<
            new AVTest::ColorProfileTest <<
            new AVTest::ColorTest <<
            new AVTest::ColorUtilTest <<
            new AVTest::ImageIOFormatsTest <<
            new AVTest::ImageIOTest <<
            new AVTest::ImageTest <<
            new AVTest::OpenGLImageTest <<
            new AVTest::OpenGLTest <<
            new AVTest::PixelDataTest <<
            new AVTest::PixelDataUtilTest <<
            new AVTest::PixelTest <<
            new AVTest::TagsTest*/;

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
        
        r = 0;
    }
    catch (const Core::Error & error)
    {
        Q_FOREACH(const Core::Error::Message & message, error.messages())
        {
            std::cout << "ERROR " <<
                message.prefix.toUtf8().data() << ": " <<
                message.string.toUtf8().data() << std::endl;
        }
    }
    catch (const std::exception & error)
    {
        std::cout << "ERROR: " << error.what() << std::endl;
    }
    return r;
}

