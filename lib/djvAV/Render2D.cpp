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

#include <djvAV/Render2D.h>

#include <djvAV/Color.h>
#include <djvAV/OpenGLMesh.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/OpenGLTexture.h>
#include <djvAV/Shader.h>
#include <djvAV/Shape.h>
#include <djvAV/TextureAtlas.h>
#include <djvAV/TriangleMesh.h>

#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>
#include <djvCore/Range.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/Timer.h>

#include <glm/gtc/matrix_transform.hpp>

#include <codecvt>
#include <list>
#include <locale>

using namespace djv::Core;

using namespace gl;

namespace djv
{
    namespace AV
    {
        namespace Render
        {
            namespace
            {
                //! \todo [1.0 S] Should this be configurable?
                const size_t textureAtlasCount        = 4;
                const size_t textureAtlasSize         = 8192;
                const size_t dynamicTextureCacheCount = 16;
                const size_t renderDataReserve        = 10000;
                const size_t vboSize                  = 10000;

                enum class ImageFormat
                {
                    L,
                    LA,
                    RGB,
                    RGBA
                };

                enum class ColorMode
                {
                    SolidColor,
                    ColorWithTextureAlpha,
                    ColorWithTextureAlphaR,
                    ColorWithTextureAlphaG,
                    ColorWithTextureAlphaB,
                    ColorAndTexture
                };

                struct RenderData
                {
                    BBox2f       bbox;
                    BBox2f       clipRect;
                    ImageFormat  imageFormat = ImageFormat::RGBA;
                    ColorMode    colorMode   = ColorMode::SolidColor;
                    Image::Color color;
                    ImageCache   imageCache  = ImageCache::Atlas;
                    size_t       atlasIndex  = 0;
                    GLuint       textureID   = 0;
                    size_t       vaoSize;
                };

                struct Render
                {
                    Render(Context * context)
                    {
                        FileSystem::Path shaderPath;
                        if (auto resourceSystem = context->getSystemT<ResourceSystem>())
                        {
                            shaderPath = resourceSystem->getPath(FileSystem::ResourcePath::ShadersDirectory);
                        }
                        shader = OpenGL::Shader::create(Shader::create(
                            FileSystem::Path(shaderPath, "djvAVRender2DVertex.glsl"),
                            FileSystem::Path(shaderPath, "djvAVRender2DFragment.glsl")));

                        GLint maxTextureUnits = 0;
                        GLint maxTextureSize  = 0;
                        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
                        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
                        {
                            if (auto logSystem = context->getSystemT<LogSystem>())
                            {
                                std::stringstream ss;
                                ss << "Maximum OpenGL texture units: " << maxTextureUnits << "\n";
                                ss << "Maximum OpenGL texture size: " << maxTextureSize;
                                logSystem->log("djv::AV::Render::Render2D", ss.str());
                            }
                        }
                        const size_t _textureAtlasCount = std::min(size_t(maxTextureUnits), textureAtlasCount);
                        const int _textureAtlasSize = std::min(maxTextureSize, int(textureAtlasSize));
                        {
                            if (auto logSystem = context->getSystemT<LogSystem>())
                            {
                                std::stringstream ss;
                                ss << "Texture atlas count: " << _textureAtlasCount << "\n";
                                ss << "Texture atlas size: " << _textureAtlasSize;
                                logSystem->log("djv::AV::Render::Render2D", ss.str());
                            }
                        }
                        textureAtlas.reset(new TextureAtlas(
                            _textureAtlasCount,
                            _textureAtlasSize,
                            Image::Type::RGBA_U8,
                            GL_NEAREST,
                            0));
                    }

                    void copyVBO()
                    {
                        if (vboIndex >= vbos.size())
                        {
                            auto vbo = OpenGL::VBO::create(vboSize, 3, OpenGL::VBOType::Pos2_F32_UV_U16);
                            vbos.push_back(vbo);
                            auto vao = OpenGL::VAO::create(vbo->getType(), vbo->getID());
                            vaos.push_back(vao);
                        }
                        auto vbo = vbos[vboIndex];
                        vbo->copy(vboData);
                        vaoCounts.push_back(std::make_pair(vaos[vboIndex], vboCount));
                        vboCount = 0;
                        vboData.clear();
                        ++vboIndex;
                    }

