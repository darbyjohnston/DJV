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

#include <djvImagePlay2TestLoad.h>

#include <djvImagePlay2TestContext.h>

#include <QCoreApplication>
#include <QOpenGLTexture>
#include <QMutexLocker>

//------------------------------------------------------------------------------
// djvImagePlay2TestLoad
//------------------------------------------------------------------------------

djvImagePlay2TestLoad::djvImagePlay2TestLoad(djvImagePlay2TestContext * context) :
    _frame       (0),
    _everyFrame  (true),
    _accum       (0),
    _shareContext(0),
    _frontTexture(0),
    _backTexture (0),
    _context     (context)
{
    _surface.reset(new QOffscreenSurface);
    _surface->create();
    
    startTimer(0);
}

djvImagePlay2TestLoad::~djvImagePlay2TestLoad()
{
    _glContext->makeCurrent(_surface.data());
    for (int i = 0; i < 2; ++i)
    {
        _textures[i]->destroy();
        delete _textures[i];        
        _textures[0] = 0;
    }
}

bool djvImagePlay2TestLoad::hasEveryFrame() const
{
    return _everyFrame;
}
    
void djvImagePlay2TestLoad::setShareContext(QOpenGLContext * context)
{
    _shareContext = context;
}

QOpenGLTexture * djvImagePlay2TestLoad::frontTexture() const
{
    return _frontTexture;
}

QOpenGLTexture * djvImagePlay2TestLoad::backTexture() const
{
    return _backTexture;
}

void djvImagePlay2TestLoad::start()
{
    //DJV_DEBUG("djvImagePlay2TestLoad::start");
    _glContext.reset(new QOpenGLContext);
    _glContext->setShareContext(_shareContext);
    _glContext->create();
    for (int i = 0; i < 2; ++i)
    {
        _textures[i] = new QOpenGLTexture(QOpenGLTexture::Target2D);
    }
    _backTexture  = _textures[0];
    _frontTexture = _textures[1];
}

void djvImagePlay2TestLoad::open(const djvFileInfo & fileInfo)
{
    //DJV_DEBUG("djvImagePlay2TestLoad::open");
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    try
    {
        _glContext->makeCurrent(_surface.data());
        _imageLoad.reset(
            _context->imageIoFactory()->load(fileInfo, _imageIoInfo));
        _fileInfo = fileInfo;
        for (int i = 0; i < 2; ++i)
        {
            _textures[i]->destroy();
            _textures[i]->create();
            _textures[i]->setSize(_imageIoInfo.size.x, _imageIoInfo.size.y);
            _textures[i]->setFormat(QOpenGLTexture::RGBA8_UNorm);
            _textures[i]->allocateStorage();
        }
    }
    catch (const djvError & error)
    {
    }
    Q_EMIT fileChanged(_imageIoInfo);
    read(0);
}

void djvImagePlay2TestLoad::read(qint64 frame)
{
    //DJV_DEBUG("djvImagePlay2TestLoad::read");
    //DJV_DEBUG_PRINT("pending events = " << qApp->hasPendingEvents());
    if (_everyFrame)
    {    
        readInternal(frame);
    }
    else
    {
        _frame = frame;
        ++_accum;
    }
}
    
void djvImagePlay2TestLoad::setEveryFrame(bool everyFrame)
{
    if (everyFrame == _everyFrame)
        return;
    _everyFrame = everyFrame;
    Q_EMIT everyFrameChanged(_everyFrame);
}

void djvImagePlay2TestLoad::timerEvent(QTimerEvent *)
{
    if (_accum > 0)
    {
        readInternal(_frame);
        _accum = 0;
    }
}

void djvImagePlay2TestLoad::readInternal(qint64 frame)
{
    if (! _imageLoad.data())
        return;
    //DJV_DEBUG("djvImagePlay2TestLoad::readInternal");
    //DJV_DEBUG_PRINT("frame = " << frame);
    //QMutexLocker locker(_context->mutex());
    try
    {
        _glContext->makeCurrent(_surface.data());
        _imageLoad->read(_image, frame);
        //DJV_DEBUG_PRINT("image = " << _image);
        _backTexture->setData(
            0,
            0,
            QOpenGLTexture::RGBA,
            QOpenGLTexture::UInt8,
            _image.data());
        //glFlush();
        QOpenGLTexture * tmp = _frontTexture;
        _frontTexture = _backTexture;
        _backTexture = tmp;
    }
    catch (const djvError & error)
    {
    }
    Q_EMIT imageRead();
}
