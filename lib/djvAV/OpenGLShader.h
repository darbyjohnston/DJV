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

#pragma once

#include <djvAV/OpenGL.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

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
            //! This class provides an OpenGL shader.
            class Shader
            {
                DJV_NON_COPYABLE(Shader);
                void _init(const std::shared_ptr<Render::Shader> &);
                Shader();

            public:
                ~Shader();

                //! Throws:
                //! - std::exception
                static std::shared_ptr<Shader> create(const std::shared_ptr<Render::Shader> &);

                gl::GLuint getProgram() const;

                void setUniform(gl::GLint, int);
                void setUniform(gl::GLint, float);
                void setUniform(gl::GLint, const glm::vec2 &);
                void setUniform(gl::GLint, const glm::vec3 &);
                void setUniform(gl::GLint, const glm::vec4 &);
                void setUniform(gl::GLint, const glm::mat3x3 &);
                void setUniform(gl::GLint, const glm::mat4x4 &);
                void setUniform(gl::GLint, const Image::Color &);

                void setUniform(const std::string &, int);
                void setUniform(const std::string &, float);
                void setUniform(const std::string &, const glm::vec2 &);
                void setUniform(const std::string &, const glm::vec3 &);
                void setUniform(const std::string &, const glm::vec4 &);
                void setUniform(const std::string &, const glm::mat3x3 &);
                void setUniform(const std::string &, const glm::mat4x4 &);
                void setUniform(const std::string &, const Image::Color &);

                void bind();

            private:
                std::shared_ptr<Render::Shader> _shader;
                gl::GLuint _vertex = 0;
                gl::GLuint _fragment = 0;
                gl::GLuint _program = 0;
            };

        } // namespace OpenGL
    } // namespace AV
} // namespace djv

