// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/OpenGL.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace djv
{
    namespace Core
    {
        class Path;
    
    } // namespace Core

    namespace AV
    {
        namespace Image
        {
            class Color;

        } // namespace Image

        namespace Render
        {
            class Shader;

        } // namespace Render

        namespace OpenGL
        {
            //! This class provides an OpenGL offscreen buffer error.
            class ShaderError : public std::runtime_error
            {
            public:
                explicit ShaderError(const std::string&);
            };
            
            //! This class provides an OpenGL shader.
            class Shader
            {
                DJV_NON_COPYABLE(Shader);
                void _init(const std::shared_ptr<Render::Shader>&);
                Shader();

            public:
                ~Shader();

                //! Throws:
                //! - ShaderError
                static std::shared_ptr<Shader> create(const std::shared_ptr<Render::Shader>&);

                GLuint getProgram() const;

                void bind();

                void setUniform(GLint, int);
                void setUniform(GLint, float);
                void setUniform(GLint, const glm::vec2&);
                void setUniform(GLint, const glm::vec3&);
                void setUniform(GLint, const glm::vec4&);
                void setUniform(GLint, const glm::mat3x3&);
                void setUniform(GLint, const glm::mat4x4&);
                void setUniform(GLint, const Image::Color&);
                void setUniformF(GLint, const Image::Color&);
                void setUniform(GLint, const float [4]);

                void setUniform(GLint, const std::vector<int>&);
                void setUniform(GLint, const std::vector<float>&);
                void setUniform(GLint, const std::vector<glm::vec3>&);
                void setUniform(GLint, const std::vector<glm::vec4>&);

                void setUniform(const std::string&, int);
                void setUniform(const std::string&, float);
                void setUniform(const std::string&, const glm::vec2&);
                void setUniform(const std::string&, const glm::vec3&);
                void setUniform(const std::string&, const glm::vec4&);
                void setUniform(const std::string&, const glm::mat3x3&);
                void setUniform(const std::string&, const glm::mat4x4&);
                void setUniform(const std::string&, const Image::Color&);
                void setUniformF(const std::string&, const Image::Color&);
                void setUniform(const std::string&, const float [4]);

                void setUniform(const std::string&, const std::vector<int>&);
                void setUniform(const std::string&, const std::vector<float>&);
                void setUniform(const std::string&, const std::vector<glm::vec3>&);
                void setUniform(const std::string&, const std::vector<glm::vec4>&);

            private:
                std::shared_ptr<Render::Shader> _shader;
                GLuint _vertex = 0;
                GLuint _fragment = 0;
                GLuint _program = 0;
            };

        } // namespace OpenGL
    } // namespace AV
} // namespace djv

