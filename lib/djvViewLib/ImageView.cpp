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
#include <djvAV/OpenGL.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/OpenGLTexture.h>
#include <djvAV/Shader.h>

#include <djvCore/Path.h>
#include <djvCore/Vector.h>
#include <djvCore/BBox.h>

#include <QVBoxLayout>

#include <glm/gtc/matrix_transform.hpp>

#include <condition_variable>

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            const size_t timeout = 10;

            class VBO
            {
            public:
                VBO(const Core::BBox2i &);
                ~VBO();

                const Core::BBox2i & getBBox() const { return _bbox; }
                size_t getVertexByteCount() const { return _vertexByteCount; }
                GLuint getID() const { return _vbo; }

            private:
                Core::BBox2i _bbox;
                size_t _vertexByteCount = 0;
                GLuint _vbo = 0;
            };

            VBO::VBO(const Core::BBox2i & bbox) :
                _bbox(bbox)
            {
                _vertexByteCount = 8 + 8;
                std::vector<float> data =
                {
                    static_cast<float>(bbox.min.x),
                    static_cast<float>(bbox.min.y),
                    0.f,
                    1.f,
                    static_cast<float>(bbox.max.x),
                    static_cast<float>(bbox.min.y),
                    1.f,
                    1.f,
                    static_cast<float>(bbox.max.x),
                    static_cast<float>(bbox.max.y),
                    1.f,
                    0.f,
                    static_cast<float>(bbox.max.x),
                    static_cast<float>(bbox.max.y),
                    1.f,
                    0.f,
                    static_cast<float>(bbox.min.x),
                    static_cast<float>(bbox.max.y),
                    0.f,
                    0.f,
                    static_cast<float>(bbox.min.x),
                    static_cast<float>(bbox.min.y),
                    0.f,
                    1.f
                };
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glGenBuffers(1, &_vbo);
                glFuncs->glBindBuffer(GL_ARRAY_BUFFER, _vbo);
                glFuncs->glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(data.size() * _vertexByteCount), data.data(), GL_STATIC_DRAW);
            }

            VBO::~VBO()
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glDeleteBuffers(1, &_vbo);
            }

            class VAO
            {
            public:
                VAO(size_t vertexByteCount, GLuint vbo);
                ~VAO();

                void bind();
                void draw(size_t start, size_t count);

            private:
                GLuint _vao = 0;
            };

            VAO::VAO(size_t vertexByteCount, GLuint vbo)
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glGenVertexArrays(1, &_vao);
                glFuncs->glBindVertexArray(_vao);
                glFuncs->glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glFuncs->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)0);
                glFuncs->glEnableVertexAttribArray(0);
                glFuncs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexByteCount), (GLvoid*)8);
                glFuncs->glEnableVertexAttribArray(1);
            }

            VAO::~VAO()
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glDeleteVertexArrays(1, &_vao);
            }

            void VAO::bind()
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glBindVertexArray(_vao);
            }

            void VAO::draw(size_t start, size_t count)
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glDrawArrays(GL_TRIANGLES, static_cast<GLsizei>(start * 6), static_cast<GLsizei>(count * 6));
            }

        } // namespace

        struct ImageView::Private
        {
            std::weak_ptr<Context> context;
            std::shared_ptr<AV::Image> image;
            bool imageChanged = false;
            std::shared_ptr<AV::Shader> shader;
            std::shared_ptr<AV::OpenGL::Shader> openGLShader;
            std::shared_ptr<AV::OpenGL::Texture> openGLTexture;
            std::unique_ptr<VBO> openGLVBO;
            std::unique_ptr<VAO> openGLVAO;
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
                if (_p->imageChanged)
                {
                    _p->imageChanged = false;
                    if (!_p->openGLTexture || (_p->openGLTexture && (_p->image->getInfo() != _p->openGLTexture->getInfo())))
                    {
                        _p->openGLTexture = AV::OpenGL::Texture::create(_p->image->getInfo());
                    }
                    _p->openGLTexture->copy(_p->image);
                }
                _p->openGLTexture->bind();

                if (!_p->openGLVBO || (_p->openGLVBO && (_p->image->getSize() != _p->openGLVBO->getBBox().getSize())))
                {
                    _p->openGLVBO.reset(new VBO(Core::BBox2i(0, 0, _p->image->getWidth() - 1, _p->image->getHeight() - 1)));
                    _p->openGLVAO.reset(new VAO(_p->openGLVBO->getVertexByteCount(), _p->openGLVBO->getID()));
                }

                _p->openGLShader->bind();
                _p->openGLShader->setUniform("textureSampler", 0);
                const auto viewMatrix = glm::ortho(
                    0.f,
                    static_cast<float>(width()),
                    0.f,
                    static_cast<float>(height()),
                    -1.f,
                    1.f);
                _p->openGLShader->setUniform("transform.mvp", viewMatrix);

                glFuncs->glActiveTexture(GL_TEXTURE0);
                _p->openGLVAO->draw(0, 1);
            }
        }

    } // namespace ViewLib
} // namespace djv

