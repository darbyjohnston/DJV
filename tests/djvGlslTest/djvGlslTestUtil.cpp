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
#include <djvOpenGlImage.h>

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

void djvGlslTestOffscreen::set(const djvGlslTestTexture & in)
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

djvGlslTestShader::djvGlslTestShader() :
    _vertex  (0),
    _fragment(0),
    _program (0)
{}

djvGlslTestShader::~djvGlslTestShader()
{
    del();
}

namespace
{

const QString vertexSrc =
"void main(void)\n"
"{\n"
"    gl_FrontColor  = gl_Color;\n"
"    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"}\n";

} // namespace

void djvGlslTestShader::init(const QString & in) throw (djvError)
{
    //DJV_DEBUG("djvGlslTestShader::init");
    //DJV_DEBUG_PRINT("in = " << in);

    GLint error = 0;

    del();

    DJV_DEBUG_OPEN_GL(_vertex = glCreateShader(GL_VERTEX_SHADER));

    if (! _vertex)
        throw djvError("Cannot create vertex shader");

    djvMemoryBuffer<char> buf(vertexSrc.length());
    djvMemory::copy(vertexSrc.toLatin1().data(), buf.data(), buf.size());
    
    const char * vertexSources [] = { buf.data() };
    const GLint  vertexLengths [] = { buf.size() };
    
    DJV_DEBUG_OPEN_GL(glShaderSource(_vertex, 1, vertexSources, vertexLengths));
    
    DJV_DEBUG_OPEN_GL(glCompileShader(_vertex));
    
    char log[djvStringUtil::cStringLength] = "";
    glGetShaderInfoLog(_vertex, djvStringUtil::cStringLength, 0, log);
    //DJV_DEBUG_PRINT("log = " << log);
    
    glGetShaderiv(_vertex, GL_COMPILE_STATUS, &error);

    if (! error)
        throw djvError(
            QString("Cannot compile vertex shader: %1").arg(log));

    DJV_DEBUG_OPEN_GL(_fragment = glCreateShader(GL_FRAGMENT_SHADER));

    if (! _fragment)
        throw djvError("Cannot create fragment shader");

    buf.setSize(in.length());
    djvMemory::copy(in.toLatin1().data(), buf.data(), buf.size());
    
    const char * fragmentSources [] = { buf.data() };
    const GLint  fragmentLengths [] = { buf.size() };
    
    DJV_DEBUG_OPEN_GL(glShaderSource(_fragment, 1, fragmentSources, fragmentLengths));
    
    DJV_DEBUG_OPEN_GL(glCompileShader(_fragment));
    
    glGetShaderInfoLog(_fragment, djvStringUtil::cStringLength, 0, log);
    //DJV_DEBUG_PRINT("log = " << log);
    glGetShaderiv(_fragment, GL_COMPILE_STATUS, &error);

    if (! error)
        throw djvError(
            QString("Cannot compile fragment shader: %1").arg(log));

    DJV_DEBUG_OPEN_GL(_program = glCreateProgram());
    DJV_DEBUG_OPEN_GL(glAttachShader(_program, _vertex));
    DJV_DEBUG_OPEN_GL(glAttachShader(_program, _fragment));
    DJV_DEBUG_OPEN_GL(glLinkProgram(_program));
    glGetProgramInfoLog(_program, djvStringUtil::cStringLength, 0, log);
    //DJV_DEBUG_PRINT("log = " << log);
    glGetProgramiv(_program, GL_LINK_STATUS, &error);

    if (! error)
        throw djvError(QString("Cannot link program: %1").arg(log));
}

void djvGlslTestShader::bind()
{
    //DJV_DEBUG("djvGlslTestShader::bind");

    DJV_DEBUG_OPEN_GL(glUseProgram(_program));
}

void djvGlslTestShader::unbind()
{
    glUseProgram(0);
}

void djvGlslTestShader::del()
{
    if (_vertex)
    {
        glDeleteShader(_vertex);
        
        _vertex = 0;
    }

    if (_fragment)
    {
        glDeleteShader(_fragment);
        
        _fragment = 0;
    }

    if (_program)
    {
        glDeleteShader(_program);
        
        _program = 0;
    }
}

GLuint djvGlslTestShader::program() const
{
    return _program;
}

djvGlslTestTexture::djvGlslTestTexture() :
    _min(GL_LINEAR),
    _mag(GL_LINEAR),
    _id (0)
{}

