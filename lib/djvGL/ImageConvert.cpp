// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvGL/ImageConvert.h>

#include <djvGL/Mesh.h>
#include <djvGL/OffscreenBuffer.h>
#include <djvGL/Shader.h>
#include <djvGL/Texture.h>

#include <djvSystem/TextSystem.h>
#include <djvSystem/ResourceSystem.h>

#include <djvGeom/Shape.h>
#include <djvGeom/TriangleMesh.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace djv::Core;

namespace djv
{
    namespace GL
    {
        struct ImageConvert::Private
        {
            std::shared_ptr<System::TextSystem> textSystem;
            std::shared_ptr<System::ResourceSystem> resourceSystem;
            Image::Size size;
            Image::Mirror mirror;
            std::shared_ptr<OffscreenBuffer> offscreenBuffer;
            std::shared_ptr<Texture2D> texture;
            std::shared_ptr<VBO> vbo;
            std::shared_ptr<VAO> vao;
            std::shared_ptr<Shader> shader;
            glm::mat4x4 mvp = glm::mat4x4(1.F);
        };

        void ImageConvert::_init(
            const std::shared_ptr<System::TextSystem>& textSystem,
            const std::shared_ptr<System::ResourceSystem>& resourceSystem)
        {
            DJV_PRIVATE_PTR();
            p.textSystem = textSystem;
            p.resourceSystem = resourceSystem;
            const auto shaderPath = resourceSystem->getPath(System::File::ResourcePath::Shaders);
            p.shader = Shader::create(
                System::File::Path(shaderPath, "djvImageConvertVertex.glsl"),
                System::File::Path(shaderPath, "djvImageConvertFragment.glsl"));
        }

        ImageConvert::ImageConvert() :
            _p(new Private)
        {}

        ImageConvert::~ImageConvert()
        {}

        std::shared_ptr<ImageConvert> ImageConvert::create(
            const std::shared_ptr<System::TextSystem>& textSystem,
            const std::shared_ptr<System::ResourceSystem>& resourceSystem)
        {
            auto out = std::shared_ptr<ImageConvert>(new ImageConvert);
            out->_init(textSystem, resourceSystem);
            return out;
        }

        void ImageConvert::process(const Image::Data& data, const Image::Info& info, Image::Data& out)
        {
            DJV_PRIVATE_PTR();
            bool create = !p.offscreenBuffer;
            create |= p.offscreenBuffer && info.size != p.offscreenBuffer->getSize();
            create |= p.offscreenBuffer && info.type != p.offscreenBuffer->getColorType();
            if (create)
            {
                p.offscreenBuffer = OffscreenBuffer::create(info.size, info.type, p.textSystem);
            }
            const OffscreenBufferBinding binding(p.offscreenBuffer);

            if (!p.texture || (p.texture && data.getInfo() != p.texture->getInfo()))
            {
                p.texture = Texture2D::create(data.getInfo());
            }
            p.texture->bind();
            p.texture->copy(data);

            p.shader->bind();
            p.shader->setUniform("textureSampler", 0);
            
            if (info.size != p.size)
            {
                p.size = info.size;
                glm::mat4x4 modelMatrix(1);
                modelMatrix = glm::rotate(modelMatrix, Math::deg2rad(90.F), glm::vec3(1.F, 0.F, 0.F));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(info.size.w, 0.F, info.size.h));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(.5F, 0.F, -.5F));
                glm::mat4x4 viewMatrix(1);
                glm::mat4x4 projectionMatrix(1);
                projectionMatrix = glm::ortho(
                    0.F,
                    static_cast<float>(info.size.w) - 1.F,
                    0.F,
                    static_cast<float>(info.size.h) - 1.F,
                    -1.F,
                    1.F);
                p.mvp = projectionMatrix * viewMatrix * modelMatrix;
            }
            p.shader->setUniform("transform.mvp", p.mvp);

            if (!p.vbo || (p.vbo && data.getLayout().mirror != p.mirror))
            {
                p.mirror = data.getLayout().mirror;
                Geom::Square square;
                Geom::TriangleMesh mesh;
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
                p.vbo = VBO::create(2 * 3, VBOType::Pos3_F32_UV_U16);
                p.vbo->copy(VBO::convert(mesh, p.vbo->getType()));
                p.vao = VAO::create(p.vbo->getType(), p.vbo->getID());
            }
            p.vao->bind();

            glViewport(0, 0, info.size.w, info.size.h);
            glClearColor(0.F, 0.F, 0.F, 0.F);
            glClear(GL_COLOR_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);

            p.vao->draw(GL_TRIANGLES, 0, 6);

            glPixelStorei(GL_PACK_ALIGNMENT, 1);
#if !defined(DJV_GL_ES2)
            glPixelStorei(GL_PACK_SWAP_BYTES, info.layout.endian != Memory::getEndian());
#endif // DJV_GL_ES2
            glReadPixels(
                0, 0, info.size.w, info.size.h,
                info.getGLFormat(),
                info.getGLType(),
                out.getData());
        }

    } // namespace GL
} // namespace djv
