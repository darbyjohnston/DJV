//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvGraphics/OpenGLShader.h>

#include <djvCore/Debug.h>

#include <QCoreApplication>

namespace djv
{
    namespace Graphics
    {
        OpenGLShader::~OpenGLShader()
        {
            del();
        }

        namespace
        {
            void shaderCompile(GLuint id, const QString & source)
            {
                //DJV_DEBUG("shaderCompile");
                //DJV_DEBUG_PRINT("source = " << source);
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                std::vector<char> buf(source.length());
                memcpy(buf.data(), source.toLatin1().data(), buf.size());
                const char * sources[] = { buf.data() };
                const GLint  sourceLengths[] = { static_cast<GLint>(buf.size()) };
                glFuncs->glShaderSource(id, 1, sources, sourceLengths);
                glFuncs->glCompileShader(id);
                GLint error = 0;
                glFuncs->glGetShaderiv(id, GL_COMPILE_STATUS, &error);
                char    log[4096] = "";
                GLsizei logSize = 0;
                glFuncs->glGetShaderInfoLog(id, 4096, &logSize, log);
                //DJV_DEBUG_PRINT("log = " << QString(log));
                if (error != GL_TRUE)
                {
                    throw Core::Error(
                        "djv::Graphics::OpenGLShader",
                        qApp->translate("djv::Graphics::OpenGLShader", "Cannot compile shader:\n%1").
                        arg(log));
                }
            }

        } // namespace

        void OpenGLShader::init(
            const QString & vertexSource,
            const QString & fragmentSource)
        {
            if (vertexSource == _vertexSource && fragmentSource == _fragmentSource)
                return;

            //DJV_DEBUG("OpenGLShader::init");
            //DJV_DEBUG_PRINT("vertexSource = " << vertexSource);
            //DJV_DEBUG_PRINT("fragmentSource = " << fragmentSource);

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

            del();

            _vertexSource = vertexSource;
            _fragmentSource = fragmentSource;

            _vertexId = glFuncs->glCreateShader(GL_VERTEX_SHADER);
            _fragmentId = glFuncs->glCreateShader(GL_FRAGMENT_SHADER);
            shaderCompile(_vertexId, _vertexSource);
            shaderCompile(_fragmentId, _fragmentSource);
            _programId = glFuncs->glCreateProgram();
            if (!_programId)
            {
                throw Core::Error(
                    "djv::Graphics::OpenGLShader",
                    qApp->translate("djv::Graphics::OpenGLShader", "Cannot create shader program"));
            }
            glFuncs->glAttachShader(_programId, _vertexId);
            glFuncs->glAttachShader(_programId, _fragmentId);
            glFuncs->glLinkProgram(_programId);
            GLint error = 0;
            glFuncs->glGetProgramiv(_programId, GL_LINK_STATUS, &error);
            char log[4096] = "";
            GLsizei logSize = 0;
            glFuncs->glGetProgramInfoLog(_programId, 4096, &logSize, log);
            //DJV_DEBUG_PRINT("log = " << QString(log));
            if (error != GL_TRUE)
            {
                throw Core::Error(
                    "djv::Graphics::OpenGLShader",
                    qApp->translate("djv::Graphics::OpenGLShader", "Cannot link shader:\n%1").
                    arg(log));
            }
        }

        void OpenGLShader::bind()
        {
            //DJV_DEBUG("OpenGLShader::bind");
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            glFuncs->glUseProgram(_programId);
        }

        const QString & OpenGLShader::vertexSource() const
        {
            return _vertexSource;
        }

        const QString & OpenGLShader::fragmentSource() const
        {
            return _fragmentSource;
        }

        GLuint OpenGLShader::program() const
        {
            return _programId;
        }

        void OpenGLShader::setUniform(const QString& name, int value)
        {
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            glFuncs->glUniform1i(
                glFuncs->glGetUniformLocation(_programId, name.toLatin1().data()),
                static_cast<GLint>(value));
        }

        void OpenGLShader::setUniform(const QString& name, float value)
        {
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            glFuncs->glUniform1f(
                glFuncs->glGetUniformLocation(_programId, name.toLatin1().data()),
                static_cast<GLfloat>(value));
        }

        void OpenGLShader::setUniform(const QString& name, const glm::mat4x4& value)
        {
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            glFuncs->glUniformMatrix4fv(
                glFuncs->glGetUniformLocation(_programId, name.toLatin1().data()),
                1,
                false,
                &value[0][0]);
        }

        void OpenGLShader::del()
        {
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            if (_vertexId)
            {
                glFuncs->glDeleteShader(_vertexId);
                _vertexId = 0;
            }
            if (_fragmentId)
            {
                glFuncs->glDeleteShader(_fragmentId);
                _fragmentId = 0;
            }
            if (_programId)
            {
                glFuncs->glDeleteProgram(_programId);
                _programId = 0;
            }
        }

    } // namespace Graphics
} // namespace djv
