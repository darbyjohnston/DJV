//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvImagePlayExample.h>

#include <djvUI/WindowUtil.h>

#include <djvCore/Error.h>
#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/Pixel.h>
#include <djvCore/Timer.h>

#include <QCursor>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>

namespace djv
{
    ImagePlayExampleWidget::ImagePlayExampleWidget(const QPointer<UI::UIContext> & context) :
        UI::ImageView(context)
    {}

    void ImagePlayExampleWidget::showEvent(QShowEvent *)
    {
        viewFit();
    }

    void ImagePlayExampleWidget::mousePressEvent(QMouseEvent * event)
    {
        const glm::ivec2 mouse(
            event->pos().x(),
            height() - 1 - event->pos().y());
        _viewPosTmp = viewPos();
        _mousePress = mouse;
    }

    void ImagePlayExampleWidget::mouseMoveEvent(QMouseEvent * event)
    {
        const glm::ivec2 mouse(
            event->pos().x(),
            height() - 1 - event->pos().y());
        setViewPos(_viewPosTmp + mouse - _mousePress);
    }

    void ImagePlayExampleWidget::keyPressEvent(QKeyEvent * event)
    {
        const QPoint pos = mapFromGlobal(QCursor::pos());
        const glm::ivec2 mouse(
            pos.x(),
            height() - 1 - pos.y());
        switch (event->key())
        {
        case Qt::Key_0:         viewZero(); break;
        case Qt::Key_Minus:     setViewZoom(viewZoom() * .5f, mouse); break;
        case Qt::Key_Equal:     setViewZoom(viewZoom() * 2.f, mouse); break;
        case Qt::Key_Backspace: viewFit(); break;
        }
    }

    ImagePlayExampleApplication::ImagePlayExampleApplication(int & argc, char ** argv) :
        QApplication(argc, argv),
        _cache(false),
        _frame(0)
    {
        _context.reset(new UI::UIContext(argc, argv));
        if (argc != 2)
        {
            _context->printMessage("Usage: djvImagePlayExample (input)");
            QTimer::singleShot(0, this, SLOT(commandLineExit()));
        }
        else
        {
            _fileInfo = Core::FileInfo(argv[1]);
            QTimer::singleShot(0, this, SLOT(work()));
        }
    }

    void ImagePlayExampleApplication::commandLineExit()
    {
        exit(1);
    }

    void ImagePlayExampleApplication::work()
    {
        if (_fileInfo.isSequenceValid())
        {
            _fileInfo.setType(Core::FileInfo::SEQUENCE);
        }
        try
        {
            _load = _context->imageIOFactory()->load(_fileInfo, _info);
        }
        catch (const Core::Error & error)
        {
            _context->printError(error);
            exit(1);
            return;
        }
        _widget.reset(new ImagePlayExampleWidget(_context.data()));
        //_widget->zoom(0.5);
        const glm::ivec2 size = UI::WindowUtil::resize(_info.size);
        _widget->resize(size.x, size.y);
        _widget->show();
        startTimer(0);
    }

    void ImagePlayExampleApplication::timerEvent(QTimerEvent * event)
    {
        //DJV_DEBUG("ImagePlayExampleApplication::timerEvent");

        _widget->setWindowTitle(
            QString("%1 Frame: %2").arg("djvImagePlayExample").arg(_frame));

        static Core::Timer t;
        static float    average = 0.f;
        static int      accum = 0;
        t.check();
        const float fps = t.fps();
        t.start();
        if (fps < 1000.f)
        {
            average += fps;
            ++accum;
        }

        _context->printMessage(QString("FPS = %1 (%2)").
            arg(fps).
            arg(accum ? (average / static_cast<float>(accum)) : 0.f));

        Graphics::Image * imageP = 0;
        try
        {
            if (_cache)
            {
                if (_cachedImages.count() < _info.sequence.frames.count())
                {
                    Graphics::Image * image = new Graphics::Image;
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
        catch (const Core::Error &)
        {
        }

        if (imageP && imageP->isValid())
        {
            _widget->setData(imageP);
            Graphics::OpenGLImageOptions options;
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

} // namespace djv

int main(int argc, char ** argv)
{
    djv::Core::CoreContext::initLibPaths(argc, argv);
    return djv::ImagePlayExampleApplication(argc, argv).exec();
}

