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

//! \file djvOpenGlOffscreenBuffer.h

#ifndef DJV_OPEN_GL_OFFSCREEN_BUFFER_H
#define DJV_OPEN_GL_OFFSCREEN_BUFFER_H

#include <djvError.h>
#include <djvOpenGl.h>
#include <djvPixelData.h>

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

//! \addtogroup djvCoreOpenGL
//@{

//------------------------------------------------------------------------------
//! \class djvOpenGlOffscreenBuffer
//!
//! This class provides an OpenGL offscreen buffer.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlOffscreenBuffer
{
public:

    //! Constructor.

    djvOpenGlOffscreenBuffer(const djvPixelDataInfo &) throw (djvError);

    //! Destructor.

    ~djvOpenGlOffscreenBuffer();

    //! Get the offscreen buffer information.

    const djvPixelDataInfo & info() const;

    //! Get the offscreen buffer id.

    GLuint id() const;

    //! Get the offscreen buffer texture.

    GLuint texture() const;

    //! Bind the offscreen buffer.

    void bind();

    //! Unbind the offscreen buffer.

    void unbind();
    
    //! This enumeration provides error codes.
    
    enum ERROR
    {
        ERROR_CREATE_TEXTURE,
        ERROR_INIT_TEXTURE,
        ERROR_CREATE_FBO,
        ERROR_INIT_FBO,
            
        ERROR_COUNT
    };
    
    //! Get the error code labels.
    
    static const QStringList & errorLabels();

private:

    DJV_PRIVATE_COPY(djvOpenGlOffscreenBuffer);

    djvPixelDataInfo _info;
    GLuint           _id;
    GLuint           _texture;
    GLint            _restore;
};

//------------------------------------------------------------------------------
//! \class djvOpenGlOffscreenBufferScope
//!
//! This class provides automatic binding for an OpenGL offscreen buffer.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlOffscreenBufferScope
{
public:

    //! Constructor.

    djvOpenGlOffscreenBufferScope(djvOpenGlOffscreenBuffer *);

    //! Destructor.

    ~djvOpenGlOffscreenBufferScope();

private:

    DJV_PRIVATE_COPY(djvOpenGlOffscreenBufferScope);

    djvOpenGlOffscreenBuffer * _buffer;
};

//@} // djvCoreOpenGL

#endif // DJV_OPEN_GL_OFFSCREEN_BUFFER_H
