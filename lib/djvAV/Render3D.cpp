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

#include <djvAV/Render3D.h>

#include <djvAV/GLFWSystem.h>
#include <djvAV/OpenGLMesh.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/ShaderSystem.h>
#include <djvAV/TextureAtlas.h>

#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>
#include <djvCore/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            namespace
            {
                //! \todo Should this be configurable?
                const uint8_t  textureAtlasCount = 4;
                const uint16_t textureAtlasSize  = 8192;

                struct Primitive
                {
                    glm::mat4x4                 xform;
                    GLenum                      type        = GL_TRIANGLES;
                    size_t                      vaoOffset   = 0;
                    size_t                      vaoSize     = 0;
                    std::shared_ptr<IMaterial>  material;
                };

            } // namespace

            void IMaterial::_init(
                const std::string& vertex,
                const std::string& fragment,
                const std::shared_ptr<Context>& context)
            {
                auto shaderSystem = context->getSystemT<AV::Render::ShaderSystem>();
                _shader = shaderSystem->getShader(vertex, fragment);
                auto program = _shader->getProgram();
                _mvpLoc = glGetUniformLocation(program, "transform.mvp");
            }

            IMaterial::IMaterial()
            {}

            IMaterial::~IMaterial()
            {}

            void IMaterial::bind(const glm::mat4x4& mvp)
            {
                _shader->bind();
                _shader->setUniform(_mvpLoc, mvp);
            }

            void DefaultMaterial::_init(const std::shared_ptr<Context>& context)
            {
                IMaterial::_init("djvAVRender3DDefaultVertex.glsl", "djvAVRender3DDefaultFragment.glsl", context);
            }

            DefaultMaterial::DefaultMaterial()
            {}

            DefaultMaterial::~DefaultMaterial()
            {}

            std::shared_ptr<DefaultMaterial> DefaultMaterial::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<DefaultMaterial>(new DefaultMaterial);
                out->_init(context);
                return out;
            }

            void DefaultMaterial::bind(const glm::mat4x4& mvp)
            {
                IMaterial::bind(mvp);
            }

            struct Render::Private
            {
                RenderOptions                               options;
                std::shared_ptr<IMaterial>                  currentMaterial;
                std::vector<Primitive*>                     primitives;
                std::shared_ptr<AV::Render::TextureAtlas>   textureAtlas;
                std::vector<uint8_t>                        vboData;
                size_t                                      vboDataSize     = 0;
                std::shared_ptr<OpenGL::VBO>                vbo;
                std::shared_ptr<OpenGL::VAO>                vao;

                std::shared_ptr<Time::Timer>                statsTimer;

                void updateVBODataSize(size_t value);
            };

            void Render::_init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("djv::AV::Render3D::Render", context);
                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<AV::GLFW::System>());

                GLint maxTextureUnits = 0;
                GLint maxTextureSize = 0;
                glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
                glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
                {
                    auto logSystem = context->getSystemT<LogSystem>();
                    std::stringstream ss;
                    ss << "Maximum OpenGL texture units: " << maxTextureUnits << "\n";
                    ss << "Maximum OpenGL texture size: " << maxTextureSize;
                    logSystem->log("djv::AV::Render3D::Render", ss.str());
                }
                const uint8_t _textureAtlasCount = std::min(maxTextureUnits, static_cast<GLint>(textureAtlasCount));
                const uint16_t _textureAtlasSize = std::min(maxTextureSize, static_cast<GLint>(textureAtlasSize));
                {
                    auto logSystem = context->getSystemT<LogSystem>();
                    std::stringstream ss;
                    ss << "Texture atlas count: " << _textureAtlasCount << "\n";
                    ss << "Texture atlas size: " << _textureAtlasSize;
                    logSystem->log("djv::AV::Render3D::Render", ss.str());
                }
                p.textureAtlas.reset(new AV::Render::TextureAtlas(
                    _textureAtlasCount,
                    _textureAtlasSize,
                    Image::Type::RGBA_U8,
                    GL_NEAREST,
                    0));

                p.statsTimer = Time::Timer::create(context);
                p.statsTimer->setRepeating(true);
                p.statsTimer->start(
                    Time::getMilliseconds(Time::TimerValue::VerySlow),
                    [this](float)
                    {
                        DJV_PRIVATE_PTR();
                        std::stringstream ss;
                        ss << "Texture atlas: " << p.textureAtlas->getPercentageUsed() << "%\n";
                        ss << "VBO size: " << (p.vbo ? p.vbo->getSize() : 0);
                        _log(ss.str());
                    });
            }

            Render::Render() :
                _p(new Private)
            {}

            Render::~Render()
            {}

            std::shared_ptr<Render> Render::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Render>(new Render);
                out->_init(context);
                return out;
            }

            void Render::beginFrame(const RenderOptions& options)
            {
                DJV_PRIVATE_PTR();
                p.options = options;
            }

            void Render::endFrame()
            {
                DJV_PRIVATE_PTR();

                glEnable(GL_SCISSOR_TEST);
                glEnable(GL_DEPTH_TEST);

                glViewport(
                    static_cast<GLint>(0),
                    static_cast<GLint>(0),
                    static_cast<GLsizei>(p.options.size.w),
                    static_cast<GLsizei>(p.options.size.h));
                glScissor(
                    static_cast<GLint>(0),
                    static_cast<GLint>(0),
                    static_cast<GLsizei>(p.options.size.w),
                    static_cast<GLsizei>(p.options.size.h));
                glClearColor(0.F, 0.F, 0.F, 0.F);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                const auto& atlasTextures = p.textureAtlas->getTextures();
                for (GLuint i = 0; i < static_cast<GLuint>(atlasTextures.size()); ++i)
                {
                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
                    glBindTexture(GL_TEXTURE_2D, atlasTextures[i]);
                }

                const size_t vertexByteCount = AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                if (!p.vbo || p.vboDataSize / vertexByteCount > p.vbo->getSize())
                {
                    p.vbo = OpenGL::VBO::create(p.vboDataSize / vertexByteCount, OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                    p.vao = OpenGL::VAO::create(p.vbo->getType(), p.vbo->getID());
                }
                p.vbo->copy(p.vboData, 0, p.vboDataSize);
                p.vao->bind();

                for (const auto& i : p.primitives)
                {
                    i->material->bind(i->xform);
                    p.vao->draw(i->type, i->vaoOffset, i->vaoSize);
                }

                for (size_t i = 0; i < p.primitives.size(); ++i)
                {
                    delete p.primitives[i];
                }
                p.primitives.clear();
                p.vboDataSize = 0;
            }

            void Render::setMaterial(const std::shared_ptr<IMaterial>& value)
            {
                DJV_PRIVATE_PTR();
                p.currentMaterial = value;
            }

            void Render::drawTriangleMesh(const Geom::TriangleMesh& value)
            {
                DJV_PRIVATE_PTR();
                if (value.triangles.size())
                {
                    auto primitive = new Primitive;
                    p.primitives.push_back(primitive);
                    primitive->xform = p.options.camera.p * p.options.camera.v * _currentTransform;
                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                    primitive->vaoSize = value.triangles.size() * 3;
                    primitive->material = p.currentMaterial;

                    const size_t vboDataSize = p.vboDataSize;
                    p.updateVBODataSize(value.triangles.size() * 3);
                    const auto data = OpenGL::VBO::convert(value, OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                    memcpy(p.vboData.data() + vboDataSize, data.data(), p.vboDataSize - vboDataSize);
                }
            }

            void Render::Private::updateVBODataSize(size_t value)
            {
                const size_t vertexByteCount = AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                vboDataSize += value * vertexByteCount;
                if (vboDataSize > vboData.size())
                {
                    vboData.resize(vboDataSize);
                }
            }

        } // namespace Render3D
    } // namespace AV
} // namespace djv
