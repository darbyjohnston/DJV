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

//! \file djvImagePlay2TestLoad.cpp

#include <djvImagePlay2TestLoad.h>

#include <djvImagePlay2TestContext.h>

#include <QCoreApplication>
#include <QOpenGLContext>

//------------------------------------------------------------------------------
// djvImagePlay2TestLoad
//------------------------------------------------------------------------------

djvImagePlay2TestLoad::djvImagePlay2TestLoad(djvImagePlay2TestContext * context) :
    _frame     (0),
    _frameTmp  (-1),
    _accum     (0),
    _everyFrame(true),
    _timer     (0),
    _context   (context)
{}

djvImagePlay2TestLoad::~djvImagePlay2TestLoad()
{
    if (_timer)
    {
        killTimer(_timer);
        
        _timer = 0;
    }
}

bool djvImagePlay2TestLoad::hasEveryFrame() const
{
    return _everyFrame;
}

void djvImagePlay2TestLoad::open(const djvFileInfo & fileInfo)
{
    try
    {
        _imageLoad.reset(
            _context->imageIoFactory()->load(fileInfo, _imageIoInfo));
        
        _fileInfo = fileInfo;
    }
    catch (const djvError & error)
    {
    }

    Q_EMIT fileChanged(_imageIoInfo);
    
    read(0);
}

void djvImagePlay2TestLoad::read(qint64 frame)
{
    if (_everyFrame)
    {
        readInternal(frame);
    }
    else
    {
        _frameTmp = frame;
    
        ++_accum;
        
        if (! _timer)
        {
            _timer = startTimer(0);
        }
    }
}
    
void djvImagePlay2TestLoad::setEveryFrame(bool everyFrame)
{
    if (everyFrame == _everyFrame)
        return;
    
    _everyFrame = everyFrame;
    
    if (_timer)
    {
        killTimer(_timer);
        
        _timer = 0;
    }
    
    Q_EMIT everyFrameChanged(_everyFrame);
}

void djvImagePlay2TestLoad::timerEvent(QTimerEvent *)
{
    qApp->processEvents();
    
    if (_frameTmp != _frame)
    {
        _frame = _frameTmp;
        
        readInternal(_frame);
    }
}

void djvImagePlay2TestLoad::readInternal(qint64 frame)
{
    if (! _imageLoad.data())
        return;

    DJV_DEBUG("djvImagePlay2TestLoad::readInternal");
    DJV_DEBUG_PRINT("frame = " << frame);
    DJV_DEBUG_PRINT("accum = " << _accum);
    
    try
    {
        _imageLoad->read(_image, frame);
    
        _context->glContext()->makeCurrent(_context->surface());
        
        _context->texture()->bind();
        _context->texture()->setData(
            0,
            0,
            QOpenGLTexture::RGB,
            QOpenGLTexture::UInt8,
            _image.data());
    }
    catch (const djvError & error)
    {
    }
    
    _accum = 0;
    
    if (_timer)
    {
        killTimer(_timer);
        
        _timer = 0;
    }
    
    Q_EMIT imageRead();
}
