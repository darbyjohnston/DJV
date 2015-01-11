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

//! \file djvCglContextPrivate.cpp

#include <djvCglContextPrivate.h>

#include <djvDebug.h>
#include <djvDebugLog.h>

#include <QCoreApplication>

#if defined (DJV_OSX)
#include <OpenGL/OpenGL.h>
#endif // DJV_OSX

//------------------------------------------------------------------------------
// PixelFormat
//------------------------------------------------------------------------------

#if defined (DJV_OSX)

namespace
{

class PixelFormat
{
public:

    PixelFormat() throw (djvError) :
        _format(0)
    {
        CGLPixelFormatAttribute attribs [] =
        {
            //kCGLPFAColorSize, (CGLPixelFormatAttribute)32,
            //kCGLPFADoubleBuffer,
            kCGLPFAAccelerated,
            (CGLPixelFormatAttribute)0
        };

        GLint npix = 0;

        CGLError error = CGLChoosePixelFormat(attribs, &_format, &npix);

        if (error != kCGLNoError)
        {
            throw djvError(
                "djvCglContextPrivate",
                djvCglContextPrivate::errorLabels()[
                    djvCglContextPrivate::ERROR_PIXEL_FORMAT].arg(error));
        }
    }

    ~PixelFormat()
    {
        if (_format)
        {
            CGLDestroyPixelFormat(_format);
        }
    }

    CGLPixelFormatObj format() const
    {
        return _format;
    }

private:

    CGLPixelFormatObj _format;
};

} // namespace

#endif // DJV_OSX

//------------------------------------------------------------------------------
// djvCglContextPrivate::P
//------------------------------------------------------------------------------

struct djvCglContextPrivate::P
{
#   if defined (DJV_OSX)

    P() :
        context(0)
    {}
    
    CGLContextObj context;

#endif
};

//------------------------------------------------------------------------------
// djvCglContextPrivate
//------------------------------------------------------------------------------

djvCglContextPrivate::djvCglContextPrivate() throw (djvError) :
    _p(new P)
{
#   if defined (DJV_OSX)

    //DJV_DEBUG("djvCglContextPrivate::djvCglContextPrivate");

    // Create the pixel format.

    PixelFormat pixelFormat;

    // Create the context.

	DJV_LOG("djvCglContextPrivate", "Creating OpenGL context...");

    CGLError error = CGLCreateContext(pixelFormat.format(), 0, &_p->context);

    if (error != kCGLNoError || ! _p->context)
    {
        throw djvError(
            "djvCglContextPrivate",
            errorLabels()[ERROR_CREATE_CONTEXT].arg(error));
    }

    // Bind the context.

    bind();

    // Initialize GLEW.
	
    DJV_LOG("djvCglContextPrivate", "Initializing GLEW...");

    GLint glError = glewInit();

    if (glError != GLEW_OK)
    {
        throw djvError(
            "djvCglContextPrivate",
            errorLabels()[ERROR_INIT_GLEW].arg(glError));
    }

    setVendor  ((const char *)glGetString(GL_VENDOR));
    setRenderer((const char *)glGetString(GL_RENDERER));
    setVersion ((const char *)glGetString(GL_VERSION));

    //DJV_DEBUG_PRINT("vendor string = " << vendor());
    //DJV_DEBUG_PRINT("renderer string = " << renderer());
    //DJV_DEBUG_PRINT("version string = " << version());
    //DJV_DEBUG_PRINT("extensions = " <<
    //    (const char *)glGetString(GL_EXTENSIONS));
    //DJV_DEBUG_PRINT("glu version = " <<
    //    (const char *)gluGetString(GLU_VERSION));
    //DJV_DEBUG_PRINT("glu extensions = " <<
    //    (const char *)gluGetString(GLU_EXTENSIONS));

    DJV_LOG("djvCglContextPrivate", QString("GL vendor: \"%1\"").arg(vendor()));
    DJV_LOG("djvCglContextPrivate", QString("GL renderer: \"%1\"").arg(renderer()));
    DJV_LOG("djvCglContextPrivate", QString("GL version: \"%1\"").arg(version()));

    if (! GL_EXT_framebuffer_object)
    {
        throw djvError(
            "djvCglContextPrivate",
            errorLabels()[ERROR_NO_FBO]);
    }

#   endif // DJV_OSX
}

djvCglContextPrivate::~djvCglContextPrivate()
{
#   if defined (DJV_OSX)

    //DJV_DEBUG("djvCglContextPrivate::~djvCglContextPrivate");

    if (_p->context)
    {
        //DJV_DEBUG_PRINT("context");

        CGLDestroyContext(_p->context);
    }
    
    delete _p;

#   endif // DJV_OSX
}

const QStringList & djvCglContextPrivate::errorLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvCglContextPrivate", "Cannot get pixel format: #%1") <<
        qApp->translate("djvCglContextPrivate", "Cannot create OpenGL context: #%1") <<
        qApp->translate("djvCglContextPrivate", "Cannot initialize GLEW: #%1") <<
        qApp->translate("djvCglContextPrivate", "No OpenGL FBO support") <<
        qApp->translate("djvCglContextPrivate", "Invalid OpenGL context") <<
        qApp->translate("djvCglContextPrivate", "Cannot bind OpenGL context: #%1");
    
    DJV_ASSERT(ERROR_COUNT == data.count());
    
    return data;
}

void djvCglContextPrivate::bind() throw (djvError)
{
#   if defined (DJV_OSX)

    if (! _p->context)
    {
        throw djvError(
            "djvCglContextPrivate",
            errorLabels()[ERROR_INVALID_CONTEXT]);
    }

    //DJV_DEBUG("djvCglContextPrivate::bind");

    CGLError error = CGLSetCurrentContext(_p->context);

    if (error != kCGLNoError)
    {
        throw djvError(
            "djvCglContextPrivate",
            errorLabels()[ERROR_BIND_CONTEXT].arg(error));
    }

#   endif // DJV_OSX
}

void djvCglContextPrivate::unbind()
{
#   if defined (DJV_OSX)

    //DJV_DEBUG("djvCglContextPrivate::unbind");

    CGLSetCurrentContext(0);

#   endif // DJV_OSX
}

