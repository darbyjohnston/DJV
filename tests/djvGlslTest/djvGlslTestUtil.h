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

//! \file djvGlslTestUtil.h

#ifndef DJV_GLSL_TEST_UTIL_H
#define DJV_GLSL_TEST_UTIL_H

#include <djvOpenGl.h>
#include <djvPixelData.h>

#include <djvError.h>

class djvGlslTestTexture;

//------------------------------------------------------------------------------
// djvGlslTestOffscreen
//------------------------------------------------------------------------------

class djvGlslTestOffscreen
{
public:

    djvGlslTestOffscreen();

    ~djvGlslTestOffscreen();

    void init() throw (djvError);

    void bind();

    static void unbind();

    void set(const djvGlslTestTexture &);

private:

    GLuint _id;
};

//------------------------------------------------------------------------------
// djvGlslTestShader
//------------------------------------------------------------------------------

class djvGlslTestShader
{
public:

    djvGlslTestShader();

    ~djvGlslTestShader();

    void init(const QString &) throw (djvError);

    void bind();

    static void unbind();

    GLuint program() const;

private:

    void del();

    GLuint _vertex;
    GLuint _fragment;
    GLuint _program;
};

//------------------------------------------------------------------------------
// djvGlslTestTexture
//------------------------------------------------------------------------------

class djvGlslTestTexture
{
public:

    djvGlslTestTexture();

    ~djvGlslTestTexture();

    void init(
        const djvPixelDataInfo &,
        GLenum min = GL_LINEAR,
        GLenum mag = GL_LINEAR) throw (djvError);

    void init(
        const djvPixelData &,
        GLenum min = GL_LINEAR,
        GLenum mag = GL_LINEAR) throw (djvError);

    void copy(const djvPixelData &);

    void copy(const djvPixelData &, const djvBox2i &);

    void copy(const djvVector2i &);

    void bind();

    const djvPixelDataInfo & info() const;

    GLenum min() const;

    GLenum mag() const;

    GLuint id() const;

private:

    void del();

    djvPixelDataInfo _info;
    GLenum           _min;
    GLenum           _mag;
    GLuint           _id;
};

//------------------------------------------------------------------------------
// djvGlslTestKernel
//------------------------------------------------------------------------------

class djvGlslTestKernel
{
public:

    djvGlslTestKernel();

    void init(int size);

    const QString & src() const;

    //void size(int);

    void value(GLuint, const float *);

    void offset(GLuint, const float *);

private:

    int     _size;
    QString _src;
};

//------------------------------------------------------------------------------
// djvGlslTestUtil
//------------------------------------------------------------------------------

struct djvGlslTestUtil
{
    static void quad(const djvPixelDataInfo &);
};

#endif // DJV_GLSL_TEST_UTIL_H
