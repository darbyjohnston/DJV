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

#include <djvViewLib/ImageView.h>

#include <djvViewLib/Context.h>

#include <djvAV/Image.h>
#include <djvAV/OpenGLMesh.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/OpenGLTexture.h>
#include <djvAV/Shader.h>
#include <djvAV/Shape.h>
#include <djvAV/TriangleMesh.h>

#include <djvCore/Path.h>
#include <djvCore/Vector.h>
#include <djvCore/BBox.h>

#include <QVBoxLayout>

#include <glm/gtc/matrix_transform.hpp>

namespace djv
{
    namespace ViewLib
    {
        struct ImageView::Private
        {
            std::weak_ptr<Context> context;
            std::shared_ptr<AV::Image> image;
            bool imageChanged = false;
            std::shared_ptr<AV::Shader> shader;
            std::shared_ptr<AV::OpenGL::Shader> openGLShader;
            std::shared_ptr<AV::OpenGL::Texture> openGLTexture;
            std::shared_ptr<AV::OpenGL::VBO> openGLVBO;
            std::shared_ptr<AV::OpenGL::VAO> openGLVAO;
        };
        
        ImageView::ImageView(const std::shared_ptr<Context> & context, QWidget * parent) :
            QOpenGLWidget(parent),
            _p(new Private)
        {
            _p->context = context;
        }
        
        ImageView::~ImageView()
        {
            makeCurrent();
        }

        void ImageView::setImage(const std::shared_ptr<AV::Image> & image)
        {
            DJV_PRIVATE_PTR();
            if (image == p.image)
                return;
            p.image = image;
            p.imageChanged = true;
            update();
        }

        void ImageView::initializeGL()
        {
            DJV_PRIVATE_PTR();
            if (auto context = p.context.lock())
            {
                try
                {
                    p.shader = AV::Shader::create(
                        context->getResourcePath(Core::ResourcePath::ShadersDirectory, "djvViewLibImageViewVertex.glsl"),
                        context->getResourcePath(Core::ResourcePath::ShadersDirectory, "djvViewLibImageViewFragment.glsl"));
                    p.openGLShader = AV::OpenGL::Shader::create(p.shader);
                }
                catch (const std::exception& e)
                {
                    context->log("djv::ViewLib::ImageView", e.what(), Core::LogLevel::Error);
                }
            }
        }

        void ImageView::resizeGL(int w, int h)
        {}

        void ImageView::paintGL()
        {
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            glFuncs->glClearColor(0.f, 0.f, 0.f, 0.f);
            glFuncs->glClear(GL_COLOR_BUFFER_BIT);

            DJV_PRIVATE_PTR();
            if (p.image)
            {
                if (!p.openGLTexture || (p.openGLTexture && (p.openGLTexture->getInfo() != p.image->getInfo())))
                {
                    p.openGLTexture = AV::OpenGL::Texture::create(p.image->getInfo());
                }
                if (p.imageChanged)
                {
                    p.imageChanged = false;
                    p.openGLTexture->copy(*p.image);
                }
                p.openGLTexture->bind();

                if (!p.openGLVBO)
                {
                    AV::Shape::Square square;
                    AV::TriangleMesh mesh;
                    square.triangulate(mesh);
                    p.openGLVBO = AV::OpenGL::VBO::create(2, 3, AV::OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                    p.openGLVBO->copy(AV::OpenGL::VBO::convert(mesh, p.openGLVBO->getType()));
                    p.openGLVAO = AV::OpenGL::VAO::create(p.openGLVBO->getType(), p.openGLVBO->getID());
                }
                p.openGLVAO->bind();

                p.openGLShader->bind();
                p.openGLShader->setUniform("textureSampler", 0);
                glm::mat4x4 modelMatrix(1);
                modelMatrix = glm::rotate(modelMatrix, Core::Math::deg2rad(-90.f), glm::vec3(1.f, 0.f, 0.f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(p.image->getWidth(), 0.f, p.image->getHeight()));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(.5f, 0.f, .5f));
                glm::mat4x4 viewMatrix(1);
                glm::mat4x4 projectionMatrix(1);
                projectionMatrix = glm::ortho(
                    0.f,
                    static_cast<float>(width()),
                    0.f,
                    static_cast<float>(height()),
                    -1.f,
                    1.f);
                p.openGLShader->setUniform("transform.mvp", projectionMatrix * viewMatrix * modelMatrix);

                glFuncs->glActiveTexture(GL_TEXTURE0);
                p.openGLVAO->draw(0, 6);
            }
        }

    } // namespace ViewLib
} // namespace djv

