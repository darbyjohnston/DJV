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

#include <djvGraphics/Pixel.h>

#include <djvCore/Box.h>
#include <djvCore/Vector.h>

#include <QOpenGLFunctions_4_1_Core>

namespace djv
{
    namespace Graphics
    {
        //! \class OpenGLUtil
        //!
        //! This class provides OpenGL utilities.
        class OpenGLUtil
        {
        public:
            virtual ~OpenGLUtil() = 0;

            //! Convert a pixel format to OpenGL.
            static GLenum format(Pixel::PIXEL, bool bgr = false);

            //! Convert a pixel type to OpenGL.
            static GLenum type(Pixel::PIXEL);

            //! Get an OpenGL error string.
            static QString errorString(GLenum);
        };

    } // namespace Graphics
} // namespace djv

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
        GLenum error = GL_NO_ERROR; \
        if ((error = glGetError()) != GL_NO_ERROR) \
        { \
            DJV_DEBUG_PRINT(QString("%1 = %2 (%3, line %4)"). \
                arg(#cmd). \
                arg((djv::Graphics::OpenGLUtil::errorString(error)). \
                arg(__FILE__). \
                arg(__LINE__)); \
        } \
    }
#else
#define DJV_DEBUG_OPEN_GL(cmd) \
    cmd;
#endif
