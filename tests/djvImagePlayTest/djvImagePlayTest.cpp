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

//! \file djvImagePlayTest.cpp

#include <djvImagePlayTest.h>

#include <djvWindowUtil.h>

#include <djvError.h>
#include <djvOpenGlImage.h>
#include <djvPixel.h>
#include <djvTimer.h>

#include <QCursor>
#include <QKeyEvent>
#include <QMouseEvent>

void djvImagePlayTestWidget::showEvent(QShowEvent *)
{
    viewFit();
}

void djvImagePlayTestWidget::mousePressEvent(QMouseEvent * event)
{
    const djvVector2i mouse(
        event->pos().x(),
        height() - 1 - event->pos().y());
    
    _viewPosTmp = viewPos();
    
    _mousePress = mouse;
}

void djvImagePlayTestWidget::mouseMoveEvent(QMouseEvent * event)
{
    const djvVector2i mouse(
        event->pos().x(),
        height() - 1 - event->pos().y());
    
    setViewPos(_viewPosTmp + mouse - _mousePress);
}

void djvImagePlayTestWidget::keyPressEvent(QKeyEvent * event)
{
    const QPoint pos = mapFromGlobal(QCursor::pos());
    
    const djvVector2i mouse(
        pos.x(),
        height() - 1 - pos.y());
    
    switch (event->key())
    {
        case Qt::Key_0:         viewZero(); break;
        case Qt::Key_Minus:     setViewZoom(viewZoom() * 0.5, mouse); break;
        case Qt::Key_Equal:     setViewZoom(viewZoom() * 2.0, mouse); break;
        case Qt::Key_Backspace: viewFit(); break;
    }
}

djvImagePlayTestApplication::djvImagePlayTestApplication(
    int     argc,
    char ** argv) :
    djvApplication("djvImagePlayTest", argc, argv),
    _load  (0),
    _cache (false),
    _widget(0),
    _frame (0)
{
    // Command line.

    if (argc != 2)
    {
        print("Usage: djvImagePlayTest (input)");
        
        setExitValue(djvApplicationEnum::EXIT_ERROR);
        
        return;
    }

    _file.setFileName(argv[1]);

    if (_file.isSequenceValid())
    {
        _file.setType(djvFileInfo::SEQUENCE);
    }

    try
    {
        _load = djvImageIoFactory::global()->load(_file, _info);
        
        if (! _load)
            throw djvError(QString("Cannot open image \"%1\"").arg(_file));
    }
    catch (const djvError & error)
    {
        printError(error);
        
        setExitValue(djvApplicationEnum::EXIT_ERROR);
        
        return;
    }

    setValid(true);

    // Widgets.

    _widget = new djvImagePlayTestWidget;

    // Initialize.
    
    _widget->setWindowTitle("djvImagePlayTest");

    //_widget->zoom(0.5);

    const djvVector2i size = djvWindowUtil::resize(_info.size);
    
    _widget->resize(size.x, size.y);

    _widget->show();
    
    startTimer(0);
}

djvImagePlayTestApplication::~djvImagePlayTestApplication()
{
    if (_widget)
    {
        delete _widget;
        
        _widget = 0;
    }
    
    if (_load)
    {
        delete _load;
        
        _load = 0;
    }
}

void djvImagePlayTestApplication::timerEvent(QTimerEvent * event)
{
    //DJV_DEBUG("djvImagePlayTestApplication::timerEvent");
    
    _widget->setWindowTitle(
        QString("%1 Frame: %2").arg("djvImagePlayTest").arg(_frame));

    static djvTimer t;
    static double   average = 0.0;
    static int      accum   = 0;
    
    t.check();
    
    const double fps = t.fps();
    
    t.start();

    if (fps < 1000.0)
    {
        average += fps;
        
        ++accum;
    }

    print(QString("FPS = %1 (%2)").
        arg(fps).
        arg(accum ? (average / static_cast<double>(accum)) : 0.0));

    djvImage * imageP = 0;
    
    try
    {
        if (_cache)
        {
            if (_cachedImages.count() < _info.sequence.frames.count())
            {
                djvImage * image = new djvImage;
                
                _load->read(*image, _info.sequence.frames[_frame]);
                
                _cachedImages += image;
                
                imageP = image;
            }
            else
            {
                imageP = _cachedImages[_frame];
            }
        }
        else
        {
            _load->read(_image, _info.sequence.frames[_frame]);
            
            imageP = &_image;
        }
    }
    catch (const djvError &)
    {}

    if (imageP && imageP->isValid())
    {
        _widget->setData(imageP);
        
        djvOpenGlImageOptions options;
        options.colorProfile = imageP->colorProfile;
        _widget->setOptions(options);

        _widget->updateGL();
    }

    ++_frame;

    if (_frame >= _info.sequence.frames.count())
    {
        _frame = 0;
    }
}

int main(int argc, char ** argv)
{
    return djvImagePlayTestApplication(argc, argv).run();
}

