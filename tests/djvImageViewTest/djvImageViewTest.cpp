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

//! \file djvImageViewTest.cpp

#include <djvImageViewTest.h>

#include <djvWindowUtil.h>

#include <djvError.h>
#include <djvOpenGlImage.h>
#include <djvPixel.h>

#include <QCursor>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>

djvImageViewTestWidget::djvImageViewTestWidget(djvGuiContext * context) :
    djvImageView(context)
{}

void djvImageViewTestWidget::mousePressEvent(QMouseEvent * event)
{
    const djvVector2i mouse(
        event->pos().x(),
        height() - 1 - event->pos().y());
    
    _viewPosTmp = viewPos();
    
    _mousePress = mouse;
}

void djvImageViewTestWidget::mouseMoveEvent(QMouseEvent * event)
{
    const djvVector2i mouse(
        event->pos().x(),
        height() - 1 - event->pos().y());
    
    setViewPos(_viewPosTmp + mouse - _mousePress);
}

void djvImageViewTestWidget::keyPressEvent(QKeyEvent * event)
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

djvImageViewTestApplication::djvImageViewTestApplication(int & argc, char ** argv) :
    QApplication(argc, argv)
{
    _context.reset(new djvGuiContext);
    
    if (argc != 2)
    {
        _context->printMessage("Usage: djvImageViewTest (input)");
        
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
        _fileInfo = djvFileInfo(argv[1]);
        
        QTimer::singleShot(0, this, SLOT(work()));
    }
}

void djvImageViewTestApplication::commandLineExit()
{
    exit(1);
}

void djvImageViewTestApplication::work()
{
    try
    {
        djvImageIoInfo info;
        
        _load.reset(_context->imageIoFactory()->load(_fileInfo, info));
        
        _load->read(_image);
    }
    catch (const djvError & error)
    {
        _context->printError(error);
        
        exit(1);
        
        return;
    }
    
    _widget.reset(new djvImageViewTestWidget(_context.data()));
    _widget->setWindowTitle("djvImageViewTest");
    _widget->setData(&_image);

    djvOpenGlImageOptions options;
    options.colorProfile = _image.colorProfile;
    _widget->setOptions(options);

    const djvVector2i size = djvWindowUtil::resize(_image.size());
    _widget->resize(size.x, size.y);
    _widget->show();
}

int main(int argc, char ** argv)
{
    return djvImageViewTestApplication(argc, argv).exec();
}

