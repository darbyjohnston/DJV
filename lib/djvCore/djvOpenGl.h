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

#pragma once

#include <djvBox.h>
#include <djvColor.h>
#include <djvPixel.h>
#include <djvVector.h>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

//! \addtogroup djvCoreOpenGL
//@{

//------------------------------------------------------------------------------
//! \class djvOpenGlUtil
//!
//! This class provides OpenGL utilities.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlUtil
{
public:
    virtual ~djvOpenGlUtil() = 0;
    
    //! Setup an orthographic camera.    
    static void ortho(
        const glm::ivec2 & size,
        const glm::vec2 &  minMax = glm::vec2(-1.f, 1.f));

    //! Convert a pixel format to OpenGL.
    static gl::GLenum format(djvPixel::PIXEL, bool bgr = false);

    //! Convert a pixel type to OpenGL.
    static gl::GLenum type(djvPixel::PIXEL);

    //! Set the current OpenGL color.
    static void color(const djvColor &);

    //! Draw a box.
    static void drawBox(const djvBox2i &);

    //! Draw a box.
    static void drawBox(const djvBox2f &);

    //! Draw a box.
    static void drawBox(const djvBox2i &, const glm::vec2 uv[4]);

    //! Draw a box.
    static void drawBox(const djvBox2f &, const glm::vec2 uv[4]);
    
    //! Get an OpenGL error string.
    static QString errorString(gl::GLenum);
};

//! This macro provides OpenGL debugging functionality.
//!
//! References:
//!
//! - Dave Shreiner, "Performance OpenGL: Platform Independent Techniques"
//!   SIGGRAPH 2001
#if 0
#define DJV_DEBUG_OPEN_GL(cmd) \
    cmd; \
    { \
        gl::GLenum error = gl::GL_NO_ERROR; \
        if ((error = gl::glGetError()) != gl::GL_NO_ERROR) \
        { \
            DJV_DEBUG_PRINT(QString("%1 = %2 (%3, line %4)"). \
                arg(#cmd). \
                arg((djvOpenGlUtil::errorString(error)). \
                arg(__FILE__). \
                arg(__LINE__)); \
        } \
    }
#else
#define DJV_DEBUG_OPEN_GL(cmd) \
    cmd;
#endif

//@} // djvCoreOpenGL

