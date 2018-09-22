//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
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

#include <MainWindow.h>

#include <Context.h>
#include <ImageLoad.h>
#include <OpDockWidget.h>
#include <OpListDockWidget.h>
#include <OpsManager.h>
#include <Playback.h>
#include <PlaybackToolBar.h>
#include <ViewWidget.h>

//------------------------------------------------------------------------------
// MainWindow
//------------------------------------------------------------------------------

MainWindow::MainWindow(
    ImageLoad * imageLoad,
    OpsManager * opsManager,
    Playback * playback,
    Context * context,
    QWidget * parent) :
    QMainWindow(parent),
    _imageLoad          (imageLoad),
    _opsManager         (opsManager),
    _playback           (playback),
    _viewWidget         (0),
    _playbackToolBar    (0),
    _opListDockWidget   (0),
    _currentOpDockWidget(0)
{
    _viewWidget = new ViewWidget(imageLoad, opsManager, playback, context);
    _playbackToolBar = new PlaybackToolBar(playback, context);
    _opListDockWidget = new OpListDockWidget(opsManager);    
    _currentOpDockWidget = new OpDockWidget(opsManager);
    
    setWindowTitle("djvGLSLExperiment");
    setCentralWidget(_viewWidget);
    addToolBar(Qt::BottomToolBarArea, _playbackToolBar);
    addDockWidget(Qt::LeftDockWidgetArea, _opListDockWidget);
    addDockWidget(Qt::LeftDockWidgetArea, _currentOpDockWidget);
    
    resize(1280, 720);
}

