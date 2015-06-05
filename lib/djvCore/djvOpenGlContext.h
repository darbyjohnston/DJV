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

//! \file djvOpenGlContext.h

#ifndef DJV_OPEN_GL_CONTEXT_H
#define DJV_OPEN_GL_CONTEXT_H

#include <djvError.h>
#include <djvUtil.h>

#include <QString>

class  djvCoreContext;
class  djvOpenGlContextFactory;
struct djvOpenGlContextPrivate;

//! \addtogroup djvCoreOpenGL
//@{

//------------------------------------------------------------------------------
//! \class djvOpenGlContext
//!
//! This class provides an OpenGL context.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlContext
{
public:

    //! Destructor.

    virtual ~djvOpenGlContext() = 0;

    //! Bind the context.

    virtual void bind() throw (djvError) = 0;

    //! Unbind the context.

    virtual void unbind() = 0;

    //! Get the vendor.

    const QString & vendor() const;

    //! Get the renderer.

    const QString & renderer() const;

    //! Get the version.

    const QString & version() const;
    
    //! Get the context.
    
    djvCoreContext * context() const;

protected:

    //! Constructor.

    explicit djvOpenGlContext(djvCoreContext *) throw (djvError);

    //! Set the vendor.

    void setVendor(const QString &);

    //! Set the renderer.

    void setRenderer(const QString &);

    //! Set the version.

    void setVersion(const QString &);

private:

    DJV_PRIVATE_COPY(djvOpenGlContext);
    
    djvOpenGlContextPrivate * _p;

    friend class djvOpenGlContextFactory;
};

//------------------------------------------------------------------------------
//! \class djvOpenGlContextFactory
//!
//! This class provides a factory for OpenGL contexts.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlContextFactory
{
public:

    //! Constructor.
    
    djvOpenGlContextFactory(djvCoreContext *);

    //! Create an OpenGL context.

    djvOpenGlContext * create(bool bind = true) throw (djvError);

private:

    djvCoreContext * _context;
};

//------------------------------------------------------------------------------
//! \class djvOpenGlContextScope
//!
//! This class provides automatic binding and undinding for an OpenGL context.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlContextScope
{
public:

    //! Constructor.

    djvOpenGlContextScope(djvOpenGlContext *);

    //! Destructor.

    ~djvOpenGlContextScope();

private:

    DJV_PRIVATE_COPY(djvOpenGlContextScope);

    djvOpenGlContext * _context;
};

//@} // djvCoreOpenGL

#endif // DJV_OPEN_GL_CONTEXT_H

