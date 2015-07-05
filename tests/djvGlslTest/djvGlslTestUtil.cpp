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

//! \file djvGlslTestUtil.cpp

#include <djvGlslTestUtil.h>

#include <djvMatrix.h>
#include <djvOpenGlTexture.h>

namespace
{

/*QString fboError(GLenum in)
{
    switch (in)
    {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";

        case GL_FRAMEBUFFER_UNSUPPORTED:
            return "GL_FRAMEBUFFER_UNSUPPORTED";

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";

        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
    }

    return "";
}*/

void fboCheck()
{
    //DJV_DEBUG("fboCheck");

    //DJV_DEBUG_PRINT("error = " <<
    //  fboError(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
}

} // namespace

djvGlslTestOffscreen::djvGlslTestOffscreen() :
    _id(0)
{}

djvGlslTestOffscreen::~djvGlslTestOffscreen()
{
    if (_id)
    {
        glDeleteFramebuffers(1, &_id);
    }
}

void djvGlslTestOffscreen::init() throw (djvError)
{
    if (_id)
        return;

    //DJV_DEBUG("djvGlslTestOffscreen::init");

    DJV_DEBUG_OPEN_GL(glGenFramebuffers(1, &_id));

    if (! _id)
        throw djvError("glGenFramebuffers");
}

void djvGlslTestOffscreen::bind()
{
    //DJV_DEBUG("djvGlslTestOffscreen::bind");

    DJV_DEBUG_OPEN_GL(glBindFramebuffer(GL_FRAMEBUFFER, _id));
}

void djvGlslTestOffscreen::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void djvGlslTestOffscreen::set(const djvOpenGlTexture & in)
{
    //DJV_DEBUG("djvGlslTestOffscreen::set");

    fboCheck();

    DJV_DEBUG_OPEN_GL(glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_RECTANGLE,
        in.id(),
        0));
}

djvGlslTestKernel::djvGlslTestKernel() :
    _size(0)
{}

namespace
{

const QString kernelSrc =
"const int kernelMax = 256;\n"
"const int kernelSize = %1;\n"
"uniform float kernelValue [kernelMax];\n"
"uniform vec2 kernelOffset [kernelMax];\n"
"\n"
"vec4 kernel(sampler2DRect texture)\n"
"{\n"
"    vec4 tmp = vec4(0.0);\n"
"    int i;\n"
"    for (i = 0; i < kernelSize; ++i)\n"
"    {\n"
"        tmp += "
"            kernelValue[i] * "
"            texture2DRect(texture, gl_TexCoord[0].st + kernelOffset[i]);\n"
"    }\n"
"    return tmp;\n"
"}\n";

} // namespace

void djvGlslTestKernel::init(int size)
{
    _size = size;

    _src = QString(kernelSrc).arg(_size);
}

const QString & djvGlslTestKernel::src() const
{
    return _src;
}

void djvGlslTestKernel::value(GLuint program, const float * in)
{
    glUniform1fv(glGetUniformLocation(program, "kernelValue"), _size, in);
}

void djvGlslTestKernel::offset(GLuint program, const float * in)
{
    glUniform2fv(glGetUniformLocation(program, "kernelOffset"), _size, in);
}

void djvGlslTestUtil::quad(const djvPixelDataInfo & in)
{
    double u [] = { 0, 0 }, v [] = { 0, 0 };
    u[! in.mirror.x] = in.size.x;
    v[! in.mirror.y] = in.size.y;
    const djvVector2f uv[] =
    {
        djvVector2f(u[0], v[0]),
        djvVector2f(u[0], v[1]),
        djvVector2f(u[1], v[1]),
        djvVector2f(u[1], v[0])
    };

    glBegin(GL_QUADS);
    djvOpenGlUtil::drawBox(djvBox2i(in.size), uv);
    glEnd();
}
