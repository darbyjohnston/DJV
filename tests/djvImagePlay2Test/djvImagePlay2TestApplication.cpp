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

//! \file djvImagePlay2TestApplication.cpp

#include <djvImagePlay2TestApplication.h>

#include <djvImagePlay2TestLoad.h>
#include <djvImagePlay2TestPlayback.h>

#include <QTimer>

//------------------------------------------------------------------------------
// djvImagePlay2TestApplication
//------------------------------------------------------------------------------

djvImagePlay2TestApplication::djvImagePlay2TestApplication(int & argc, char ** argv) :
    QApplication(argc, argv),
    _context(new djvImagePlay2TestContext)
{
    //DJV_DEBUG("djvImagePlay2TestApplication");
    
    if (argc != 2)
    {
        _context->printMessage("Usage: djvImagePlay2Test (input)");
        
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
#       if QT_VERSION < 0x050000
        setStyle(new QPlastiqueStyle);
#       else
        setStyle("fusion");
#       endif
        
        _window.reset(new djvImagePlay2TestWindow(_context.data()));
        _window->setWindowTitle("djvImagePlay2Test");
        _window->show();
        
        _context->glContext()->setShareContext(_window->glContext());
        _context->glContext()->create();

        djvFileInfo fileInfo(argv[1]);
    
        if (fileInfo.isSequenceValid())
        {
            fileInfo.setType(djvFileInfo::SEQUENCE);
        }
        
        _context->load()->connect(
            this,
            SIGNAL(open(const djvFileInfo &)),
            SLOT(open(const djvFileInfo &)));
        
        Q_EMIT open(fileInfo);
    }
}

djvImagePlay2TestApplication::~djvImagePlay2TestApplication()
{}

void djvImagePlay2TestApplication::commandLineExit()
{
    exit(1);
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
    return djvImagePlay2TestApplication(argc, argv).exec();
}

