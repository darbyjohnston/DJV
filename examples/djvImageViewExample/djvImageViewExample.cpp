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

#include <djvImageViewExample.h>

#include <djvUI/WindowUtil.h>

#include <djvAV/OpenGLImage.h>
#include <djvAV/Pixel.h>

#include <djvCore/Error.h>

#include <QCursor>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>

namespace djv
{
    ImageViewExampleWidget::ImageViewExampleWidget(const QPointer<UI::UIContext> & context) :
        UI::ImageView(context)
    {}

    void ImageViewExampleWidget::mousePressEvent(QMouseEvent * event)
    {
        const glm::ivec2 mouse(
            event->pos().x(),
            height() - 1 - event->pos().y());
        _viewPosTmp = viewPos();
        _mousePress = mouse;
    }

    void ImageViewExampleWidget::mouseMoveEvent(QMouseEvent * event)
    {
        const glm::ivec2 mouse(
            event->pos().x(),
            height() - 1 - event->pos().y());
        setViewPos(_viewPosTmp + mouse - _mousePress);
    }

    void ImageViewExampleWidget::keyPressEvent(QKeyEvent * event)
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

    ImageViewExampleApplication::ImageViewExampleApplication(int & argc, char ** argv) :
        QApplication(argc, argv)
    {
        _context.reset(new UI::UIContext(argc, argv));
        if (argc != 2)
        {
            _context->printMessage("Usage: djvImageViewExample (input)");
            QTimer::singleShot(0, this, SLOT(commandLineExit()));
        }
        else
        {
            _fileInfo = Core::FileInfo(argv[1]);
            QTimer::singleShot(0, this, SLOT(work()));
        }
    }

    void ImageViewExampleApplication::commandLineExit()
    {
        exit(1);
    }

    void ImageViewExampleApplication::work()
    {
        try
        {
            AV::IOInfo info;
            _load = _context->ioFactory()->load(_fileInfo, info);
            _image = std::shared_ptr<AV::Image>(new AV::Image);
            _load->read(*_image);
        }
        catch (const Core::Error & error)
        {
            _context->printError(error);
            exit(1);
            return;
        }

        _widget.reset(new ImageViewExampleWidget(_context.data()));
        _widget->setWindowTitle("djvImageViewExample");
        _widget->setData(_image);
        if (_image)
        {
            AV::OpenGLImageOptions options;
            options.colorProfile = _image->colorProfile;
            _widget->setOptions(options);
            const glm::ivec2 size = UI::WindowUtil::resize(_image->size());
            _widget->resize(size.x, size.y);
        }
        _widget->show();
    }

} // namespace djv

int main(int argc, char ** argv)
{
    djv::Core::CoreContext::initLibPaths(argc, argv);
    return djv::ImageViewExampleApplication(argc, argv).exec();
}

