//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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
                    ss << DJV_TEXT("The OpenGL fragment shader") << " '" << _shader->getFragmentName() << "' " <<
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
                if (Image::Type::L_U8 == value.getType())
                {
                    const Image::U8_T* p = reinterpret_cast<const Image::U8_T*>(value.getData());
                    const GLfloat v[] =
                    {
                        p[0] / static_cast<GLfloat>(Image::U8Range.max),
                        0.F,
                        0.F,
                        1.F
                    };
                    glUniform4fv(location, 1, v);
                }
                else if (Image::Type::LA_U8 == value.getType())
                {
                    const Image::U8_T* p = reinterpret_cast<const Image::U8_T*>(value.getData());
                    const GLfloat v[] =
                    {
                        p[0] / static_cast<GLfloat>(Image::U8Range.max),
                        0.F,
                        0.F,
                        p[1] / static_cast<GLfloat>(Image::U8Range.max)
                    };
                    glUniform4fv(location, 1, v);
                }
                else if (Image::Type::RGB_U8 == value.getType())
                {
                    const Image::U8_T* p = reinterpret_cast<const Image::U8_T*>(value.getData());
                    const GLfloat v[] =
                    {
                        p[0] / static_cast<GLfloat>(Image::U8Range.max),
                        p[1] / static_cast<GLfloat>(Image::U8Range.max),
                        p[2] / static_cast<GLfloat>(Image::U8Range.max),
                        1.F
                    };
                    glUniform4fv(location, 1, v);
                }
                else if (Image::Type::RGBA_U8 == value.getType())
                {
                    const Image::U8_T* p = reinterpret_cast<const Image::U8_T*>(value.getData());
                    const GLfloat v[] =
                    {
                        p[0] / static_cast<GLfloat>(Image::U8Range.max),
                        p[1] / static_cast<GLfloat>(Image::U8Range.max),
                        p[2] / static_cast<GLfloat>(Image::U8Range.max),
                        p[3] / static_cast<GLfloat>(Image::U8Range.max)
                    };
                    glUniform4fv(location, 1, v);
                }
                else if (Image::Type::L_F32 == value.getType())
                {
                    const Image::F32_T* p = reinterpret_cast<const Image::F32_T*>(value.getData());
                    const GLfloat v[] = { p[0], 0.F, 0.F, 1.F };
                    glUniform4fv(location, 1, v);
                }
                else if (Image::Type::LA_F32 == value.getType())
                {
                    const Image::F32_T* p = reinterpret_cast<const Image::F32_T*>(value.getData());
                    const GLfloat v[] = { p[0], p[1], 0.F, 1.F };
                    glUniform4fv(location, 1, v);
                }
                else if (Image::Type::RGB_F32 == value.getType())
                {
                    const Image::F32_T* p = reinterpret_cast<const Image::F32_T*>(value.getData());
                    const GLfloat v[] = { p[0], p[1], p[2], 1.F };
                    glUniform4fv(location, 1, v);
                }
                else if (Image::Type::RGBA_F32 == value.getType())
                {
                    glUniform4fv(location, 1, reinterpret_cast<const GLfloat*>(value.getData()));
                }
                else
                {
                    auto color = value.convert(Image::Type::RGBA_F32);
                    glUniform4fv(location, 1, reinterpret_cast<const GLfloat*>(color.getData()));
                }
            }

            void Shader::setUniformF(GLint location, const Image::Color& value)
            {
                if (Image::Type::L_F32 == value.getType())
                {
                    glUniform1fv(location, 1, reinterpret_cast<const GLfloat*>(value.getData()));
                }
                else if (Image::Type::LA_F32 == value.getType())
                {
                    glUniform2fv(location, 1, reinterpret_cast<const GLfloat*>(value.getData()));
                }
                else if (Image::Type::RGB_F32 == value.getType())
                {
                    glUniform3fv(location, 1, reinterpret_cast<const GLfloat*>(value.getData()));
                }
                else if (Image::Type::RGBA_F32 == value.getType())
                {
                    glUniform4fv(location, 1, reinterpret_cast<const GLfloat*>(value.getData()));
                }
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
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniform1i(location, value);
            }

            void Shader::setUniform(const std::string & name, float value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniform1f(location, value);
            }

            void Shader::setUniform(const std::string & name, const glm::vec2 & value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniform2fv(location, 1, &value[0]);
            }

            void Shader::setUniform(const std::string & name, const glm::vec3 & value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniform3fv(location, 1, &value[0]);
            }

            void Shader::setUniform(const std::string & name, const glm::vec4 & value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniform4fv(location, 1, &value[0]);
            }

            void Shader::setUniform(const std::string & name, const glm::mat3x3 & value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
            }

            void Shader::setUniform(const std::string & name, const glm::mat4x4 & value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
            }

            void Shader::setUniform(const std::string& name, const Image::Color& value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                auto color = value.convert(Image::Type::RGBA_F32);
                glUniform4fv(location, 1, reinterpret_cast<const GLfloat*>(color.getData()));
            }

            void Shader::setUniformF(const std::string& name, const Image::Color& value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                if (Image::Type::L_F32 == value.getType())
                {
                    glUniform1fv(location, 1, reinterpret_cast<const GLfloat*>(value.getData()));
                }
                else if (Image::Type::LA_F32 == value.getType())
                {
                    glUniform2fv(location, 1, reinterpret_cast<const GLfloat*>(value.getData()));
                }
                else if (Image::Type::RGB_F32 == value.getType())
                {
                    glUniform3fv(location, 1, reinterpret_cast<const GLfloat*>(value.getData()));
                }
                else if (Image::Type::RGBA_F32 == value.getType())
                {
                    glUniform4fv(location, 1, reinterpret_cast<const GLfloat*>(value.getData()));
                }
            }

            void Shader::setUniform(const std::string& name, const float value[4])
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniform4fv(location, 1, value);
            }

            void Shader::setUniform(const std::string& name, const std::vector<int>& value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniform1iv(location, value.size(), &value[0]);
            }

            void Shader::setUniform(const std::string& name, const std::vector<float>& value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniform1fv(location, value.size(), &value[0]);
            }

            void Shader::setUniform(const std::string& name, const std::vector<glm::vec3>& value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniform3fv(location, value.size(), &value[0][0]);
            }

            void Shader::setUniform(const std::string& name, const std::vector<glm::vec4>& value)
            {
                const GLint location = glGetUniformLocation(_program, name.c_str());
                glUniform4fv(location, value.size(), &value[0][0]);
            }

            void Shader::bind()
            {
                glUseProgram(_program);
            }

        } // namespace OpenGL
    } // namespace AV
} // namespace djv
