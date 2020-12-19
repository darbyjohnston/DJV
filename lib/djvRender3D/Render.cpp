// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvRender3D/Render.h>

#include <djvRender3D/Camera.h>
#include <djvRender3D/Material.h>

#include <djvGL/GLFWSystem.h>
#include <djvGL/MeshFunc.h>
#include <djvGL/MeshCache.h>
#include <djvGL/Shader.h>
#include <djvGL/ShaderSystem.h>
#include <djvGL/TextureAtlas.h>

#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/TimerFunc.h>

#include <array>

using namespace djv::Core;

namespace djv
{
    namespace Render3D
    {
        namespace
        {
            //! \todo Should this be configurable?
            const uint8_t         textureAtlasCount       = 4;
            const uint16_t        textureAtlasSize        = 8192;
            const size_t          shadedMeshCacheSize     = 50000000;
            const size_t          solidColorMeshCacheSize = 10000000;
#if defined(DJV_GL_ES2)
            const GL::VBOType shadedMeshType     = GL::VBOType::Pos3_F32_UV_F32_Normal_F32;
            const GL::VBOType solidColorMeshType = GL::VBOType::Pos3_F32;
#else // DJV_GL_ES2
            const GL::VBOType shadedMeshType     = GL::VBOType::Pos3_F32_UV_U16_Normal_U10;
            const GL::VBOType solidColorMeshType = GL::VBOType::Pos3_F32;
#endif // DJV_GL_ES2

            struct Primitive
            {
                glm::mat4x4                   xform;
                GLenum                        type     = GL_TRIANGLES;
                std::vector<Math::SizeTRange> vaoRange;
                Image::Color                  color;
                std::shared_ptr<IMaterial>    material;
            };

        } // namespace

        struct Render::Private
        {
            RenderOptions                           options;

            std::list<glm::mat4x4>                  transforms;
            std::list<glm::mat4x4>                  inverseTransforms;
            const glm::mat4x4                       identity          = glm::mat4x4(1.F);
            Image::Color                            currentColor;
            std::shared_ptr<IMaterial>              currentMaterial;
            std::vector<std::shared_ptr<ILight> >   lights;
            std::shared_ptr<GL::TextureAtlas>       textureAtlas;
            std::map<GL::VBOType, std::shared_ptr<GL::MeshCache> > meshCache;
            std::map<GL::VBOType, std::map<UID, UID> > meshCacheUIDs;

            std::map<GL::VBOType, std::map<std::shared_ptr<IMaterial>, std::vector<std::shared_ptr<Primitive> > > > primitives;

            std::shared_ptr<System::Timer> statsTimer;
        };

        void Render::_init(const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init("djv::Render3D::Render", context);
            DJV_PRIVATE_PTR();

            addDependency(GL::GLFW::GLFWSystem::create(context));

            GLint maxTextureUnits = 0;
            GLint maxTextureSize = 0;
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
            {
                auto logSystem = context->getSystemT<System::LogSystem>();
                std::stringstream ss;
                ss << "Maximum OpenGL texture units: " << maxTextureUnits << "\n";
                ss << "Maximum OpenGL texture size: " << maxTextureSize;
                logSystem->log("djv::Render3D::Render", ss.str());
            }
            const uint8_t _textureAtlasCount = std::min(maxTextureUnits, static_cast<GLint>(textureAtlasCount));
            const uint16_t _textureAtlasSize = std::min(maxTextureSize, static_cast<GLint>(textureAtlasSize));
            {
                auto logSystem = context->getSystemT<System::LogSystem>();
                std::stringstream ss;
                ss << "Texture atlas count: " << static_cast<size_t>(_textureAtlasCount) << "\n";
                ss << "Texture atlas size: " << static_cast<size_t>(_textureAtlasSize);
                logSystem->log("djv::Render3D::Render", ss.str());
            }
            p.textureAtlas.reset(new GL::TextureAtlas(
                _textureAtlasCount,
                _textureAtlasSize,
                Image::Type::RGBA_U8,
                GL_NEAREST,
                0));

            p.meshCache[shadedMeshType].reset(new GL::MeshCache(
                shadedMeshCacheSize,
                shadedMeshType));
            p.meshCache[solidColorMeshType].reset(new GL::MeshCache(
                solidColorMeshCacheSize,
                solidColorMeshType));

            p.statsTimer = System::Timer::create(context);
            p.statsTimer->setRepeating(true);
            p.statsTimer->start(
                System::getTimerDuration(System::TimerValue::VerySlow),
                [this](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                {
                    DJV_PRIVATE_PTR();
                    std::stringstream ss;
                    ss << "Texture atlas: " << std::fixed << p.textureAtlas->getPercentageUsed() << "%\n";
                    for (const auto& i : p.meshCache)
                    {
                        ss << "Mesh cache " << i.first << ": " << i.second->getPercentageUsed() << "%\n";
                    }
                    _log(ss.str());
                });

            _logInitTime();
        }

        Render::Render() :
            _p(new Private)
        {}

        Render::~Render()
        {}

        std::shared_ptr<Render> Render::create(const std::shared_ptr<System::Context>& context)
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
                cameraP[2][2] = p.options.clip.getMin() / (p.options.clip.getMin() - p.options.clip.getMax());
                cameraP[3][2] = (p.options.clip.getMax() * p.options.clip.getMin()) / (p.options.clip.getMin() - p.options.clip.getMax());
                break;
            default: break;
            }
            glDisable(GL_BLEND);
            //glEnable(GL_POINT_SMOOTH);
#if !defined(DJV_GL_ES2)
            glPointSize(2.0);
            glEnable(GL_LINE_SMOOTH);
#endif // DJV_GL_ES2
            glLineWidth(1.0);

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