                    BBox2f                                           viewport;
                    std::shared_ptr<OpenGL::Shader>                  shader;
                    std::shared_ptr<TextureAtlas>                    textureAtlas;
                    std::map<UID, uint64_t>                          textureIDs;
                    std::map<UID, uint64_t>                          glyphTextureIDs;
                    std::map<UID, std::shared_ptr<OpenGL::Texture> > dynamicTextureCache;
                    std::vector<std::shared_ptr<OpenGL::VBO> >       vbos;
                    size_t                                           vboIndex = 0;
                    size_t                                           vboCount = 0;
                    std::vector<uint8_t>                             vboData;
                    std::vector<std::shared_ptr<OpenGL::VAO> >       vaos;
                    std::vector<std::pair<std::shared_ptr<OpenGL::VAO>, size_t> >
                                                                     vaoCounts;
                };

                /*struct RoundedRectPrimitive : public Primitive
                {
                    RoundedRectPrimitive(Render & render) :
                        Primitive(render)
                    {}

                    BBox2f rect;
                    float  radius;
                    Side   side;
                    size_t facets;

                    void process(std::vector<RenderData> & out, Geom::TriangleMesh & mesh) override
                    {
                        if (rect.intersects(render.viewport))
                        {
                            RenderData data;
                            data.bbox      = rect;
                            data.clipRect  = clipRect;
                            data.colorMode = colorMode;
                            data.color     = color;

                            size_t count = 0;
                            switch (side)
                            {
                            case Side::None: count = 3 * 2 + facets * 4; break;
                            default:         count = 2 * 2 + facets * 4; break;
                            }
                            data.vaoSize = count * 3;
                            out.push_back(std::move(data));

                            size_t vCount = mesh.v.size();
                            const size_t trianglesCount = mesh.triangles.size();
                            mesh.v.resize(vCount + count * 3);
                            mesh.triangles.resize(trianglesCount + count);
                            auto vIt = mesh.v.begin() + vCount;
                            auto triangleIt = mesh.triangles.begin() + trianglesCount;

                            switch (side)
                            {
                            case Side::None:
                            {
                                // Center
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 1);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 2);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 3);
                                ++triangleIt;
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 3);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 4);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 1);
                                ++triangleIt;
                                vIt->x = rect.min.x;
                                vIt->y = rect.min.y + radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x;
                                vIt->y = rect.min.y + radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x;
                                vIt->y = rect.max.y - radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.min.x;
                                vIt->y = rect.max.y - radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vCount += 4;

                                // Top
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 1);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 2);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 3);
                                ++triangleIt;
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 3);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 4);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 1);
                                ++triangleIt;
                                vIt->x = rect.min.x + radius;
                                vIt->y = rect.min.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x - radius;
                                vIt->y = rect.min.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x - radius;
                                vIt->y = rect.min.y + radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.min.x + radius;
                                vIt->y = rect.min.y + radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vCount += 4;

                                // Bottom
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 1);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 2);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 3);
                                ++triangleIt;
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 3);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 4);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 1);
                                ++triangleIt;
                                vIt->x = rect.min.x + radius;
                                vIt->y = rect.max.y - radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x - radius;
                                vIt->y = rect.max.y - radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x - radius;
                                vIt->y = rect.max.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.min.x + radius;
                                vIt->y = rect.max.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vCount += 4;

                                // Upper left corner
                                for (size_t i = 0; i < facets; ++i)
                                {
                                    triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 1);
                                    triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 2);
                                    triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 3);
                                    ++triangleIt;

                                    const float x = rect.min.x + radius;
                                    const float y = rect.min.y + radius;
                                    vIt->x = x;
                                    vIt->y = y;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    float degrees = i / static_cast<float>(facets) * 90.f + 180.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    degrees = (i + 1) / static_cast<float>(facets) * 90.f + 180.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                }
                                vCount += facets * 3;

                                // Upper right corner
                                for (size_t i = 0; i < facets; ++i)
                                {
                                    triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 1);
                                    triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 2);
                                    triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 3);
                                    ++triangleIt;

                                    const float x = rect.max.x - radius;
                                    const float y = rect.min.y + radius;
                                    vIt->x = x;
                                    vIt->y = y;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    float degrees = i / static_cast<float>(facets) * 90.f - 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    degrees = (i + 1) / static_cast<float>(facets) * 90.f - 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                }
                                vCount += facets * 3;

                                // Lower left corner
                                for (size_t i = 0; i < facets; ++i)
                                {
                                    triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 1);
                                    triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 2);
                                    triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 3);
                                    ++triangleIt;

                                    const float x = rect.min.x + radius;
                                    const float y = rect.max.y - radius;
                                    vIt->x = x;
                                    vIt->y = y;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    float degrees = i / static_cast<float>(facets) * 90.f + 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    degrees = (i + 1) / static_cast<float>(facets) * 90.f + 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                }
                                vCount += facets * 3;

                                // Lower right corner
                                for (size_t i = 0; i < facets; ++i)
                                {
                                    triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 1);
                                    triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 2);
                                    triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 3);
                                    ++triangleIt;

                                    const float x = rect.max.x - radius;
                                    const float y = rect.max.y - radius;
                                    vIt->x = x;
                                    vIt->y = y;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    float degrees = i / static_cast<float>(facets) * 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    degrees = (i + 1) / static_cast<float>(facets) * 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                }
                                vCount += facets * 3;
                                break;
                            }
                            case Side::Left:
                            {
                                DJV_ASSERT(0);
                                break;
                            }
                            case Side::Top:
                            {
                                // Center
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 1);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 2);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 3);
                                ++triangleIt;
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 3);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 4);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 1);
                                ++triangleIt;
                                vIt->x = rect.min.x;
                                vIt->y = rect.min.y + radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x;
                                vIt->y = rect.min.y + radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x;
                                vIt->y = rect.max.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.min.x;
                                vIt->y = rect.max.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vCount += 4;

                                // Top
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 1);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 2);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 3);
                                ++triangleIt;
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 3);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 4);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 1);
                                ++triangleIt;
                                vIt->x = rect.min.x + radius;
                                vIt->y = rect.min.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x - radius;
                                vIt->y = rect.min.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x - radius;
                                vIt->y = rect.min.y + radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.min.x + radius;
                                vIt->y = rect.min.y + radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vCount += 4;

                                // Upper left corner
                                for (size_t i = 0; i < facets; ++i)
                                {
                                    triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 1);
                                    triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 2);
                                    triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 3);
                                    ++triangleIt;

                                    const float x = rect.min.x + radius;
                                    const float y = rect.min.y + radius;
                                    vIt->x = x;
                                    vIt->y = y;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    float degrees = i / static_cast<float>(facets) * 90.f + 180.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    degrees = (i + 1) / static_cast<float>(facets) * 90.f + 180.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                }
                                vCount += facets * 3;

                                // Upper right corner
                                for (size_t i = 0; i < facets; ++i)
                                {
                                    triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 1);
                                    triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 2);
                                    triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 3);
                                    ++triangleIt;

                                    const float x = rect.max.x - radius;
                                    const float y = rect.min.y + radius;
                                    vIt->x = x;
                                    vIt->y = y;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    float degrees = i / static_cast<float>(facets) * 90.f - 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    degrees = (i + 1) / static_cast<float>(facets) * 90.f - 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                }
                                vCount += facets * 3;
                                break;
                            }
                            case Side::Right:
                            {
                                DJV_ASSERT(0);
                                break;
                            }
                            case Side::Bottom:
                            {
                                // Center
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 1);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 2);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 3);
                                ++triangleIt;
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 3);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 4);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 1);
                                ++triangleIt;
                                vIt->x = rect.min.x;
                                vIt->y = rect.min.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x;
                                vIt->y = rect.min.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x;
                                vIt->y = rect.max.y - radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.min.x;
                                vIt->y = rect.max.y - radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vCount += 4;

                                // Bottom
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 1);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 2);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 3);
                                ++triangleIt;
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + 3);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + 4);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + 1);
                                ++triangleIt;
                                vIt->x = rect.min.x + radius;
                                vIt->y = rect.max.y - radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x - radius;
                                vIt->y = rect.max.y - radius;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.max.x - radius;
                                vIt->y = rect.max.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vIt->x = rect.min.x + radius;
                                vIt->y = rect.max.y;
                                vIt->z = 0.f;
                                ++vIt;
                                vCount += 4;

                                // Lower left corner
                                for (size_t i = 0; i < facets; ++i)
                                {
                                    triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 1);
                                    triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 2);
                                    triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 3);
                                    ++triangleIt;

                                    const float x = rect.min.x + radius;
                                    const float y = rect.max.y - radius;
                                    vIt->x = x;
                                    vIt->y = y;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    float degrees = i / static_cast<float>(facets) * 90.f + 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    degrees = (i + 1) / static_cast<float>(facets) * 90.f + 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                }
                                vCount += facets * 3;

                                // Lower right corner
                                for (size_t i = 0; i < facets; ++i)
                                {
                                    triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 1);
                                    triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 2);
                                    triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 3);
                                    ++triangleIt;

                                    const float x = rect.max.x - radius;
                                    const float y = rect.max.y - radius;
                                    vIt->x = x;
                                    vIt->y = y;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    float degrees = i / static_cast<float>(facets) * 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                    degrees = (i + 1) / static_cast<float>(facets) * 90.f;
                                    vIt->x = x + cosf(Math::deg2rad(degrees)) * radius;
                                    vIt->y = y + sinf(Math::deg2rad(degrees)) * radius;
                                    vIt->z = 0.f;
                                    ++vIt;
                                }
                                vCount += facets * 3;
                                break;
                            }
                            default: break;
                            }
                        }
                    }
                };*/

