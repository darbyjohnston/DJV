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
            if (image == _p->image)
                return;
            _p->image = image;
            _p->imageChanged = true;
            update();
        }

        void ImageView::initializeGL()
        {
            if (auto context = _p->context.lock())
            {
                try
                {
                    _p->shader = AV::Shader::create(
                        context->getResourcePath(Core::ResourcePath::ShadersDirectory, "djvViewLibImageViewVertex.glsl"),
                        context->getResourcePath(Core::ResourcePath::ShadersDirectory, "djvViewLibImageViewFragment.glsl"));
                    _p->openGLShader = AV::OpenGL::Shader::create(_p->shader);
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

            if (_p->image)
            {
                if (!_p->openGLTexture || (_p->openGLTexture && (_p->openGLTexture->getInfo() != _p->image->getInfo())))
                {
                    _p->openGLTexture = AV::OpenGL::Texture::create(_p->image->getInfo());
                }
                if (_p->imageChanged)
                {
                    _p->imageChanged = false;
                    _p->openGLTexture->copy(*_p->image);
                }
                _p->openGLTexture->bind();

                if (!_p->openGLVBO)
                {
                    AV::Shape::Square square;
                    AV::TriangleMesh mesh;
                    square.triangulate(mesh);
                    _p->openGLVBO = AV::OpenGL::VBO::create(2, 3, AV::OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                    _p->openGLVBO->copy(AV::OpenGL::VBO::convert(mesh, _p->openGLVBO->getType()));
                    _p->openGLVAO = AV::OpenGL::VAO::create(_p->openGLVBO->getType(), _p->openGLVBO->getID());
                }
                _p->openGLVAO->bind();

                _p->openGLShader->bind();
                _p->openGLShader->setUniform("textureSampler", 0);
                glm::mat4x4 modelMatrix(1);
                modelMatrix = glm::rotate(modelMatrix, Core::Math::deg2rad(-90.f), glm::vec3(1.f, 0.f, 0.f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(_p->image->getWidth(), 0.f, _p->image->getHeight()));
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
                _p->openGLShader->setUniform("transform.mvp", projectionMatrix * viewMatrix * modelMatrix);

                glFuncs->glActiveTexture(GL_TEXTURE0);
                _p->openGLVAO->draw(0, 6);
            }
        }

    } // namespace ViewLib
} // namespace djv