            BindData bindData;
            bindData.lights = p.lights;
            PrimitiveBindData primitiveBindData;
            primitiveBindData.camera = p.options.camera->getP() * p.options.camera->getV();
            for (const auto& i : p.primitives)
            {
                auto vao = p.meshCache[i.first]->getVAO();
                vao->bind();
                for (const auto& j : i.second)
                {
                    j.first->getShader()->bind();
                    j.first->bind(bindData);
                    for (const auto& k : j.second)
                    {
                        primitiveBindData.model = k->xform;
                        primitiveBindData.color = k->color;
                        j.first->primitiveBind(primitiveBindData);
                        for (const auto& vaoIt : k->vaoRange)
                        {
                            vao->draw(k->type, vaoIt.getMin(), vaoIt.getMax() - vaoIt.getMin() + 1);
                        }
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

        void Render::setColor(const Image::Color& value)
        {
            DJV_PRIVATE_PTR();
            p.currentColor = value;
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

        void Render::drawPoints(const std::vector<std::shared_ptr<Geom::PointList> >& value)
        {
            DJV_PRIVATE_PTR();
            if (value.size())
            {
                auto primitive = std::shared_ptr<Primitive>(new Primitive);
                primitive->xform = getCurrentTransform();
                primitive->type = GL_POINTS;
                primitive->color = p.currentColor;
                primitive->material = p.currentMaterial;

                for (const auto& i : value)
                {
                    if (i && i->v.size())
                    {
                        auto& meshCache = p.meshCache[solidColorMeshType];
                        auto& meshCacheUIDs = p.meshCacheUIDs[solidColorMeshType];
                        Math::SizeTRange range;
                        bool cached = false;
                        const UID uid = i->getUID();
                        const auto j = meshCacheUIDs.find(uid);
                        if (j != meshCacheUIDs.end())
                        {
                            cached = meshCache->get(j->second, range);
                        }
                        if (!cached)
                        {
                            const auto data = GL::VBO::convert(*i, solidColorMeshType);
                            meshCacheUIDs[uid] = meshCache->add(data, range);
                        }
                        primitive->vaoRange.push_back(range);
                    }
                }

                p.primitives[solidColorMeshType][primitive->material].push_back(primitive);
            }
        }

        void Render::drawPolyLine(const std::shared_ptr<Geom::PointList>& value)
        {
            DJV_PRIVATE_PTR();
            if (value->v.size())
            {
                auto primitive = std::shared_ptr<Primitive>(new Primitive);
                primitive->xform = getCurrentTransform();
                primitive->type = GL_LINE_STRIP;
                primitive->color = p.currentColor;
                primitive->material = p.currentMaterial;

                auto& meshCache = p.meshCache[solidColorMeshType];
                auto& meshCacheUIDs = p.meshCacheUIDs[solidColorMeshType];
                Math::SizeTRange range;
                const UID uid = value->getUID();
                const auto i = meshCacheUIDs.find(uid);
                if (i != meshCacheUIDs.end())
                {
                    meshCache->get(i->second, range);
                }
                if (range.getMin() == range.getMax())
                {
                    const auto data = GL::VBO::convert(*value, solidColorMeshType);
                    meshCacheUIDs[uid] = meshCache->add(data, range);
                }
                primitive->vaoRange.push_back(range);

                p.primitives[solidColorMeshType][primitive->material].push_back(primitive);
            }
        }

        void Render::drawPolyLines(const std::vector<std::shared_ptr<Geom::PointList> >& value)
        {
            DJV_PRIVATE_PTR();
            if (value.size())
            {
                auto primitive = std::shared_ptr<Primitive>(new Primitive);
                primitive->xform = getCurrentTransform();
                primitive->type = GL_LINE_STRIP;
                primitive->color = p.currentColor;
                primitive->material = p.currentMaterial;

                for (const auto& i : value)
                {
                    if (i->v.size())
                    {
                        auto& meshCache = p.meshCache[solidColorMeshType];
                        auto& meshCacheUIDs = p.meshCacheUIDs[solidColorMeshType];
                        Math::SizeTRange range;
                        const UID uid = i->getUID();
                        const auto j = meshCacheUIDs.find(uid);
                        if (j != meshCacheUIDs.end())
                        {
                            meshCache->get(j->second, range);
                        }
                        if (range.getMin() == range.getMax())
                        {
                            const auto data = GL::VBO::convert(*i, solidColorMeshType);
                            meshCacheUIDs[uid] = meshCache->add(data, range);
                        }
                        primitive->vaoRange.push_back(range);
                    }
                }

                p.primitives[solidColorMeshType][primitive->material].push_back(primitive);
            }
        }

        void Render::drawTriangleMesh(const Geom::TriangleMesh& value)
        {
            DJV_PRIVATE_PTR();
            if (value.triangles.size())
            {
                auto primitive = std::shared_ptr<Primitive>(new Primitive);
                primitive->xform = getCurrentTransform();
                primitive->color = p.currentColor;
                primitive->material = p.currentMaterial;

                auto& meshCache = p.meshCache[shadedMeshType];
                auto& meshCacheUIDs = p.meshCacheUIDs[shadedMeshType];
                Math::SizeTRange range;
                const UID uid = value.getUID();
                const auto i = meshCacheUIDs.find(uid);
                if (i != meshCacheUIDs.end())
                {
                    meshCache->get(i->second, range);
                }
                if (range.getMin() == range.getMax())
                {
                    const auto data = GL::VBO::convert(value, shadedMeshType);
                    meshCacheUIDs[uid] = meshCache->add(data, range);
                }
                primitive->vaoRange.push_back(range);

                p.primitives[shadedMeshType][primitive->material].push_back(primitive);
            }
        }

        void Render::drawTriangleMeshes(const std::vector<Geom::TriangleMesh>& value)
        {
            DJV_PRIVATE_PTR();
            if (value.size())
            {
                auto primitive = std::shared_ptr<Primitive>(new Primitive);
                primitive->xform = getCurrentTransform();
                primitive->color = p.currentColor;
                primitive->material = p.currentMaterial;

                auto& meshCache = p.meshCache[shadedMeshType];
                auto& meshCacheUIDs = p.meshCacheUIDs[shadedMeshType];
                for (const auto& i : value)
                {
                    if (i.triangles.size())
                    {
                        Math::SizeTRange range;
                        const UID uid = i.getUID();
                        const auto j = meshCacheUIDs.find(uid);
                        if (j != meshCacheUIDs.end())
                        {
                            meshCache->get(j->second, range);
                        }
                        if (range.getMin() == range.getMax())
                        {
                            const auto data = GL::VBO::convert(i, shadedMeshType);
                            meshCacheUIDs[uid] = meshCache->add(data, range);
                        }
                        primitive->vaoRange.push_back(range);
                    }
                }

                p.primitives[shadedMeshType][primitive->material].push_back(primitive);
            }
        }

        void Render::drawTriangleMeshes(const std::vector<std::shared_ptr<Geom::TriangleMesh> >& value)
        {
            DJV_PRIVATE_PTR();
            if (value.size())
            {
                auto primitive = std::shared_ptr<Primitive>(new Primitive);
                primitive->xform = getCurrentTransform();
                primitive->color = p.currentColor;
                primitive->material = p.currentMaterial;

                auto& meshCache = p.meshCache[shadedMeshType];
                auto& meshCacheUIDs = p.meshCacheUIDs[shadedMeshType];
                for (const auto& i : value)
                {
                    if (i->triangles.size())
                    {
                        Math::SizeTRange range;
                        const UID uid = i->getUID();
                        const auto j = meshCacheUIDs.find(uid);
                        if (j != meshCacheUIDs.end())
                        {
                            meshCache->get(j->second, range);
                        }
                        if (range.getMin() == range.getMax())
                        {
                            const auto data = GL::VBO::convert(*i, shadedMeshType);
                            meshCacheUIDs[uid] = meshCache->add(data, range);
                        }
                        primitive->vaoRange.push_back(range);
                    }
                }

                p.primitives[shadedMeshType][primitive->material].push_back(primitive);
            }
        }

        DJV_ENUM_HELPERS_IMPLEMENTATION(DepthBufferMode);

    } // namespace Render3D
    
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Render3D,
        DepthBufferMode,
        DJV_TEXT("render3d_depth_buffer_mode_standard"),
        DJV_TEXT("render3d_depth_buffer_mode_reverse"));

} // namespace djv