                BBox2f flip(const BBox2f & value, const glm::ivec2 & size)
                {
                    BBox2f out;
                    out.min.x = value.min.x;
                    out.min.y = static_cast<float>(size.y) - value.max.y;
                    out.max.x = value.max.x;
                    out.max.y = static_cast<float>(size.y) - value.min.y;
                    return out;
                }

            } // namespace

            struct Render2D::Private
            {
                glm::ivec2                   size               = glm::ivec2(0, 0);
                std::list<BBox2f>            clipRects;
                BBox2f                       currentClipRect    = BBox2f(0.f, 0.f, 0.f, 0.f);
                Image::Color                 fillColor          = Image::Color(1.f, 1.f, 1.f, 1.f);
                std::weak_ptr<Font::System>  fontSystem;
                Font::Info                   currentFont;
                Font::Metrics                currentFontMetrics;

                std::vector<RenderData>      renderData;
                std::unique_ptr<Render>      render;

                std::shared_ptr<Time::Timer> statsTimer;
                std::vector<float>           fpsSamples;
                std::chrono::time_point<std::chrono::system_clock>
                                             fpsTime            = std::chrono::system_clock::now();

                void updateCurrentClipRect();
                void drawRoundedRect(const Core::BBox2f&, float radius, size_t facets);
                void drawImage(
                    const std::shared_ptr<Image::Data>&,
                    const Core::BBox2f&,
                    ImageCache,
                    ColorMode);

