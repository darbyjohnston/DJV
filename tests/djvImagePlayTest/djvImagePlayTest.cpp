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

#include <djvImagePlayTest.h>

#include <djvWindowUtil.h>

#include <djvError.h>
#include <djvOpenGlImage.h>
#include <djvPixel.h>
#include <djvTimer.h>

#include <QCursor>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>

djvImagePlayTestWidget::djvImagePlayTestWidget(djvGuiContext * context) :
    djvImageView(context)
{}

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

djvImagePlayTestApplication::djvImagePlayTestApplication(int & argc, char ** argv) :
    QApplication(argc, argv),
    _cache(false),
    _frame(0)
{
    _context.reset(new djvGuiContext);
    if (argc != 2)
    {
        _context->printMessage("Usage: djvImagePlayTest (input)");
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
        _fileInfo = djvFileInfo(argv[1]);
        QTimer::singleShot(0, this, SLOT(work()));
    }
}

void djvImagePlayTestApplication::commandLineExit()
{
    exit(1);
}

void djvImagePlayTestApplication::work()
{
    if (_fileInfo.isSequenceValid())
    {
        _fileInfo.setType(djvFileInfo::SEQUENCE);
    }
    try
    {
        _load.reset(_context->imageIoFactory()->load(_fileInfo, _info));
    }
    catch (const djvError & error)
    {
        _context->printError(error);
        exit(1);
        return;
    }
    _widget.reset(new djvImagePlayTestWidget(_context.data()));
    _widget->setWindowTitle("djvImagePlayTest");
    //_widget->zoom(0.5);
    const djvVector2i size = djvWindowUtil::resize(_info.size);
    _widget->resize(size.x, size.y);
    _widget->show();
    startTimer(0);
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

    _context->printMessage(QString("FPS = %1 (%2)").
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
        _widget->update();
    }

    ++_frame;
    if (_frame >= _info.sequence.frames.count())
    {
        _frame = 0;
    }
}

int main(int argc, char ** argv)
{
    return djvImagePlayTestApplication(argc, argv).exec();
}

