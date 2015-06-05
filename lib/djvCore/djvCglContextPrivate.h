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

//! \file djvCglContextPrivate.h

#ifndef DJV_CGL_CONTEXT_PRIVATE_H
#define DJV_CGL_CONTEXT_PRIVATE_H

#include <djvOpenGlContext.h>

#include <djvOpenGl.h>

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

struct djvCglContextPrivate;

//! \addtogroup djvCoreOpenGL
//@{

//------------------------------------------------------------------------------
//! \class djvCglContext
//!
//! This class provides an Apple OS X OpenGL context.
//------------------------------------------------------------------------------

class djvCglContext : public djvOpenGlContext
{
public:

    //! Destructor.

    virtual ~djvCglContext();
    
    //! This enumeration provides error codes.
    
    enum ERROR
    {
        ERROR_PIXEL_FORMAT,
        ERROR_CREATE_CONTEXT,
        ERROR_INIT_GLEW,
        ERROR_NO_FBO,
        ERROR_INVALID_CONTEXT,
        ERROR_BIND_CONTEXT,
        
        ERROR_COUNT
    };
    
    //! Get error code labels.
    
    static const QStringList & errorLabels();

    virtual void bind() throw (djvError);

    virtual void unbind();

protected:

    //! Constructor.

    explicit djvCglContext(djvCoreContext *) throw (djvError);

private:

    DJV_PRIVATE_COPY(djvCglContext);
    
    djvCglContextPrivate * _p;

    friend class djvOpenGlContextFactory;
};

//@} // djvCoreOpenGL

#endif // DJV_CGL_CONTEXT_PRIVATE_H
