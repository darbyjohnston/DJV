//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvTest.cpp

#include <djvCoreApplicationTest.h>
#include <djvBoxTest.h>
#include <djvBoxUtilTest.h>
#include <djvColorProfileTest.h>
#include <djvColorTest.h>
#include <djvColorUtilTest.h>
#include <djvDebugTest.h>
#include <djvErrorTest.h>
#include <djvFileInfoTest.h>
#include <djvFileInfoUtilTest.h>
#include <djvFileIoTest.h>
#include <djvFileIoUtilTest.h>
#include <djvImageApplicationTest.h>
#include <djvImageIoTest.h>
#include <djvImageIoPluginTest.h>
#include <djvImageTagsTest.h>
#include <djvImageTest.h>
#include <djvListUtilTest.h>
#include <djvMathTest.h>
#include <djvMatrixTest.h>
#include <djvMatrixUtilTest.h>
#include <djvMemoryBufferTest.h>
#include <djvMemoryTest.h>
#include <djvOpenGlContextTest.h>
#include <djvOpenGlImageTest.h>
#include <djvOpenGlTest.h>
#include <djvPixelDataTest.h>
#include <djvPixelDataUtilTest.h>
#include <djvPixelTest.h>
#include <djvRangeTest.h>
#include <djvSequenceTest.h>
#include <djvSequenceUtilTest.h>
#include <djvSignalBlockerTest.h>
#include <djvSpeedTest.h>
#include <djvStringUtilTest.h>
#include <djvSystemTest.h>
#include <djvTimeTest.h>
#include <djvTimerTest.h>
#include <djvUserTest.h>
#include <djvVectorTest.h>
#include <djvVectorUtilTest.h>

#include <QVector>

int main(int argc, char ** argv)
{
    QVector<djvAbstractTest *> tests = QVector<djvAbstractTest *>() <<
        new djvCoreApplicationTest <<
        new djvBoxTest <<
        new djvBoxUtilTest <<
        new djvColorProfileTest <<
        new djvColorTest <<
        new djvColorUtilTest <<
        new djvDebugTest <<
        new djvErrorTest <<
        new djvFileInfoTest <<
        new djvFileInfoUtilTest <<
        new djvFileIoTest <<
        new djvFileIoUtilTest <<
        new djvImageApplicationTest <<
        new djvImageIoTest <<
        new djvImageIoPluginTest <<
        new djvImageTagsTest <<
        new djvImageTest <<
        new djvListUtilTest <<
        new djvMathTest <<
        new djvMatrixTest <<
        new djvMatrixUtilTest <<
        new djvMemoryBufferTest <<
        new djvMemoryTest <<
        new djvOpenGlContextTest <<
        new djvOpenGlImageTest <<
        new djvOpenGlTest <<
        new djvPixelDataTest <<
        new djvPixelDataUtilTest <<
        new djvPixelTest <<
        new djvRangeTest <<
        new djvSequenceTest <<
        new djvSequenceUtilTest <<
        new djvSignalBlockerTest <<
        new djvSpeedTest <<
        new djvStringUtilTest <<
        new djvSystemTest <<
        new djvTimeTest <<
        new djvTimerTest <<
        new djvUserTest <<
        new djvVectorTest <<
        new djvVectorUtilTest;	
    
    //tests = QVector<djvAbstractTest *>() <<
    //    new djvSequenceTest;

    for (int i = 0; i < tests.count(); ++i)
    {
        tests[i]->run(argc, argv);
    }
    
    qDeleteAll(tests);
    
    return 0;
}

