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

//! \file djvGlslTestWindow.cpp

#include <djvGlslTestWindow.h>

#include <djvGlslTestImageLoad.h>
#include <djvGlslTestPlayback.h>
#include <djvGlslTestPlaybackWidget.h>

#include <djvGlslTestContext.h>
#include <djvGlslTestOp.h>
#include <djvGlslTestOpManager.h>

#include <djvImageView.h>

#include <djvImage.h>

#include <QSplitter>
#include <QStackedWidget>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvGlslTestWindow
//------------------------------------------------------------------------------

djvGlslTestWindow::djvGlslTestWindow(
    djvGlslTestImageLoad * imageLoad,
    djvGlslTestOpManager * opManager,
    djvGlslTestPlayback *  playback,
    djvGlslTestContext *   context,
    QWidget *              parent) :
    QWidget(parent),
    _imageLoad     (imageLoad),
    _opManager     (opManager),
    _playback      (playback),
    _view          (0),
    _playbackWidget(0),
    _stackWidget   (0),
    _splitter      (0)
{
    _view = new djvImageView(context);
    
    _playbackWidget = new djvGlslTestPlaybackWidget(playback, context);
    
    _stackWidget = new QStackedWidget;
    Q_FOREACH(djvGlslTestOp * op, opManager->list())
        _stackWidget->addWidget(op);
    
    _splitter = new QSplitter;
    _splitter->addWidget(_stackWidget);
    _splitter->addWidget(_view);
    
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_splitter, 1);
    layout->addWidget(_playbackWidget);

    _view->setData(imageLoad->image(playback->frame()));
    
    connect(
        playback,
        SIGNAL(frameChanged(qint64)),
        SLOT(frameCallback(qint64)));
}

void djvGlslTestWindow::setImage(djvImage * image)
{
    _view->setData(image);
    _view->updateGL();
}

void djvGlslTestWindow::frameCallback(qint64 frame)
{
    _view->setData(_imageLoad->image(frame));
    _view->updateGL();
}
