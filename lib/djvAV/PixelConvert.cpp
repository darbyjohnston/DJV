//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/PixelConvert.h>

#include <djvAV/OpenGLMesh.h>
#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/OpenGLTexture.h>
#include <djvAV/Shader.h>
#include <djvAV/Shape.h>
#include <djvAV/System.h>
#include <djvAV/TriangleMesh.h>

#include <djvCore/Context.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace djv::Core;

using namespace gl;

namespace djv
{
    namespace AV
    {
	    namespace Pixel
	    {
            struct Convert::Private
            {
                glm::ivec2 size;
                AV::Pixel::Mirror mirror;
                std::shared_ptr<OpenGL::OffscreenBuffer> offscreenBuffer;
                std::shared_ptr<AV::OpenGL::Texture> texture;
                std::shared_ptr<AV::OpenGL::VBO> vbo;
                std::shared_ptr<AV::OpenGL::VAO> vao;
                std::shared_ptr<AV::OpenGL::Shader> shader;
                glm::mat4x4 mvp;
            };

            void Convert::_init(Context * context)
            {
                DJV_PRIVATE_PTR();
                const auto shaderPath = context->getPath(Core::FileSystem::ResourcePath::ShadersDirectory);
                p.shader = AV::OpenGL::Shader::create(AV::Shader::create(
                    FileSystem::Path(shaderPath, "djvAVPixelConvertVertex.glsl"),
                    FileSystem::Path(shaderPath, "djvAVPixelConvertFragment.glsl")));
            }

            Convert::Convert() :
                _p(new Private)
            {}

            Convert::~Convert()
            {}

            std::shared_ptr<Convert> Convert::create(Context * context)
            {
                auto out = std::shared_ptr<Convert>(new Convert);
                out->_init(context);
                return out;
            }

            void Convert::process(const Data & data, const Info & info, Data & out)
            {
                DJV_PRIVATE_PTR();
                if (!p.offscreenBuffer || (p.offscreenBuffer && info != p.offscreenBuffer->getInfo()))
                {
                    p.offscreenBuffer = OpenGL::OffscreenBuffer::create(info);
                }
                const OpenGL::OffscreenBufferBinding binding(p.offscreenBuffer);

                if (!p.texture || (p.texture && data.getInfo() != p.texture->getInfo()))
                {
                    p.texture = OpenGL::Texture::create(data.getInfo());
                }
                p.texture->bind();
                p.texture->copy(data);

                p.shader->bind();
                p.shader->setUniform("textureSampler", 0);
                
                if (info.size != p.size)
                {
                    p.size = info.size;
                    glm::mat4x4 modelMatrix(1);
                    modelMatrix = glm::rotate(modelMatrix, Core::Math::deg2rad(-90.f), glm::vec3(1.f, 0.f, 0.f));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(info.size.x, 0.f, info.size.y));
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(.5f, 0.f, .5f));
                    glm::mat4x4 viewMatrix(1);
                    glm::mat4x4 projectionMatrix(1);
                    projectionMatrix = glm::ortho(
                        0.f,
                        static_cast<float>(info.size.x),
                        0.f,
                        static_cast<float>(info.size.y),
                        -1.f,
                        1.f);
                    p.mvp = projectionMatrix * viewMatrix * modelMatrix;
                }
                p.shader->setUniform("transform.mvp", p.mvp);

                if (!p.vbo || (p.vbo && info.layout.mirror != p.mirror))
                {
                    p.mirror = info.layout.mirror;
                    AV::Shape::Square square;
                    AV::Mesh::TriangleMesh mesh;
                    square.triangulate(mesh);
                    if (p.mirror.x)
                    {
                        auto tmp = mesh.t[0].x;
                        mesh.t[0].x = mesh.t[1].x;
                        mesh.t[1].x = tmp;
                        tmp = mesh.t[2].x;
                        mesh.t[2].x = mesh.t[3].x;
                        mesh.t[3].x = tmp;
                    }
                    if (p.mirror.y)
                    {
                        auto tmp = mesh.t[0].y;
                        mesh.t[0].y = mesh.t[2].y;
                        mesh.t[2].y = tmp;
                        tmp = mesh.t[1].y;
                        mesh.t[1].y = mesh.t[3].y;
                        mesh.t[3].y = tmp;
                    }
                    p.vbo = AV::OpenGL::VBO::create(2, 3, AV::OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                    p.vbo->copy(AV::OpenGL::VBO::convert(mesh, p.vbo->getType()));
                    p.vao = AV::OpenGL::VAO::create(p.vbo->getType(), p.vbo->getID());
                }
                p.vao->bind();

                glViewport(0, 0, info.size.x, info.size.y);
                glClearColor(0.f, 0.f, 0.f, 0.f);
                glClear(GL_COLOR_BUFFER_BIT);
                glActiveTexture(GL_TEXTURE0);

                p.vao->draw(0, 6);

                glPixelStorei(GL_PACK_ALIGNMENT, 1);
                glReadPixels(
                    0, 0, info.size.x, info.size.y,
                    info.getGLFormat(),
                    info.getGLType(),
                    out.getData());
            }

	    } // namespace Keycode
	} // namespace AV
} // namespace djv