                struct Vertex
                {
                    glm::vec2 v;
                    glm::vec2 t;
                };
                void addVertices(const std::vector<Vertex>&);
            };

            void Render2D::_init(Context * context)
            {
                ISystem::_init("djv::AV::Render::Render2D", context);

                DJV_PRIVATE_PTR();
                
                auto fontSystem = context->getSystemT<Font::System>();
                p.fontSystem = fontSystem;
                addDependency(fontSystem);

                p.renderData.reserve(renderDataReserve);
                p.render.reset(new Render(context));

                p.statsTimer = Time::Timer::create(context);
                p.statsTimer->setRepeating(true);
                p.statsTimer->start(
                    Time::getMilliseconds(Time::TimerValue::VerySlow),
                    [this](float)
                {
                    DJV_PRIVATE_PTR();
                    std::stringstream s;
                    s << "Texture atlas: " << p.render->textureAtlas->getPercentageUsed() << "%\n";
                    s << "Dynamic texture cache: " << p.render->dynamicTextureCache.size() << "\n";
                    s << "VBOs: " << p.render->vbos.size();
                    _log(s.str());
                });

                /*p.fpsTimer = Time::Timer::create(context);
                p.fpsTimer->setRepeating(true);
                p.fpsTimer->start(
                    Time::getMilliseconds(Time::TimerValue::VerySlow),
                    [this](float)
                {
                    DJV_PRIVATE_PTR();
                    float average = 1.f;
                    for (const auto & i : p.fpsSamples)
                    {
                        average += i;
                    }
                    average /= static_cast<float>(p.fpsSamples.size());
                    std::stringstream s;
                    s << "FPS: " << average;
                    _log(s.str());
                });*/
            }

            Render2D::Render2D() :
                _p(new Private)
            {}

            Render2D::~Render2D()
            {}

            std::shared_ptr<Render2D> Render2D::create(Context * context)
            {
                auto out = std::shared_ptr<Render2D>(new Render2D);
                out->_init(context);
                return out;
            }

            void Render2D::beginFrame(const glm::ivec2 & size)
            {
                DJV_PRIVATE_PTR();
                p.size = size;
                p.currentClipRect = BBox2f(0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y));
                p.render->viewport = BBox2f(0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y));
            }

