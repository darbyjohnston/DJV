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

#include <djvGlslTestColor.h>
#include <djvGlslTestLevels.h>
#include <djvGlslTestExposure.h>
#include <djvGlslTestBlur.h>
#include <djvGlslTestSharpen.h>
#include <djvGlslTestEdge.h>
#include <djvGlslTestScale.h>

#include <djvIconLibrary.h>
#include <djvToolButton.h>
#include <djvWindowUtil.h>

#include <djvFileInfo.h>
#include <djvOpenGlOffscreenBuffer.h>
#include <djvSignalBlocker.h>

#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>

djvGlslTestApplication::djvGlslTestApplication(int argc, char ** argv) :
    djvApplication("djvGlslTest", argc, argv),
    _play        (false),
    _frame       (0),
    _timerId     (0),
    _op          (0)
{
    //DJV_DEBUG("djvGlslTestApplication::djvGlslTestApplication");

    // Parset the command line.
    
    if (argc != 2)
    {
        print("Usage: djvGlslTest (image)");
        
        setExitValue(djvApplicationEnum::EXIT_ERROR);
        
        return;
    }

    try
    {
        djvFileInfo file(argv[1]);

        if (file.isSequenceValid())
        {
            file.setType(djvFileInfo::SEQUENCE);
        }

        _load.reset(djvImageIoFactory::global()->load(file, _info));
        
        if (! _load.data())
            throw djvError(QString("Cannot open image \"%1\"").arg(file));
    }
    catch (const djvError & error)
    {
        printError(error);
        
        return;
    }

    setValid(true);
    
    // Create the ops.

    _ops += new djvGlslTestNullOp();
    _ops += new djvGlslTestColorOp();
    _ops += new djvGlslTestLevelsOp();
    _ops += new djvGlslTestExposureOp();
    _ops += new djvGlslTestBlurOp();
    _ops += new djvGlslTestSharpenOp();
    _ops += new djvGlslTestEdgeOp();
    _ops += new djvGlslTestScaleOp();

    // Create the widgets.
    
    QWidget * window = new QWidget;
    window->setWindowTitle("djvGlslTest");
    
    _view = new djvGlslTestWidget;
    
    _playWidget = new djvToolButton;
    _playWidget->setIcon(djvIconLibrary::global()->icon("djvPlayForwardIcon.png"));
    _playWidget->setCheckable(true);
    
    _slider = new djvIntSlider;

    QTabWidget * tabWidget = new QTabWidget;
    tabWidget->setTabPosition(QTabWidget::West);

    for (int i = 0; i < _ops.count(); ++i)
    {
        tabWidget->addTab(_ops[i], _ops[i]->name());
    }
    
    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(window);
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

    _slider->setRange(0, _info.sequence.frames.count() - 1);
    
    tabWidget->setCurrentIndex(_op);
    
    const djvVector2i size = djvWindowUtil::resize(
        _info.size + djvVector2i(300, 0));
    window->resize(size.x, size.y);

    splitter->setSizes(QList<int>() << 300 << (size.x - 300));
    
    frameUpdate();
    update();

    window->show();
    
    // Setup the callbacks.

    connect(_playWidget, SIGNAL(toggled(bool)), SLOT(playCallback(bool)));
    
    connect(_slider, SIGNAL(valueChanged(int)), SLOT(frameCallback(int)));
    
    connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(opCallback(int)));

    for (int i = 0; i < _ops.count(); ++i)
    {
        connect(_ops[i], SIGNAL(changed()), SLOT(updateCallback()));
    }
}

void djvGlslTestApplication::timerEvent(QTimerEvent *)
{
    //DJV_DEBUG("djvGlslTestApplication::timerEvent");
    
    _frame++;

    if (_frame >= _info.sequence.frames.count())
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

void djvGlslTestApplication::opCallback(int in)
{
    _op = in;
    
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
        const int64_t frame =
            _info.sequence.frames.count() ? _info.sequence.frames[_frame] : -1;
        
        _load->read(_image, frame);
        
        _view->set(_ops[_op], &_image);
        _view->update();
    }
    catch (const djvError &)
    {}
}

int main(int argc, char ** argv)
{
    return djvGlslTestApplication(argc, argv).run();
}

