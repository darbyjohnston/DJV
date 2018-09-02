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

#include <djvError.h>
#include <djvOpenGl.h>
#include <djvPixelData.h>

//------------------------------------------------------------------------------
//! \class djvOpenGlTexture
//!
//! This class proivides an OpenGL texture.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlTexture
{
public:
    djvOpenGlTexture();

    ~djvOpenGlTexture();
    
    //! Initialize the texture.
    void init(
        const djvPixelDataInfo &,
        gl::GLenum                   target = gl::GL_TEXTURE_2D,
        gl::GLenum                   min    = gl::GL_LINEAR,
        gl::GLenum                   mag    = gl::GL_LINEAR) throw (djvError);

    //! Initialize the texture.
    void init(
        const djvPixelData &,
        gl::GLenum           target = gl::GL_TEXTURE_2D,
        gl::GLenum           min    = gl::GL_LINEAR,
        gl::GLenum           mag    = gl::GL_LINEAR) throw (djvError);
    
    //! Get the pixel information.
    const djvPixelDataInfo & info() const;

    //! Get the target.
    gl::GLenum target() const;

    //! Get the minify filter.
    gl::GLenum min() const;

    //! Get the magnify filter.
    gl::GLenum mag() const;

    //! Get the texture ID.
    gl::GLuint id() const;

    //! Bind the texture.
    void bind();

    //! Copy pixel data to the texture.
    void copy(const djvPixelData &);

    //! Copy pixel data to the texture.
    void copy(const djvPixelData &, const djvBox2i &);

    //! Copy the current read buffer to the texture.
    void copy(const djvVector2i &);

private:
    void del();

    djvPixelDataInfo _info;
    gl::GLenum       _target;
    gl::GLenum       _min;
    gl::GLenum       _mag;
    gl::GLuint       _id;
};