djvGlslTestTexture::~djvGlslTestTexture()
{
    del();
}

void djvGlslTestTexture::init(
    const djvPixelDataInfo & in,
    GLenum                   min,
    GLenum                   mag) throw (djvError)
{
    if (in == _info && min == _min && mag == _mag)
        return;
    
    //DJV_DEBUG("Texture::init");
    //DJV_DEBUG_PRINT("in = " << in);

    _info = in;
    _min = min;
    _mag = mag;

    del();

    DJV_DEBUG_OPEN_GL(glGenTextures(1, &_id));

    if (! _id)
        throw djvError("glGenTextures");

    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_RECTANGLE, _id));
    DJV_DEBUG_OPEN_GL(glTexParameteri(
        GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP));
    DJV_DEBUG_OPEN_GL(glTexParameteri(
        GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP));
    DJV_DEBUG_OPEN_GL(glTexParameteri(
        GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, _mag));
    DJV_DEBUG_OPEN_GL(glTexParameteri(
        GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, _min));

    GLenum format = GL_RGBA;

    if (
        djvPixel::F16 == djvPixel::type(_info.pixel) ||
        djvPixel::F32 == djvPixel::type(_info.pixel))
    {
        format = GL_RGBA32F;
    }

    DJV_DEBUG_OPEN_GL(glTexImage2D(
        GL_TEXTURE_RECTANGLE,
        0,
        format,
        _info.size.x,
        _info.size.y,
        0,
        djvOpenGlUtil::format(_info.pixel, _info.bgr),
        djvOpenGlUtil::type(_info.pixel),
        0));
}

void djvGlslTestTexture::init(const djvPixelData & in, GLenum min, GLenum mag)
    throw (djvError)
{
    //DJV_DEBUG("djvGlslTestTexture::init");
    //DJV_DEBUG_PRINT("in = " << in);

    init(in.info(), min, mag);
    bind();
    copy(in);
}

void djvGlslTestTexture::del()
{
    if (_id)
    {
        glDeleteTextures(1, &_id);
        
        _id = 0;
    }
}

void djvGlslTestTexture::copy(const djvPixelData & in)
{
    //DJV_DEBUG("djvGlslTestTexture::copy");
    //DJV_DEBUG_PRINT("in = " << in);

    const djvPixelDataInfo & info = in.info();

    djvOpenGlImage::stateUnpack(in.info());

    DJV_DEBUG_OPEN_GL(glTexSubImage2D(
        GL_TEXTURE_RECTANGLE,
        0,
        0,
        0,
        info.size.x,
        info.size.y,
        djvOpenGlUtil::format(info.pixel, info.bgr),
        djvOpenGlUtil::type(info.pixel),
        in.data()));
}

void djvGlslTestTexture::copy(const djvPixelData & in, const djvBox2i & area)
{
    //DJV_DEBUG("djvGlslTestTexture::copy");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("area = " << area);

    const djvPixelDataInfo & info = in.info();

    djvVector2i position = area.position;

    if (info.mirror.x)
    {
        position.x = info.size.x - area.position.x - area.size.x;
    }

    if (info.mirror.y)
    {
        position.y = info.size.y - area.position.y - area.size.y;
    }

    djvOpenGlImage::stateUnpack(in.info(), position);

    DJV_DEBUG_OPEN_GL(glTexSubImage2D(
        GL_TEXTURE_RECTANGLE,
        0,
        0,
        0,
        area.size.x,
        area.size.y,
        djvOpenGlUtil::format(info.pixel, info.bgr),
        djvOpenGlUtil::type(info.pixel),
        in.data()));
}

void djvGlslTestTexture::copy(const djvVector2i & in)
{
    //DJV_DEBUG("djvGlslTestTexture::copy");
    //DJV_DEBUG_PRINT("in = " << in);

    DJV_DEBUG_OPEN_GL(glCopyTexSubImage2D(
        GL_TEXTURE_RECTANGLE,
        0,
        0,
        0,
        0,
        0,
        in.x,
        in.y));
}

void djvGlslTestTexture::bind()
{
    //DJV_DEBUG("djvGlslTestTexture::bind");

    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_RECTANGLE, _id));
}

const djvPixelDataInfo & djvGlslTestTexture::info() const
{
    return _info;
}

GLenum djvGlslTestTexture::min() const
{
    return _min;
}
GLenum djvGlslTestTexture::mag() const
{
    return _mag;
}
GLuint djvGlslTestTexture::id() const
{
    return _id;
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
