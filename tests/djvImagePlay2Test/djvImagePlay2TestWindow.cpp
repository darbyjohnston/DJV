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

//! \file djvImagePlay2TestWindow.cpp

#include <djvImagePlay2TestWindow.h>

#include <djvImagePlay2TestContext.h>
#include <djvImagePlay2TestLoad.h>
#include <djvImagePlay2TestPlayback.h>
#include <djvImagePlay2TestPlayBar.h>
#include <djvImagePlay2TestView.h>

#include <QGridLayout>
#include <QMutexLocker>

//------------------------------------------------------------------------------
// djvImagePlay2TestWindow
//------------------------------------------------------------------------------

djvImagePlay2TestWindow::djvImagePlay2TestWindow(djvImagePlay2TestContext * context) :
    _view   (0),
    _playBar(0),
    _context(context)
{
    _view = new djvImagePlay2TestView(context);
    
    _playBar = new djvImagePlay2TestPlayBar(context);
    
    QGridLayout * layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->addWidget(_view, 1, 1);
    layout->addWidget(_playBar, 2, 0, 1, 3);
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(1, 1);
    
    connect(
        context->load(),
        SIGNAL(fileChanged(const djvImageIoInfo &)),
        SLOT(fileCallback(const djvImageIoInfo &)));
    
    connect(
        context->load(),
        SIGNAL(imageRead()),
        SLOT(imageCallback()));
}

QOpenGLContext * djvImagePlay2TestWindow::glContext() const
{
    return _view->context();
}

void djvImagePlay2TestWindow::fileCallback(const djvImageIoInfo & info)
{
    resize(info.size.x, info.size.y);
    
    _view->setInfo(info);
    
    _context->playback()->setFrameList(info.sequence.frames);
    _context->playback()->setSpeed(info.sequence.speed);
}

void djvImagePlay2TestWindow::imageCallback()
{
    _view->update();
}
