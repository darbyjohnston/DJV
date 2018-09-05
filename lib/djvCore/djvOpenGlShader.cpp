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

#include <djvOpenGlShader.h>

#include <djvDebug.h>

#include <QCoreApplication>

using namespace gl;

//------------------------------------------------------------------------------
// djvOpenGlShader
//------------------------------------------------------------------------------

djvOpenGlShader::~djvOpenGlShader()
{
    del();
}

namespace
{
GLuint shaderCreate(GLenum type) throw (djvError)
{
    //DJV_DEBUG("shaderCreate");
    GLuint r = 0;
    DJV_DEBUG_OPEN_GL(r = glCreateShader(type));
    if (! r)
    {
        throw djvError(
            "djvOpenGlShader",
            qApp->translate("djvOpenGlShader", "Cannot create shader"));
    }
    return r;
}

void shaderCompile(GLuint id, const QString & source)
{
    //DJV_DEBUG("shaderCompile");
    //DJV_DEBUG_PRINT("source = " << source);
    std::vector<char> buf(source.length());
    memcpy(&buf.front(), source.toLatin1().data(), buf.size());
    const char * sources       [] = { &buf.front() };
    const GLint  sourceLengths [] = { static_cast<GLint>(buf.size()) };
    DJV_DEBUG_OPEN_GL(glShaderSource(id, 1, sources, sourceLengths));
    DJV_DEBUG_OPEN_GL(glCompileShader(id));
    GLboolean error = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &error);
    char    log [4096] = "";
    GLsizei logSize    = 0;
    glGetShaderInfoLog(id, 4096, &logSize, log);
    //DJV_DEBUG_PRINT("log = " << QString(log));
    if (error != GL_TRUE)
    {
        throw djvError(
            "djvOpenGlShader",
            qApp->translate("djvOpenGlShader", "Cannot compile shader:\n%1").
            arg(log));
    }
}

} // namespace

void djvOpenGlShader::init(
    const QString & vertexSource,
    const QString & fragmentSource) throw (djvError)
{
    if (vertexSource == _vertexSource && fragmentSource == _fragmentSource)
        return;

    //DJV_DEBUG("djvOpenGlShader::init");
    //DJV_DEBUG_PRINT("vertexSource = " << vertexSource);
    //DJV_DEBUG_PRINT("fragmentSource = " << fragmentSource);

    del();

    _vertexSource   = vertexSource;
    _fragmentSource = fragmentSource;

    GLboolean error = GL_FALSE;
    _vertexId   = glCreateShader(GL_VERTEX_SHADER);
    _fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
    shaderCompile(_vertexId,   _vertexSource);
    shaderCompile(_fragmentId, _fragmentSource);
    DJV_DEBUG_OPEN_GL(_programId = glCreateProgram());
    if (! _programId)
    {
        throw djvError(
            "djvOpenGlShader",
            qApp->translate("djvOpenGlShader", "Cannot create shader program"));
    }
    DJV_DEBUG_OPEN_GL(glAttachShader(_programId, _vertexId));
    DJV_DEBUG_OPEN_GL(glAttachShader(_programId, _fragmentId));
    DJV_DEBUG_OPEN_GL(glLinkProgram (_programId));
    glGetProgramiv(_programId, GL_LINK_STATUS, &error);
    char log [4096] = "";
    GLsizei logSize = 0;
    glGetProgramInfoLog(_programId, 4096, &logSize, log);
    //DJV_DEBUG_PRINT("log = " << QString(log));
    if (error != GL_TRUE)
    {
        throw djvError(
            "djvOpenGlShader",
            qApp->translate("djvOpenGlShader", "Cannot link shader:\n%1").
            arg(log));
    }
}

void djvOpenGlShader::bind()
{
    //DJV_DEBUG("djvOpenGlShader::bind");
    DJV_DEBUG_OPEN_GL(glUseProgram(_programId));
}
    
const QString & djvOpenGlShader::vertexSource() const
{
    return _vertexSource;
}

const QString & djvOpenGlShader::fragmentSource() const
{
    return _fragmentSource;
}

GLuint djvOpenGlShader::program() const
{
    return _programId;
}

void djvOpenGlShader::del()
{
    if (_vertexId)
    {
        glDeleteShader(_vertexId);
        _vertexId = 0;
    }
    if (_fragmentId)
    {
        glDeleteShader(_fragmentId);
        _fragmentId = 0;
    }
    if (_programId)
    {
        glDeleteProgram(_programId);
        _programId = 0;
    }
}
