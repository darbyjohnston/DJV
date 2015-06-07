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

//! \file djvGlslTestApplication.cpp

#include <djvGlslTestApplication.h>

#include <QTimer>

/*#include <djvGlslTestColor.h>
#include <djvGlslTestLevels.h>
#include <djvGlslTestExposure.h>
#include <djvGlslTestBlur.h>
#include <djvGlslTestSharpen.h>
#include <djvGlslTestEdge.h>
#include <djvGlslTestScale.h>
#include <djvGlslTestWidget.h>*/

/*#include <djvIconLibrary.h>
#include <djvIntSlider.h>
#include <djvToolButton.h>
#include <djvWindowUtil.h>

#include <djvOpenGlOffscreenBuffer.h>
#include <djvSignalBlocker.h>

#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>*/

djvGlslTestApplication::djvGlslTestApplication(int & argc, char ** argv) :
    QApplication(argc, argv)
{
    //DJV_DEBUG("djvGlslTestApplication::djvGlslTestApplication");
    
#   if QT_VERSION < 0x050000
    setStyle(new QPlastiqueStyle);
#   else
    setStyle("fusion");
#   endif

    // Create the context.
    
    _context.reset(new djvGlslTestContext);
    
    // Parse the command line.
    
    if (! _context->commandLine(argc, argv))
    {
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
        QTimer::singleShot(0, this, SLOT(work()));
    }
}

void djvGlslTestApplication::commandLineExit()
{
    exit(1);
}

void djvGlslTestApplication::work()
{
    //DJV_DEBUG("djvGlslTestApplication::work");
    
    _imageLoad.reset(new djvGlslTestImageLoad(_context.data()));
    
    try
    {
        djvFileInfo fileInfo = _context->fileInfo();
        
        if (fileInfo.isSequenceValid())
        {
            fileInfo.setType(djvFileInfo::SEQUENCE);
        }
        
        _imageLoad->load(fileInfo);
    }
    catch (const djvError & error)
    {
        _context->printError(error);
        
        exit(1);
        
        return;
    }

    _context->setValid(true);
    
    _opManager.reset(new djvGlslTestOpManager(_context.data()));
    
    _playback.reset(new djvGlslTestPlayback);
    _playback->setRange(
        _imageLoad->info().sequence.start(),
        _imageLoad->info().sequence.end());
    _playback->setFrame(_imageLoad->info().sequence.start());
    _playback->setSpeed(_imageLoad->info().sequence.speed);
    
    _window.reset(new djvGlslTestWindow(
        _imageLoad.data(),
        _opManager.data(),
        _playback.data(),
        _context.data()));
    
    //_playback->setPlayback(djvGlslTestPlayback::FORWARD);
    
    _window->show();

    // Create the ops.

    /*_imageOpList += new djvGlslTestNullOp(_context.data());
    _imageOpList += new djvGlslTestColorOp(_context.data());
    _imageOpList += new djvGlslTestLevelsOp(_context.data());
    _imageOpList += new djvGlslTestExposureOp(_context.data());
    _imageOpList += new djvGlslTestBlurOp(_context.data());
    _imageOpList += new djvGlslTestSharpenOp(_context.data());
    _imageOpList += new djvGlslTestEdgeOp(_context.data());
    _imageOpList += new djvGlslTestScaleOp(_context.data());

    // Create the widgets.
    
    _window = new QWidget;
    _window->setWindowTitle("djvGlslTest");
    
    _view = new djvGlslTestWidget(_context.data());
    
    _playWidget = new djvToolButton;
    _playWidget->setIcon(_context->iconLibrary()->icon("djvPlayForwardIcon.png"));
    _playWidget->setCheckable(true);
    
    _slider = new djvIntSlider;

    QTabWidget * tabWidget = new QTabWidget;
    tabWidget->setTabPosition(QTabWidget::West);

    for (int i = 0; i < _imageOpList.count(); ++i)
    {
        tabWidget->addTab(_imageOpList[i], _imageOpList[i]->name());
    }
    
    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(_window);
    layout->setMargin(0);
    layout->setSpacing(0);
    
    QSplitter * splitter = new QSplitter;
    splitter->addWidget(tabWidget);
    splitter->addWidget(_view);
    layout->addWidget(splitter, 1);
    
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->setMargin(5);
    hLayout->setSpacing(5);
    hLayout->addWidget(_playWidget);
    hLayout->addWidget(_slider);
    layout->addLayout(hLayout);

    // Initialize.

    _slider->setRange(0, _imageLoadInfo.sequence.frames.count() - 1);
    
    tabWidget->setCurrentIndex(_currentImageOp);
    
    const djvVector2i size = djvWindowUtil::resize(
        _imageLoadInfo.size + djvVector2i(300, 0));
    _window->resize(size.x, size.y);

    splitter->setSizes(QList<int>() << 300 << (size.x - 300));
    
    frameUpdate();
    update();

    _window->show();
    
    // Setup the callbacks.

    connect(_playWidget, SIGNAL(toggled(bool)), SLOT(playCallback(bool)));
    
    connect(_slider, SIGNAL(valueChanged(int)), SLOT(frameCallback(int)));
    
    connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(currentImageOpCallback(int)));

    for (int i = 0; i < _imageOpList.count(); ++i)
    {
        connect(_imageOpList[i], SIGNAL(changed()), SLOT(updateCallback()));
    }*/
}

/*void djvGlslTestApplication::timerEvent(QTimerEvent *)
{
    DJV_DEBUG("djvGlslTestApplication::timerEvent");
    
    _frame++;

    if (_frame >= _imageLoadInfo.sequence.frames.count())
    {
        _frame = 0;
    }

    frameUpdate();
    update();
}

void djvGlslTestApplication::playCallback(bool in)
{
    _play = in;
    
    playUpdate();
}

void djvGlslTestApplication::frameCallback(int in)
{
    _play = false;
    
    _frame = in;
    
    playUpdate();
    frameUpdate();
    update();
}

void djvGlslTestApplication::currentImageOpCallback(int in)
{
    _currentImageOp = in;
    
    update();
}

void djvGlslTestApplication::updateCallback()
{
    update();
}

void djvGlslTestApplication::playUpdate()
{
    djvSignalBlocker signalBlocker(_playWidget);

    if (_play)
    {
        _timerId = startTimer(0);
    }
    else if (_timerId != 0)
    {
        killTimer(_timerId);
        
        _timerId = 0;
    }

    _playWidget->setChecked(_play);
}

void djvGlslTestApplication::frameUpdate()
{
    djvSignalBlocker signalBlocker(_slider);

    _slider->setValue(_frame);
}

void djvGlslTestApplication::update()
{
    djvSignalBlocker signalBlocker(_view);

    try
    {
        const int64_t frame = _imageLoadInfo.sequence.frames.count() ?
            _imageLoadInfo.sequence.frames[_frame] :
            -1;
        
        _imageLoad->read(_image, frame);
        
        _view->set(_imageOpList[_currentImageOp], &_image);
        _view->update();
    }
    catch (const djvError &)
    {}
}*/

int main(int argc, char ** argv)
{
    return djvGlslTestApplication(argc, argv).exec();
}

