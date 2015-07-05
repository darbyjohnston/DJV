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

//! \file djvImagePlay2TestContext.h

#ifndef DJV_IMAGE_PLAY2_TEST_CONTEXT_H
#define DJV_IMAGE_PLAY2_TEST_CONTEXT_H

#include <djvImagePlay2TestPlayback.h>

#include <djvGuiContext.h>

#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QScopedPointer>
#include <QThread>

class djvImagePlay2TestLoad;

//------------------------------------------------------------------------------
// djvImagePlay2TestContext
//------------------------------------------------------------------------------

class djvImagePlay2TestContext : public djvGuiContext
{
    Q_OBJECT

public:

    explicit djvImagePlay2TestContext(QObject * parent = 0);

    virtual ~djvImagePlay2TestContext();

    QOffscreenSurface * surface() const;

    QOpenGLContext * glContext() const;

    unsigned int texture() const;

    djvImagePlay2TestLoad * load() const;

    djvImagePlay2TestPlayback * playback() const;

private:

    QScopedPointer<QOffscreenSurface>         _surface;
    QScopedPointer<QOpenGLContext>            _glContext;
    unsigned int                              _texture;
    djvImagePlay2TestLoad *                   _load;
    QThread                                   _thread;
    QScopedPointer<djvImagePlay2TestPlayback> _playback;
};

#endif // DJV_IMAGE_PLAY2_TEST_CONTEXT_H
