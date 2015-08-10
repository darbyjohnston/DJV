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

//! \file djvFileBrowserTestApplication.cpp

#include <djvFileBrowserTestApplication.h>

#include <djvFileBrowserTestDir.h>
#include <djvFileBrowserTestInfo.h>
#include <djvFileBrowserTestThumbnail.h>
#include <djvFileBrowserTestUtil.h>
#include <djvFileBrowserTestWidget.h>

#include <djvError.h>

#include <QTimer>

//------------------------------------------------------------------------------
// djvFileBrowserTestApplication
//------------------------------------------------------------------------------

djvFileBrowserTestApplication::djvFileBrowserTestApplication(int & argc, char ** argv) :
    QApplication(argc, argv)
{
#   if QT_VERSION < 0x050000
    setStyle(new QPlastiqueStyle);
#   else
    setStyle("fusion");
#   endif

    qRegisterMetaType<djvFileBrowserTestDirRequest>(
        "djvFileBrowserTestDirRequest");
    qRegisterMetaType<djvFileBrowserTestDirResult>(
        "djvFileBrowserTestDirResult");
    qRegisterMetaType<djvFileBrowserTestInfoRequest>(
        "djvFileBrowserTestInfoRequest");
    qRegisterMetaType<djvFileBrowserTestInfoResult>(
        "djvFileBrowserTestInfoResult");
    qRegisterMetaType<djvFileBrowserTestThumbnailRequest>(
        "djvFileBrowserTestThumbnailRequest");
    qRegisterMetaType<djvFileBrowserTestThumbnailResult>(
        "djvFileBrowserTestThumbnailResult");
    qRegisterMetaType<djvFileBrowserTestUtil::THUMBNAILS>(
        "djvFileBrowserTestUtil::THUMBNAILS");

    _context.reset(new djvGuiContext);
    
    if (argc != 2)
    {
        _context->printMessage("Usage: djvFileBrowserTest (path)");
        
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
        _path = argv[1];
        
        QTimer::singleShot(0, this, SLOT(work()));
    }
}

void djvFileBrowserTestApplication::commandLineExit()
{
    exit(1);
}

void djvFileBrowserTestApplication::work()
{
    _widget.reset(new djvFileBrowserTestWidget(_context.data()));
    _widget->setPath(_path);
    _widget->show();
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
    return djvFileBrowserTestApplication(argc, argv).exec();
}