            void Render2D::endFrame()
            {
                DJV_PRIVATE_PTR();

                glEnable(GL_MULTISAMPLE);
                glEnable(GL_SCISSOR_TEST);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glViewport(
                    static_cast<GLint>(p.render->viewport.min.x),
                    static_cast<GLint>(p.render->viewport.min.y),
                    static_cast<GLsizei>(p.render->viewport.w()),
                    static_cast<GLsizei>(p.render->viewport.h()));
                glScissor(
                    static_cast<GLint>(p.render->viewport.min.x),
                    static_cast<GLint>(p.render->viewport.min.y),
                    static_cast<GLsizei>(p.render->viewport.w()),
                    static_cast<GLsizei>(p.render->viewport.h()));
                glClearColor(0.f, 0.f, 0.f, 0.f);
                glClear(GL_COLOR_BUFFER_BIT);

                p.render->shader->bind();
                const auto viewMatrix = glm::ortho(
                    p.render->viewport.min.x,
                    p.render->viewport.max.x,
                    p.render->viewport.max.y,
                    p.render->viewport.min.y,
                    -1.f, 1.f);
                p.render->shader->setUniform("transform.mvp", viewMatrix);
                const auto program = p.render->shader->getProgram();
                const GLint imageFormatLoc = glGetUniformLocation(program, "imageFormat");
                const GLint colorModeLoc = glGetUniformLocation(program, "colorMode");
                const GLint colorLoc = glGetUniformLocation(program, "color");
                const GLint textureSamplerLoc = glGetUniformLocation(program, "textureSampler");

                const auto & atlasTextures = p.render->textureAtlas->getTextures();
                const size_t atlasTexturesCount = atlasTextures.size();
                for (GLuint i = 0; i < static_cast<GLuint>(atlasTexturesCount); ++i)
                {
                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
                    glBindTexture(GL_TEXTURE_2D, atlasTextures[i]);
                }

                if (p.render->vboData.size())
                {
                    p.render->copyVBO();
                }

                auto imageFormat = static_cast<ImageFormat>(0);
                auto colorMode = static_cast<ColorMode>(0);
                Image::Color color;
                ImageCache imageCache = ImageCache::Atlas;
                size_t atlasIndex = 0;
                GLuint textureID = 0;
                size_t primitiveIndex = 0;
                for (const auto & i : p.render->vaoCounts)
                {
                    auto vao = i.first;
                    vao->bind();
                    size_t vaoOffset = 0;
                    for (size_t j = primitiveIndex; j < primitiveIndex + i.second; ++j)
                    {
                        const auto & data = p.renderData[j];
                        const BBox2f clipRect = flip(data.clipRect, p.size);
                        glScissor(
                            static_cast<GLint>(clipRect.min.x),
                            static_cast<GLint>(clipRect.min.y),
                            static_cast<GLsizei>(clipRect.w()),
                            static_cast<GLsizei>(clipRect.h()));
                        if (0 == primitiveIndex || data.imageFormat != imageFormat)
                        {
                            imageFormat = data.imageFormat;
                            p.render->shader->setUniform(imageFormatLoc, static_cast<int>(data.imageFormat));
                        }
                        if (0 == primitiveIndex || data.colorMode != colorMode)
                        {
                            colorMode = data.colorMode;
                            p.render->shader->setUniform(colorModeLoc, static_cast<int>(data.colorMode));
                        }
                        if (0 == primitiveIndex || data.color != color)
                        {
                            color = data.color;
                            p.render->shader->setUniform(colorLoc, reinterpret_cast<const GLfloat*>(data.color.getData()));
                        }
                        switch (data.imageCache)
                        {
                        case ImageCache::Atlas:
                            if (0 == primitiveIndex || data.imageCache != imageCache || data.atlasIndex != atlasIndex)
                            {
                                imageCache = data.imageCache;
                                atlasIndex = data.atlasIndex;
                                p.render->shader->setUniform(textureSamplerLoc, static_cast<int>(data.atlasIndex));
                            }
                            break;
                        case ImageCache::Dynamic:
                            if (0 == primitiveIndex || data.imageCache != imageCache || data.textureID != textureID)
                            {
                                imageCache = data.imageCache;
                                textureID = data.textureID;
                                glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + atlasTexturesCount));
                                glBindTexture(GL_TEXTURE_2D, textureID);
                                p.render->shader->setUniform(textureSamplerLoc, static_cast<int>(atlasTexturesCount));
                            }
                            break;
                        default: break;
                        }
                        switch (data.colorMode)
                        {
                        case ColorMode::ColorWithTextureAlphaR:
                            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
                            vao->draw(vaoOffset, data.vaoSize);
                            p.render->shader->setUniform(colorModeLoc, static_cast<int>(ColorMode::ColorWithTextureAlphaG));
                            glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE);
                            vao->draw(vaoOffset, data.vaoSize);
                            p.render->shader->setUniform(colorModeLoc, static_cast<int>(ColorMode::ColorWithTextureAlphaB));
                            glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
                            vao->draw(vaoOffset, data.vaoSize);
                            colorMode = ColorMode::ColorWithTextureAlphaB;
                            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                            break;
                        default:
                            vao->draw(vaoOffset, data.vaoSize);
                            break;
                        }
                        vaoOffset += data.vaoSize;
                    }
                    primitiveIndex += i.second;
                }

                const auto now = std::chrono::system_clock::now();
                const std::chrono::duration<float> delta = now - p.fpsTime;
                p.fpsTime = now;
                p.fpsSamples.push_back(1.f / delta.count());
                while (p.fpsSamples.size() > 10)
                {
                    p.fpsSamples.erase(p.fpsSamples.begin());
                }

                p.clipRects.clear();
                p.renderData.clear();
                p.render->vboIndex = 0;
                p.render->vboCount = 0;
                p.render->vboData.clear();
                p.render->vaoCounts.clear();
                while (p.render->dynamicTextureCache.size() > dynamicTextureCacheCount)
                {
                    p.render->dynamicTextureCache.erase(p.render->dynamicTextureCache.begin());
                }
            }

            void Render2D::pushClipRect(const BBox2f & value)
            {
                DJV_PRIVATE_PTR();
                p.clipRects.push_back(value);
                p.updateCurrentClipRect();
            }

            void Render2D::popClipRect()
            {
                DJV_PRIVATE_PTR();
                p.clipRects.pop_back();
                p.updateCurrentClipRect();
            }

            void Render2D::setFillColor(const Image::Color & value)
            {
                DJV_PRIVATE_PTR();
                p.fillColor = value.convert(Image::Type::RGBA_F32);
            }

            void Render2D::drawRect(const BBox2f & value)
            {
                DJV_PRIVATE_PTR();
                if (value.intersects(p.render->viewport))
                {
                    RenderData data;
                    data.bbox = value;
                    data.clipRect = p.currentClipRect;
                    data.colorMode = ColorMode::SolidColor;
                    data.color = p.fillColor;
                    data.vaoSize = 6;
                    p.renderData.push_back(std::move(data));
                    p.addVertices(
                        {
                            { glm::vec2(value.min.x, value.min.y) },
                            { glm::vec2(value.max.x, value.min.y) },
                            { glm::vec2(value.max.x, value.max.y) },
                            { glm::vec2(value.max.x, value.max.y) },
                            { glm::vec2(value.min.x, value.max.y) },
                            { glm::vec2(value.min.x, value.min.y) }
                        }
                    );
                }
            }

            void Render2D::drawRoundedRect(const Core::BBox2f & rect, float radius, size_t facets)
            {
                DJV_PRIVATE_PTR();
                /*auto primitive = std::unique_ptr<RoundedRectPrimitive>(new RoundedRectPrimitive(*p.render));
                primitive->rect = rect;
                primitive->radius = radius;
                primitive->side = Side::None;
                primitive->facets = facets;
                primitive->clipRect = p.currentClipRect;
                primitive->colorMode = ColorMode::SolidColor;
                primitive->color = p.fillColor;
                p.render->primitives.push_back(std::move(primitive));*/
            }

            void Render2D::drawRoundedRect(const Core::BBox2f & rect, float radius, Side side, size_t facets)
            {
                DJV_PRIVATE_PTR();
                /*auto primitive = std::unique_ptr<RoundedRectPrimitive>(new RoundedRectPrimitive(*p.render));
                primitive->rect = rect;
                primitive->radius = radius;
                primitive->side = side;
                primitive->facets = facets;
                primitive->clipRect = p.currentClipRect;
                primitive->colorMode = ColorMode::SolidColor;
                primitive->color = p.fillColor;
                p.render->primitives.push_back(std::move(primitive));*/
            }

            void Render2D::drawCircle(const glm::vec2 & pos, float radius, size_t facets)
            {
                DJV_PRIVATE_PTR();
                const BBox2f rect(pos.x - radius, pos.y - radius, radius * 2.f, radius * 2.f);
                if (rect.intersects(p.render->viewport))
                {
                    RenderData data;
                    data.bbox = rect;
                    data.clipRect = p.currentClipRect;
                    data.colorMode = ColorMode::SolidColor;
                    data.color = p.fillColor;
                    data.vaoSize = 3 * facets;
                    p.renderData.push_back(std::move(data));
                    std::vector<Private::Vertex> v;
                    for (size_t i = 0; i < facets; ++i)
                    {
                        v.push_back({ glm::vec2(pos.x, pos.y) });
                        float degrees = i / static_cast<float>(facets) * 360.f;
                        v.push_back({ glm::vec2(pos.x + cosf(Math::deg2rad(degrees)) * radius, pos.y + sinf(Math::deg2rad(degrees)) * radius) });
                        degrees = (i + 1) / static_cast<float>(facets) * 360.f;
                        v.push_back({ glm::vec2(pos.x + cosf(Math::deg2rad(degrees)) * radius, pos.y + sinf(Math::deg2rad(degrees)) * radius) });
                    }
                    p.addVertices(v);
                }
            }

            void Render2D::drawImage(const std::shared_ptr<Image::Data> & imageData, const BBox2f & rect, ImageCache cache)
            {
                DJV_PRIVATE_PTR();
                p.drawImage(imageData, rect, cache, ColorMode::ColorAndTexture);
            }

            void Render2D::drawFilledImage(const std::shared_ptr<Image::Data> & imageData, const BBox2f & rect, ImageCache cache)
            {
                DJV_PRIVATE_PTR();
                p.drawImage(imageData, rect, cache, ColorMode::ColorWithTextureAlpha);
            }

            void Render2D::setCurrentFont(const Font::Info & value)
            {
                DJV_PRIVATE_PTR();
                p.currentFont = value;
            }

            void Render2D::drawText(const std::string & value, const glm::vec2 & pos, size_t maxLineWidth)
            {
                DJV_PRIVATE_PTR();
                if (auto fontSystem = p.fontSystem.lock())
                {
                    try
                    {
                        float x = 0.f;
                        float y = 0.f;
                        int32_t rsbDeltaPrev = 0;
                        const auto glyphs = fontSystem->getGlyphs(value, p.currentFont).get();
                        for (const auto& glyph : glyphs)
                        {
                            if (rsbDeltaPrev - glyph->lsbDelta > 32)
                            {
                                x -= 1.f;
                            }
                            else if (rsbDeltaPrev - glyph->lsbDelta < -31)
                            {
                                x += 1.f;
                            }
                            rsbDeltaPrev = glyph->rsbDelta;

                            const glm::vec2& size = glyph->imageData->getSize();
                            const glm::vec2& offset = glyph->offset;
                            const BBox2f bbox(
                                pos.x + x + offset.x,
                                pos.y + y - offset.y,
                                size.x,
                                size.y);
                            if (bbox.intersects(p.render->viewport))
                            {
                                const auto uid = glyph->imageData->getUID();
                                uint64_t id = 0;
                                const auto i = p.render->glyphTextureIDs.find(uid);
                                if (i != p.render->glyphTextureIDs.end())
                                {
                                    id = i->second;
                                }
                                TextureAtlasItem item;
                                if (!p.render->textureAtlas->getItem(id, item))
                                {
                                    id = p.render->textureAtlas->addItem(glyph->imageData, item);
                                    p.render->glyphTextureIDs[uid] = id;
                                }

                                RenderData data;
                                data.bbox = bbox;
                                data.clipRect = p.currentClipRect;
                                data.colorMode = ColorMode::ColorWithTextureAlpha;
                                data.color = p.fillColor;
                                data.atlasIndex = item.textureIndex;
                                data.vaoSize = 6;
                                p.renderData.push_back(std::move(data));
                                p.addVertices(
                                    {
                                        { glm::vec2(bbox.min.x, bbox.min.y), glm::vec2(item.textureU.min, item.textureV.min) },
                                        { glm::vec2(bbox.max.x, bbox.min.y), glm::vec2(item.textureU.max, item.textureV.min) },
                                        { glm::vec2(bbox.max.x, bbox.max.y), glm::vec2(item.textureU.max, item.textureV.max) },
                                        { glm::vec2(bbox.max.x, bbox.max.y), glm::vec2(item.textureU.max, item.textureV.max) },
                                        { glm::vec2(bbox.min.x, bbox.max.y), glm::vec2(item.textureU.min, item.textureV.max) },
                                        { glm::vec2(bbox.min.x, bbox.min.y), glm::vec2(item.textureU.min, item.textureV.min) }
                                    }
                                );
                            }

                            x += glyph->advance;
                        }
                    }
                    catch (const std::exception&)
                    {
                    }
                }
            }

            void Render2D::Private::updateCurrentClipRect()
            {
                BBox2f clipRect = BBox2f(0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y));
                for (const auto & i : clipRects)
                {
                    clipRect = clipRect.intersect(i);
                }
                currentClipRect = clipRect;
            }

            void Render2D::Private::drawRoundedRect(const BBox2f& rect, float radius, size_t facets)
            {
                //! \todo
            }

            void Render2D::Private::drawImage(const std::shared_ptr<Image::Data>& imageData, const BBox2f& rect, ImageCache cache, ColorMode colorMode)
            {
                if (rect.intersects(render->viewport))
                {
                    RenderData data;
                    data.bbox = rect;
                    data.clipRect = currentClipRect;
                    const auto& info = imageData->getInfo();
                    switch (info.getGLFormat())
                    {
                    case GL_RED:  data.imageFormat = ImageFormat::L;    break;
                    case GL_RG:   data.imageFormat = ImageFormat::LA;   break;
                    case GL_RGB:  data.imageFormat = ImageFormat::RGB;  break;
                    case GL_RGBA: data.imageFormat = ImageFormat::RGBA; break;
                    default: break;
                    }
                    data.colorMode = colorMode;
                    data.color = fillColor;

                    data.imageCache = cache;
                    FloatRange textureU;
                    FloatRange textureV;
                    const UID uid = imageData->getUID();
                    switch (cache)
                    {
                    case ImageCache::Atlas:
                    {
                        TextureAtlasItem item;
                        uint64_t id = 0;
                        const auto i = render->textureIDs.find(uid);
                        if (i != render->textureIDs.end())
                        {
                            id = i->second;
                        }
                        if (!render->textureAtlas->getItem(id, item))
                        {
                            render->textureIDs[uid] = render->textureAtlas->addItem(imageData, item);
                        }
                        data.atlasIndex = item.textureIndex;
                        if (info.layout.mirror.x)
                        {
                            textureU.min = item.textureU.max;
                            textureU.max = item.textureU.min;
                        }
                        else
                        {
                            textureU = item.textureU;
                        }
                        if (info.layout.mirror.y)
                        {
                            textureV.min = item.textureV.max;
                            textureV.max = item.textureV.min;
                        }
                        else
                        {
                            textureV = item.textureV;
                        }
                        break;
                    }
                    case ImageCache::Dynamic:
                    {
                        const auto i = render->dynamicTextureCache.find(uid);
                        if (i != render->dynamicTextureCache.end())
                        {
                            data.textureID = i->second->getID();
                        }
                        else
                        {
                            auto texture = OpenGL::Texture::create(imageData->getInfo());
                            texture->copy(*imageData);
                            render->dynamicTextureCache[uid] = texture;
                            data.textureID = texture->getID();
                        }
                        textureU.min = 0.f;
                        textureU.max = 1.f;
                        textureV.min = 0.f;
                        textureV.max = 1.f;
                        break;
                    }
                    default: break;
                    }
                    data.vaoSize = 6;
                    renderData.push_back(std::move(data));
                    addVertices(
                        {
                            { glm::vec2(rect.min.x, rect.min.y), glm::vec2(textureU.min, textureV.min) },
                            { glm::vec2(rect.max.x, rect.min.y), glm::vec2(textureU.max, textureV.min) },
                            { glm::vec2(rect.max.x, rect.max.y), glm::vec2(textureU.max, textureV.max) },
                            { glm::vec2(rect.max.x, rect.max.y), glm::vec2(textureU.max, textureV.max) },
                            { glm::vec2(rect.min.x, rect.max.y), glm::vec2(textureU.min, textureV.max) },
                            { glm::vec2(rect.min.x, rect.min.y), glm::vec2(textureU.min, textureV.min) }
                        }
                    );
                }
            }

            void Render2D::Private::addVertices(const std::vector<Vertex>& list)
            {
                if (render->vboData.size() / 3 / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16) + list.size() / 3 > vboSize)
                {
                    render->copyVBO();
                }
                render->vboCount = render->vboCount + 1;
                const size_t vboDataSize = render->vboData.size();
                const size_t size = list.size() * AV::OpenGL::getVertexByteCount(AV::OpenGL::VBOType::Pos2_F32_UV_U16);
                render->vboData.resize(vboDataSize + size);
                uint8_t * pData = &render->vboData[vboDataSize];
                for (const auto& v : list)
                {
                    float* pf = reinterpret_cast<float*>(pData);
                    pf[0] = v.v[0];
                    pf[1] = v.v[1];
                    pData += 2 * sizeof(float);

                    uint16_t* pu16 = reinterpret_cast<uint16_t*>(pData);
                    pu16[0] = Math::clamp(static_cast<int>(v.t[0] * 65535.f), 0, 65535);
                    pu16[1] = Math::clamp(static_cast<int>(v.t[1] * 65535.f), 0, 65535);
                    pData += 2 * sizeof(uint16_t);
                }
            }

        } // namespace Render
    } // namespace AV
} // namespace djv
