// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvGL/Shader.h>

#include <djvImage/Color.h>

#include <djvSystem/Path.h>
#include <djvSystem/FileIO.h>

#include <djvCore/String.h>
#include <djvCore/StringFormat.h>

using namespace djv::Core;

namespace djv
{
    namespace GL
    {
        ShaderError::ShaderError(const std::string& what) :
            std::runtime_error(what)
        {}
        
        void Shader::_init()
        {
            _vertex = glCreateShader(GL_VERTEX_SHADER);
            if (!_vertex)
            {
                //! \todo How can we translate this?
                throw ShaderError(DJV_TEXT("error_gl_vertex_shader_creation"));
            }
            const char * src = _vertexSource.second.c_str();
            glShaderSource(_vertex, 1, &src, NULL);
            glCompileShader(_vertex);
            int success = 0;
            char infoLog[String::cStringLength];
            glGetShaderiv(_vertex, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(_vertex, String::cStringLength, NULL, infoLog);
                throw ShaderError(String::Format("{0}: {1}").
                    arg(_vertexSource.first).
                    arg(infoLog));
            }

            _fragment = glCreateShader(GL_FRAGMENT_SHADER);
            if (!_fragment)
            {
                //! \todo How can we translate this?
                throw ShaderError(DJV_TEXT("error_gl_fragment_shader_creation"));
            }
            src = _fragmentSource.second.c_str();
            glShaderSource(_fragment, 1, &src, NULL);
            glCompileShader(_fragment);
            glGetShaderiv(_fragment, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(_fragment, String::cStringLength, NULL, infoLog);
                throw ShaderError(String::Format("{0}: {1}").
                    arg(_fragmentSource.first).
                    arg(infoLog));
            }

            _program = glCreateProgram();
            glAttachShader(_program, _vertex);
            glAttachShader(_program, _fragment);
            glLinkProgram(_program);
            glGetProgramiv(_program, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(_program, String::cStringLength, NULL, infoLog);
                throw ShaderError(String::Format("{0}: {1}").
                    arg(_vertexSource.first).
                    arg(infoLog));
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

        std::shared_ptr<Shader> Shader::create(
            const std::string& vertexSource,
            const std::string& fragmentSource)
        {
            auto out = std::shared_ptr<Shader>(new Shader);
            out->_vertexSource.second = vertexSource;
            out->_fragmentSource.second = fragmentSource;
            out->_init();
            return out;
        }

        std::shared_ptr<Shader> Shader::create(
            const System::File::Path& vertexSource,
            const System::File::Path& fragmentSource)
        {
            auto out = std::shared_ptr<Shader>(new Shader);
            try
            {
                auto fileIO = System::File::IO::create();
                fileIO->open(std::string(vertexSource), System::File::Mode::Read);
                out->_vertexSource.second = System::File::readContents(fileIO);
                out->_vertexSource.first = vertexSource.get();
                fileIO->open(std::string(fragmentSource), System::File::Mode::Read);
                out->_fragmentSource.second = System::File::readContents(fileIO);
                out->_fragmentSource.first = fragmentSource.get();
                out->_init();
            }
            catch (const std::exception& e)
            {
                throw ShaderError(e.what());
            }
            return out;
        }

        const std::string& Shader::getVertexName() const
        {
            return _vertexSource.first;
        }

        const std::string& Shader::getVertexSource() const
        {
            return _vertexSource.second;
        }

        const std::string& Shader::getFragmentName() const
        {
            return _fragmentSource.first;
        }

        const std::string& Shader::getFragmentSource() const
        {
            return _fragmentSource.second;
        }

        void Shader::setVertexName(const std::string& value)
        {
            _vertexSource.first = value;
        }
        
        void Shader::setFragmentName(const std::string& value)
        {
            _fragmentSource.first = value;
        }

        GLuint Shader::getProgram() const
        {
            return _program;
        }

        void Shader::bind()
        {
            glUseProgram(_program);
        }

        void Shader::setUniform(GLint location, int value)
        {
            glUniform1i(location, value);
        }

        void Shader::setUniform(GLint location, float value)
        {
            glUniform1f(location, value);
        }

        void Shader::setUniform(GLint location, const glm::vec2& value)
        {
            glUniform2fv(location, 1, &value[0]);
        }

        void Shader::setUniform(GLint location, const glm::vec3& value)
        {
            glUniform3fv(location, 1, &value[0]);
        }

        void Shader::setUniform(GLint location, const glm::vec4& value)
        {
            glUniform4fv(location, 1, &value[0]);
        }

        void Shader::setUniform(GLint location, const glm::mat3x3& value)
        {
            glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
        }

        void Shader::setUniform(GLint location, const glm::mat4x4& value)
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
                    p[0] / static_cast<GLfloat>(Image::U8Range.getMax()),
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
                    p[0] / static_cast<GLfloat>(Image::U8Range.getMax()),
                    0.F,
                    0.F,
                    p[1] / static_cast<GLfloat>(Image::U8Range.getMax())
                };
                glUniform4fv(location, 1, v);
            }
            else if (Image::Type::RGB_U8 == value.getType())
            {
                const Image::U8_T* p = reinterpret_cast<const Image::U8_T*>(value.getData());
                const GLfloat v[] =
                {
                    p[0] / static_cast<GLfloat>(Image::U8Range.getMax()),
                    p[1] / static_cast<GLfloat>(Image::U8Range.getMax()),
                    p[2] / static_cast<GLfloat>(Image::U8Range.getMax()),
                    1.F
                };
                glUniform4fv(location, 1, v);
            }
            else if (Image::Type::RGBA_U8 == value.getType())
            {
                const Image::U8_T* p = reinterpret_cast<const Image::U8_T*>(value.getData());
                const GLfloat v[] =
                {
                    p[0] / static_cast<GLfloat>(Image::U8Range.getMax()),
                    p[1] / static_cast<GLfloat>(Image::U8Range.getMax()),
                    p[2] / static_cast<GLfloat>(Image::U8Range.getMax()),
                    p[3] / static_cast<GLfloat>(Image::U8Range.getMax())
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

        void Shader::setUniform(const std::string& name, int value)
        {
            const GLint location = glGetUniformLocation(_program, name.c_str());
            glUniform1i(location, value);
        }

        void Shader::setUniform(const std::string& name, float value)
        {
            const GLint location = glGetUniformLocation(_program, name.c_str());
            glUniform1f(location, value);
        }

        void Shader::setUniform(const std::string& name, const glm::vec2& value)
        {
            const GLint location = glGetUniformLocation(_program, name.c_str());
            glUniform2fv(location, 1, &value[0]);
        }

        void Shader::setUniform(const std::string& name, const glm::vec3& value)
        {
            const GLint location = glGetUniformLocation(_program, name.c_str());
            glUniform3fv(location, 1, &value[0]);
        }

        void Shader::setUniform(const std::string& name, const glm::vec4& value)
        {
            const GLint location = glGetUniformLocation(_program, name.c_str());
            glUniform4fv(location, 1, &value[0]);
        }

        void Shader::setUniform(const std::string& name, const glm::mat3x3& value)
        {
            const GLint location = glGetUniformLocation(_program, name.c_str());
            glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
        }

        void Shader::setUniform(const std::string& name, const glm::mat4x4& value)
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

    } // namespace GL
} // namespace djv
