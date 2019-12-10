//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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
#include <djvAV/OpenGLMeshCache.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/OpenGLTextureAtlas.h>
#include <djvAV/Render3DCamera.h>
#include <djvAV/Render3DLight.h>
#include <djvAV/Render3DMaterial.h>
#include <djvAV/ShaderSystem.h>

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
                const uint8_t  meshCacheCount    = 1;
                const size_t   meshCacheSize     = 50000000;

                struct Primitive
                {
                    glm::mat4x4                 xform;
                    GLenum                      type     = GL_TRIANGLES;
                    std::vector<SizeTRange>     vaoRange;
                    std::shared_ptr<IMaterial>  material;
                };

            } // namespace

            struct Render::Private
            {
                RenderOptions                           options;

                std::list<glm::mat4x4>                  transforms;
                std::list<glm::mat4x4>                  inverseTransforms;
                const glm::mat4x4                       identity            = glm::mat4x4(1.f);
                std::shared_ptr<IMaterial>              currentMaterial;
                std::vector<std::shared_ptr<ILight> >   lights;
                std::shared_ptr<OpenGL::TextureAtlas>   textureAtlas;
                std::shared_ptr<OpenGL::MeshCache>      meshCache;
                std::map<UID, UID>                      meshCacheUIDs;

                std::map<std::shared_ptr<AV::OpenGL::Shader>, std::vector<std::shared_ptr<Primitive> > > primitives;

                std::shared_ptr<Time::Timer>            statsTimer;
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
                p.textureAtlas.reset(new OpenGL::TextureAtlas(
                    _textureAtlasCount,
                    _textureAtlasSize,
                    Image::Type::RGBA_U8,
                    GL_NEAREST,
                    0));

                p.meshCache.reset(new OpenGL::MeshCache(
                    meshCacheSize,
                    OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10));

                p.statsTimer = Time::Timer::create(context);
                p.statsTimer->setRepeating(true);
                p.statsTimer->start(
                    Time::getMilliseconds(Time::TimerValue::VerySlow),
                    [this](float)
                    {
                        DJV_PRIVATE_PTR();
                        std::stringstream ss;
                        ss << "Texture atlas: " << p.textureAtlas->getPercentageUsed() << "%\n";
                        ss << "Mesh cache: " << p.meshCache->getPercentageUsed() << "%\n";
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
                glm::mat4x4 cameraP = p.options.camera->getP();
                switch (p.options.depthBufferMode)
                {
                case DepthBufferMode::Standard:
                    glDepthRangef(0.0, 1.0);
                    glDepthFunc(GL_LESS);
                    break;
                case DepthBufferMode::Reverse:
                    glDepthRangef(-1.0, 1.0);
                    cameraP[2][2] = p.options.clip.min / (p.options.clip.min - p.options.clip.max);
                    cameraP[3][2] = (p.options.clip.max * p.options.clip.min) / (p.options.clip.min - p.options.clip.max);
                    break;
                }
                glDisable(GL_BLEND);

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

                auto vao = p.meshCache->getVAO();
                vao->bind();

                std::shared_ptr<HemisphereLight>                hemisphereLight;
                std::vector<std::shared_ptr<DirectionalLight> > directionalLights;
                std::vector<std::shared_ptr<PointLight> >       pointLights;
                std::vector<std::shared_ptr<SpotLight> >        spotLights;
                for (auto light = p.lights.begin(); light != p.lights.end(); ++light)
                {
                    if (auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(*light))
                    {
                        if (directionalLights.size() < 16)
                        {
                            directionalLights.push_back(directionalLight);
                        }
                    }
                    else if (auto pointLight = std::dynamic_pointer_cast<PointLight>(*light))
                    {
                        if (pointLights.size() < 16)
                        {
                            pointLights.push_back(pointLight);
                        }
                    }
                    else if (auto spotLight = std::dynamic_pointer_cast<SpotLight>(*light))
                    {
                        if (spotLights.size() < 16)
                        {
                            spotLights.push_back(spotLight);
                        }
                    }
                }
                if (0 == directionalLights.size() && 0 == pointLights.size() && 0 == spotLights.size())
                {
                    hemisphereLight = HemisphereLight::create();
                    auto directionalLight = DirectionalLight::create();
                    directionalLights.push_back(directionalLight);
                }

                for (const auto& shaderIt : p.primitives)
                {
                    shaderIt.first->bind();

                    if (hemisphereLight)
                    {
                        shaderIt.first->setUniform("hemisphereLight.intensity", hemisphereLight->getIntensity());
                        shaderIt.first->setUniform("hemisphereLight.up", hemisphereLight->getUp());
                        shaderIt.first->setUniformF("hemisphereLight.topColor", hemisphereLight->getTopColor());
                        shaderIt.first->setUniformF("hemisphereLight.bottomColor", hemisphereLight->getBottomColor());
                        shaderIt.first->setUniform("hemisphereLightEnabled", 1);
                    }

                    size_t size = directionalLights.size();
                    for (size_t i = 0; i < size; ++i)
                    {
                        {
                            std::stringstream ss;
                            ss << "directionalLights[" << i << "].intensity";
                            shaderIt.first->setUniform(ss.str(), directionalLights[i]->getIntensity());
                        }
                        {
                            std::stringstream ss;
                            ss << "directionalLights[" << i << "].direction";
                            shaderIt.first->setUniform(ss.str(), glm::normalize(directionalLights[i]->getDirection()));
                        }
                    }
                    shaderIt.first->setUniform("directionalLightsCount", static_cast<int>(size));

                    size = pointLights.size();
                    for (size_t i = 0; i < size; ++i)
                    {
                        {
                            std::stringstream ss;
                            ss << "pointLights[" << i << "].intensity";
                            shaderIt.first->setUniform(ss.str(), pointLights[i]->getIntensity());
                        }
                        {
                            std::stringstream ss;
                            ss << "pointLights[" << i << "].position";
                            shaderIt.first->setUniform(ss.str(), pointLights[i]->getPosition());
                        }
                    }
                    shaderIt.first->setUniform("pointLightsCount", static_cast<int>(size));

                    size = spotLights.size();
                    for (size_t i = 0; i < size; ++i)
                    {
                        {
                            std::stringstream ss;
                            ss << "spotLights[" << i << "].intensity";
                            shaderIt.first->setUniform(ss.str(), spotLights[i]->getIntensity());
                        }
                        {
                            std::stringstream ss;
                            ss << "spotLights[" << i << "].coneAngle";
                            shaderIt.first->setUniform(ss.str(), spotLights[i]->getConeAngle());
                        }
                        {
                            std::stringstream ss;
                            ss << "spotLights[" << i << "].direction";
                            shaderIt.first->setUniform(ss.str(), glm::normalize(spotLights[i]->getDirection()));
                        }
                        {
                            std::stringstream ss;
                            ss << "spotLights[" << i << "].position";
                            shaderIt.first->setUniform(ss.str(), spotLights[i]->getPosition());
                        }
                    }
                    shaderIt.first->setUniform("spotLightsCount", static_cast<int>(size));

                    const GLint mLoc = glGetUniformLocation(shaderIt.first->getProgram(), "transform.m");
                    const GLint mvpLoc = glGetUniformLocation(shaderIt.first->getProgram(), "transform.mvp");
                    const GLint normalsLoc = glGetUniformLocation(shaderIt.first->getProgram(), "transform.normals");
                    for (const auto& primitiveIt : shaderIt.second)
                    {
                        shaderIt.first->setUniform(mLoc, primitiveIt->xform);
                        shaderIt.first->setUniform(mvpLoc, cameraP * p.options.camera->getV() * primitiveIt->xform);
                        shaderIt.first->setUniform(normalsLoc, glm::transpose(glm::inverse(glm::mat3x3(primitiveIt->xform))));
                        primitiveIt->material->bind();
                        for (const auto& vaoIt : primitiveIt->vaoRange)
                        {
                            vao->draw(primitiveIt->type, vaoIt.min, vaoIt.max - vaoIt.min + 1);
                        }
                    }
                }

                p.transforms.clear();
                p.inverseTransforms.clear();
                p.primitives.clear();
                p.lights.clear();
            }

            const glm::mat4x4& Render::getCurrentTransform() const
            {
                DJV_PRIVATE_PTR();
                return p.transforms.size() ? p.transforms.back() : p.identity;
            }

            const glm::mat4x4& Render::getCurrentInverseTransform() const
            {
                DJV_PRIVATE_PTR();
                return p.inverseTransforms.size() ? p.inverseTransforms.back() : p.identity;
            }

            void Render::pushTransform(const glm::mat4x4& value)
            {
                DJV_PRIVATE_PTR();
                p.transforms.push_back(getCurrentTransform() * value);
                glm::mat4x4 inverse = glm::inverse(getCurrentTransform());
                p.inverseTransforms.push_back(inverse);
            }

            void Render::popTransform()
            {
                DJV_PRIVATE_PTR();
                if (p.transforms.size())
                {
                    p.transforms.pop_back();
                }
                if (p.inverseTransforms.size())
                {
                    p.inverseTransforms.pop_back();
                }
            }

            void Render::setMaterial(const std::shared_ptr<IMaterial>& value)
            {
                DJV_PRIVATE_PTR();
                p.currentMaterial = value;
            }

            void Render::addLight(const std::shared_ptr<ILight>& value)
            {
                DJV_PRIVATE_PTR();
                p.lights.push_back(value);
            }

            void Render::drawTriangleMesh(const Geom::TriangleMesh& value)
            {
                DJV_PRIVATE_PTR();
                if (value.triangles.size())
                {
                    auto primitive = std::shared_ptr<Primitive>(new Primitive);
                    primitive->xform = getCurrentTransform();
                    primitive->material = p.currentMaterial;

                    SizeTRange range;
                    const UID uid = value.getUID();
                    const auto i = p.meshCacheUIDs.find(uid);
                    if (i != p.meshCacheUIDs.end())
                    {
                        p.meshCache->getItem(i->second, range);
                    }
                    if (range.min == range.max)
                    {
                        const auto data = OpenGL::VBO::convert(value, OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                        p.meshCacheUIDs[value.getUID()] = p.meshCache->addItem(data, range);
                    }
                    primitive->vaoRange.push_back(range);

                    p.primitives[primitive->material->getShader()].push_back(primitive);
                }
            }

            void Render::drawTriangleMeshes(const std::vector<Geom::TriangleMesh>& value)
            {
                DJV_PRIVATE_PTR();
                if (value.size())
                {
                    auto primitive = std::shared_ptr<Primitive>(new Primitive);
                    primitive->xform = getCurrentTransform();
                    primitive->material = p.currentMaterial;
                    for (const auto& i : value)
                    {
                        if (i.triangles.size())
                        {
                            SizeTRange range;
                            const UID uid = i.getUID();
                            const auto j = p.meshCacheUIDs.find(uid);
                            if (j != p.meshCacheUIDs.end())
                            {
                                p.meshCache->getItem(j->second, range);
                            }
                            if (range.min == range.max)
                            {
                                const auto data = OpenGL::VBO::convert(i, OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                                p.meshCacheUIDs[i.getUID()] = p.meshCache->addItem(data, range);
                            }
                            primitive->vaoRange.push_back(range);
                        }
                    }
                    p.primitives[primitive->material->getShader()].push_back(primitive);
                }
            }

            void Render::drawTriangleMeshes(const std::vector<std::shared_ptr<Geom::TriangleMesh> >& value)
            {
                DJV_PRIVATE_PTR();
                if (value.size())
                {
                    auto primitive = std::shared_ptr<Primitive>(new Primitive);
                    primitive->xform = getCurrentTransform();
                    primitive->material = p.currentMaterial;
                    for (const auto& i : value)
                    {
                        if (i->triangles.size())
                        {
                            SizeTRange range;
                            const UID uid = i->getUID();
                            const auto j = p.meshCacheUIDs.find(uid);
                            if (j != p.meshCacheUIDs.end())
                            {
                                p.meshCache->getItem(j->second, range);
                            }
                            if (range.min == range.max)
                            {
                                const auto data = OpenGL::VBO::convert(*i, OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                                p.meshCacheUIDs[i->getUID()] = p.meshCache->addItem(data, range);
                            }
                            primitive->vaoRange.push_back(range);
                        }
                    }
                    p.primitives[primitive->material->getShader()].push_back(primitive);
                }
            }

        } // namespace Render3D
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::Render3D,
        DepthBufferMode,
        DJV_TEXT("Standard"),
        DJV_TEXT("Reverse"));

} // namespace djv
