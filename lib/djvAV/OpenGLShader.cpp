//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/OpenGLShader.h>

#include <djvAV/Color.h>
#include <djvAV/OpenGL.h>
#include <djvAV/Shader.h>

#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            ShaderError::ShaderError(const std::string& what) :
                std::runtime_error(what)
            {}
            
            void Shader::_init(const std::shared_ptr<Render::Shader> & shader)
            {
                _shader = shader;

                _vertex = glCreateShader(GL_VERTEX_SHADER);
                if (!_vertex)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("The OpenGL vertex shader cannot be created.");
                    throw ShaderError(ss.str());
                }
                const char * src = _shader->getVertexSource().c_str();
                glShaderSource(_vertex, 1, &src, NULL);
                glCompileShader(_vertex);
                int success = 0;
                char infoLog[String::cStringLength];
                glGetShaderiv(_vertex, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(_vertex, String::cStringLength, NULL, infoLog);
                    std::stringstream ss;
                    ss << DJV_TEXT("The OpenGL vertex shader") << " '" << _shader->getVertexName() << "' " <<
                        DJV_TEXT("cannot be compiled") << ". " << infoLog;
                    throw ShaderError(ss.str());
                }

                _fragment = glCreateShader(GL_FRAGMENT_SHADER);
                if (!_fragment)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("The OpenGL fragment shader cannot be created.");
                    throw ShaderError(ss.str());
                }
                src = _shader->getFragmentSource().c_str();
                glShaderSource(_fragment, 1, &src, NULL);
                glCompileShader(_fragment);
                glGetShaderiv(_fragment, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(_fragment, String::cStringLength, NULL, infoLog);
                    std::stringstream ss;
                    ss << DJV_TEXT("The OpenGL framgent shader") << " '" << _shader->getFragmentName() << "' " <<
                        DJV_TEXT("cannot be compiled") << ". " << infoLog;
                    throw ShaderError(ss.str());
                }

                _program = glCreateProgram();
                glAttachShader(_program, _vertex);
                glAttachShader(_program, _fragment);
                glLinkProgram(_program);
                glGetProgramiv(_program, GL_LINK_STATUS, &success);
                if (!success)
                {
                    glGetProgramInfoLog(_program, String::cStringLength, NULL, infoLog);
                    std::stringstream ss;
                    ss << DJV_TEXT("The program") << " '" << _shader->getVertexName() << "' " <<
                        DJV_TEXT("cannot be linked") << ". " << infoLog;
                    throw std::invalid_argument(ss.str());
                }
            }

            Shader::Shader()
            {}

            Shader::~Shader()
            {
                if (_program)
                {
                    glDeleteProgram(_program);
                    _program = 0;
                }
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
            }

            std::shared_ptr<Shader> Shader::create(const std::shared_ptr<Render::Shader> & shader)
            {
                auto out = std::shared_ptr<Shader>(new Shader);
                out->_init(shader);
                return out;
            }

            GLuint Shader::getProgram() const
            {
                return _program;
            }

            void Shader::setUniform(GLint location, int value)
            {
                glUniform1i(location, value);
            }

            void Shader::setUniform(GLint location, float value)
            {
                glUniform1f(location, value);
            }

            void Shader::setUniform(GLint location, const glm::vec2 & value)
            {
                glUniform2fv(location, 1, &value[0]);
            }

            void Shader::setUniform(GLint location, const glm::vec3 & value)
            {
                glUniform3fv(location, 1, &value[0]);
            }

            void Shader::setUniform(GLint location, const glm::vec4 & value)
            {
                glUniform4fv(location, 1, &value[0]);
            }

            void Shader::setUniform(GLint location, const glm::mat3x3 & value)
            {
                glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
            }

            void Shader::setUniform(GLint location, const glm::mat4x4 & value)
            {
                glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
            }

            void Shader::setUniform(GLint location, const Image::Color& value)
            {
                auto color = value.convert(Image::Type::RGBA_F32);
                glUniform4fv(location, 1, reinterpret_cast<const GLfloat*>(color.getData()));
            }

            void Shader::setUniform(GLint location, const float value[4])
            {
                glUniform4fv(location, 1, value);
            }

            void Shader::setUniform(GLint location, const std::vector<int>& value)
            {
                glUniform1iv(location, value.size(), &value[0]);
            }

            void Shader::setUniform(GLint location, const std::vector<float>& value)
            {
                glUniform1fv(location, value.size(), &value[0]);
            }

            void Shader::setUniform(GLint location, const std::vector<glm::vec3>& value)
            {
                glUniform3fv(location, value.size(), &value[0][0]);
            }

            void Shader::setUniform(GLint location, const std::vector<glm::vec4>& value)
            {
                glUniform4fv(location, value.size(), &value[0][0]);
            }

            void Shader::setUniform(const std::string & name, int value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniform1i(loc, value);
            }

            void Shader::setUniform(const std::string & name, float value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniform1f(loc, value);
            }

            void Shader::setUniform(const std::string & name, const glm::vec2 & value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniform2fv(loc, 1, &value[0]);
            }

            void Shader::setUniform(const std::string & name, const glm::vec3 & value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniform3fv(loc, 1, &value[0]);
            }

            void Shader::setUniform(const std::string & name, const glm::vec4 & value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniform4fv(loc, 1, &value[0]);
            }

            void Shader::setUniform(const std::string & name, const glm::mat3x3 & value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniformMatrix3fv(loc, 1, GL_FALSE, &value[0][0]);
            }

            void Shader::setUniform(const std::string & name, const glm::mat4x4 & value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
            }

            void Shader::setUniform(const std::string& name, const Image::Color& value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                auto color = value.convert(Image::Type::RGBA_F32);
                glUniform4fv(loc, 1, reinterpret_cast<const GLfloat*>(color.getData()));
            }

            void Shader::setUniform(const std::string& name, const float value[4])
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniform4fv(loc, 1, value);
            }

            void Shader::setUniform(const std::string& name, const std::vector<int>& value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniform1iv(loc, value.size(), &value[0]);
            }

            void Shader::setUniform(const std::string& name, const std::vector<float>& value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniform1fv(loc, value.size(), &value[0]);
            }

            void Shader::setUniform(const std::string& name, const std::vector<glm::vec3>& value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniform3fv(loc, value.size(), &value[0][0]);
            }

            void Shader::setUniform(const std::string& name, const std::vector<glm::vec4>& value)
            {
                const GLint loc = glGetUniformLocation(_program, name.c_str());
                glUniform4fv(loc, value.size(), &value[0][0]);
            }

            void Shader::bind()
            {
                glUseProgram(_program);
            }

        } // namespace OpenGL
    } // namespace AV
} // namespace djv
