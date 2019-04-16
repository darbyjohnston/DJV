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
                const size_t primitiveSize            = 10000;
                const size_t textureAtlasCount        = 4;
                const size_t textureAtlasSize         = 8192;
                const size_t dynamicTextureCacheCount = 16;
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

                struct Primitive
                {
                    BBox2f      bbox;
                    BBox2f      clipRect;
                    ImageFormat imageFormat = ImageFormat::RGBA;
                    ColorMode   colorMode   = ColorMode::SolidColor;
                    float       color[4];
                    ImageCache  imageCache  = ImageCache::Atlas;
                    size_t      atlasIndex  = 0;
                    GLuint      textureID;
                    size_t      vaoSize;
                };

                struct Render
                {
                    Render(Context * context)
                    {
                        primitives.resize(primitiveSize);

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
                        vboData.resize(vboSize * 3 * OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16));

                        FileSystem::Path shaderPath;
                        if (auto resourceSystem = context->getSystemT<ResourceSystem>())
                        {
                            shaderPath = resourceSystem->getPath(FileSystem::ResourcePath::ShadersDirectory);
                        }
                        shader = OpenGL::Shader::create(Shader::create(
                            FileSystem::Path(shaderPath, "djvAVRender2DVertex.glsl"),
                            FileSystem::Path(shaderPath, "djvAVRender2DFragment.glsl")));
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
                        vbo->copy(vboData, 0, vboDataSize);
                        vaoPrimitiveCounts.push_back(std::make_pair(vaos[vboIndex], vboPrimitiveCount));
                        vboPrimitiveCount = 0;
                        vboDataSize = 0;
                        ++vboIndex;
                    }

                    BBox2f                                           viewport;
                    std::vector<Primitive>                           primitives;
                    size_t                                           primitivesSize = 0;
                    std::shared_ptr<TextureAtlas>                    textureAtlas;
                    std::map<UID, uint64_t>                          textureIDs;
                    std::map<UID, uint64_t>                          glyphTextureIDs;
                    std::map<UID, std::shared_ptr<OpenGL::Texture> > dynamicTextureCache;
                    std::vector<std::shared_ptr<OpenGL::VBO> >       vbos;
                    size_t                                           vboIndex = 0;
                    size_t                                           vboPrimitiveCount = 0;
                    std::vector<uint8_t>                             vboData;
                    size_t                                           vboDataSize = 0;
                    std::vector<std::shared_ptr<OpenGL::VAO> >       vaos;
                    std::vector<std::pair<std::shared_ptr<OpenGL::VAO>, size_t> >
                                                                     vaoPrimitiveCounts;
                    std::shared_ptr<OpenGL::Shader>                  shader;
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

                struct Vertex
                {
                    float vx;
                    float vy;
                    float tx;
                    float ty;
                };

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

                void addVertices(const std::vector<Vertex>&);
            };

            void Render2D::_init(Context * context)
            {
                ISystem::_init("djv::AV::Render::Render2D", context);

                DJV_PRIVATE_PTR();
                
                auto fontSystem = context->getSystemT<Font::System>();
                p.fontSystem = fontSystem;
                addDependency(fontSystem);

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
                    s << "VBO count: " << p.render->vbos.size();
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

                if (p.render->vboDataSize)
                {
                    p.render->copyVBO();
                }

                auto imageFormat = static_cast<ImageFormat>(0);
                auto colorMode = static_cast<ColorMode>(0);
                float color[] = { 0.f, 0.f, 0.f, 0.f };
                ImageCache imageCache = ImageCache::Atlas;
                size_t atlasIndex = 0;
                GLuint textureID = 0;
                size_t primitiveIndex = 0;
                for (const auto & i : p.render->vaoPrimitiveCounts)
                {
                    auto vao = i.first;
                    vao->bind();
                    size_t vaoOffset = 0;
                    for (size_t j = primitiveIndex; j < primitiveIndex + i.second; ++j)
                    {
                        const auto & primitive = p.render->primitives[j];
                        const BBox2f clipRect = flip(primitive.clipRect, p.size);
                        glScissor(
                            static_cast<GLint>(clipRect.min.x),
                            static_cast<GLint>(clipRect.min.y),
                            static_cast<GLsizei>(clipRect.w()),
                            static_cast<GLsizei>(clipRect.h()));
                        if ((0 == primitiveIndex && 0 == j) || primitive.imageFormat != imageFormat)
                        {
                            imageFormat = primitive.imageFormat;
                            p.render->shader->setUniform(imageFormatLoc, static_cast<int>(primitive.imageFormat));
                        }
                        if ((0 == primitiveIndex && 0 == j) || primitive.colorMode != colorMode)
                        {
                            colorMode = primitive.colorMode;
                            p.render->shader->setUniform(colorModeLoc, static_cast<int>(primitive.colorMode));
                        }
                        if ((0 == primitiveIndex && 0 == j) ||
                            primitive.color[0] != color[0] ||
                            primitive.color[1] != color[1] ||
                            primitive.color[2] != color[2] ||
                            primitive.color[3] != color[3])
                        {
                            color[0] = primitive.color[0];
                            color[1] = primitive.color[1];
                            color[2] = primitive.color[2];
                            color[3] = primitive.color[3];
                            p.render->shader->setUniform(colorLoc, reinterpret_cast<const GLfloat*>(color));
                        }
                        switch (primitive.imageCache)
                        {
                        case ImageCache::Atlas:
                            if ((0 == primitiveIndex && 0 == j) || primitive.imageCache != imageCache || primitive.atlasIndex != atlasIndex)
                            {
                                imageCache = primitive.imageCache;
                                atlasIndex = primitive.atlasIndex;
                                p.render->shader->setUniform(textureSamplerLoc, static_cast<int>(primitive.atlasIndex));
                            }
                            break;
                        case ImageCache::Dynamic:
                            if ((0 == primitiveIndex && 0 == j) || primitive.imageCache != imageCache || primitive.textureID != textureID)
                            {
                                imageCache = primitive.imageCache;
                                textureID = primitive.textureID;
                                glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + atlasTexturesCount));
                                glBindTexture(GL_TEXTURE_2D, textureID);
                                p.render->shader->setUniform(textureSamplerLoc, static_cast<int>(atlasTexturesCount));
                            }
                            break;
                        default: break;
                        }
                        switch (primitive.colorMode)
                        {
                        case ColorMode::ColorWithTextureAlphaR:
                            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
                            vao->draw(vaoOffset, primitive.vaoSize);
                            p.render->shader->setUniform(colorModeLoc, static_cast<int>(ColorMode::ColorWithTextureAlphaG));
                            glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE);
                            vao->draw(vaoOffset, primitive.vaoSize);
                            p.render->shader->setUniform(colorModeLoc, static_cast<int>(ColorMode::ColorWithTextureAlphaB));
                            glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
                            vao->draw(vaoOffset, primitive.vaoSize);
                            colorMode = ColorMode::ColorWithTextureAlphaB;
                            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                            break;
                        default:
                            vao->draw(vaoOffset, primitive.vaoSize);
                            break;
                        }
                        vaoOffset += primitive.vaoSize;
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
                p.render->primitivesSize = 0;
                p.render->vboIndex = 0;
                p.render->vboPrimitiveCount = 0;
                p.render->vboDataSize = 0;
                p.render->vaoPrimitiveCounts.clear();
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
                if (value == p.fillColor)
                    return;
                if (p.fillColor.getType() == value.getType())
                {
                    p.fillColor = value;
                }
                else
                {
                    p.fillColor = value.convert(Image::Type::RGBA_F32);
                }
            }

            void Render2D::drawRect(const BBox2f & value)
            {
                DJV_PRIVATE_PTR();
                if (value.intersects(p.render->viewport))
                {
                    if (p.render->primitivesSize >= p.render->primitives.size())
                    {
                        p.render->primitives.resize(p.render->primitivesSize + 1);
                    }
                    Primitive& primitive = p.render->primitives[p.render->primitivesSize++];
                    primitive.bbox = value;
                    primitive.clipRect = p.currentClipRect;
                    primitive.colorMode = ColorMode::SolidColor;
                    primitive.color[0] = p.fillColor.getF32(0);
                    primitive.color[1] = p.fillColor.getF32(1);
                    primitive.color[2] = p.fillColor.getF32(2);
                    primitive.color[3] = p.fillColor.getF32(3);
                    primitive.vaoSize = 6;

                    static std::vector<Vertex> v(6);
                    v[0].vx = value.min.x;
                    v[0].vy = value.min.y;
                    v[1].vx = value.max.x;
                    v[1].vy = value.min.y;
                    v[2].vx = value.max.x;
                    v[2].vy = value.max.y;
                    v[3].vx = value.max.x;
                    v[3].vy = value.max.y;
                    v[4].vx = value.min.x;
                    v[4].vy = value.max.y;
                    v[5].vx = value.min.x;
                    v[5].vy = value.min.y;
                    p.addVertices(v);
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
                    if (p.render->primitivesSize >= p.render->primitives.size())
                    {
                        p.render->primitives.resize(p.render->primitivesSize + 1);
                    }
                    Primitive& primitive = p.render->primitives[p.render->primitivesSize++];
                    primitive.bbox = rect;
                    primitive.clipRect = p.currentClipRect;
                    primitive.colorMode = ColorMode::SolidColor;
                    primitive.color[0] = p.fillColor.getF32(0);
                    primitive.color[1] = p.fillColor.getF32(1);
                    primitive.color[2] = p.fillColor.getF32(2);
                    primitive.color[3] = p.fillColor.getF32(3);
                    primitive.vaoSize = 3 * facets;

                    static std::vector<Vertex> v;
                    v.resize(facets * 3);
                    for (size_t i = 0; i < facets * 3; i += 3)
                    {
                        v[i].vx = pos.x;
                        v[i].vy = pos.y;
                        float degrees = i / static_cast<float>(facets) * 360.f;
                        v[i + 1].vx = pos.x + cosf(Math::deg2rad(degrees)) * radius;
                        v[i + 1].vy = pos.y + sinf(Math::deg2rad(degrees)) * radius;
                        degrees = (i + 1) / static_cast<float>(facets) * 360.f;
                        v[i + 2].vx = pos.x + cosf(Math::deg2rad(degrees)) * radius;
                        v[i + 2].vy = pos.y + sinf(Math::deg2rad(degrees)) * radius;
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
                            const BBox2f bbox(pos.x + x + offset.x, pos.y + y - offset.y, size.x, size.y);
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

                                if (p.render->primitivesSize >= p.render->primitives.size())
                                {
                                    p.render->primitives.resize(p.render->primitivesSize + 1);
                                }
                                Primitive& primitive = p.render->primitives[p.render->primitivesSize++];
                                primitive.bbox = bbox;
                                primitive.clipRect = p.currentClipRect;
                                primitive.colorMode = ColorMode::ColorWithTextureAlpha;
                                primitive.color[0] = p.fillColor.getF32(0);
                                primitive.color[1] = p.fillColor.getF32(1);
                                primitive.color[2] = p.fillColor.getF32(2);
                                primitive.color[3] = p.fillColor.getF32(3);
                                primitive.atlasIndex = item.textureIndex;
                                primitive.vaoSize = 6;

                                static std::vector<Vertex> v(6);
                                v[0].vx = bbox.min.x;
                                v[0].vy = bbox.min.y;
                                v[0].tx = item.textureU.min;
                                v[0].ty = item.textureV.min;
                                v[1].vx = bbox.max.x;
                                v[1].vy = bbox.min.y;
                                v[1].tx = item.textureU.max;
                                v[1].ty = item.textureV.min;
                                v[2].vx = bbox.max.x;
                                v[2].vy = bbox.max.y;
                                v[2].tx = item.textureU.max;
                                v[2].ty = item.textureV.max;
                                v[3].vx = bbox.max.x;
                                v[3].vy = bbox.max.y;
                                v[3].tx = item.textureU.max;
                                v[3].ty = item.textureV.max;
                                v[4].vx = bbox.min.x;
                                v[4].vy = bbox.max.y;
                                v[4].tx = item.textureU.min;
                                v[4].ty = item.textureV.max;
                                v[5].vx = bbox.min.x;
                                v[5].vy = bbox.min.y;
                                v[5].tx = item.textureU.min;
                                v[5].ty = item.textureV.min;
                                p.addVertices(v);
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
                    if (primitiveSize >= render->primitives.size())
                    {
                        render->primitives.resize(primitiveSize + 1);
                    }
                    Primitive& primitive = render->primitives[render->primitivesSize++];
                    primitive.bbox = rect;
                    primitive.clipRect = currentClipRect;
                    const auto& info = imageData->getInfo();
                    switch (info.getGLFormat())
                    {
                    case GL_RED:  primitive.imageFormat = ImageFormat::L;    break;
                    case GL_RG:   primitive.imageFormat = ImageFormat::LA;   break;
                    case GL_RGB:  primitive.imageFormat = ImageFormat::RGB;  break;
                    case GL_RGBA: primitive.imageFormat = ImageFormat::RGBA; break;
                    default: break;
                    }
                    primitive.colorMode = colorMode;
                    primitive.color[0] = fillColor.getF32(0);
                    primitive.color[1] = fillColor.getF32(1);
                    primitive.color[2] = fillColor.getF32(2);
                    primitive.color[3] = fillColor.getF32(3);
                    primitive.imageCache = cache;
                    static FloatRange textureU;
                    static FloatRange textureV;
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
                        primitive.atlasIndex = item.textureIndex;
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
                            primitive.textureID = i->second->getID();
                        }
                        else
                        {
                            auto texture = OpenGL::Texture::create(imageData->getInfo());
                            texture->copy(*imageData);
                            render->dynamicTextureCache[uid] = texture;
                            primitive.textureID = texture->getID();
                        }
                        textureU.min = 0.f;
                        textureU.max = 1.f;
                        textureV.min = 0.f;
                        textureV.max = 1.f;
                        break;
                    }
                    default: break;
                    }
                    primitive.vaoSize = 6;

                    static std::vector<Vertex> v(6);
                    v[0].vx = rect.min.x;
                    v[0].vy = rect.min.y;
                    v[0].tx = textureU.min;
                    v[0].ty = textureV.min;
                    v[1].vx = rect.max.x;
                    v[1].vy = rect.min.y;
                    v[1].tx = textureU.max;
                    v[1].ty = textureV.min;
                    v[2].vx = rect.max.x;
                    v[2].vy = rect.max.y;
                    v[2].tx = textureU.max;
                    v[2].ty = textureV.max;
                    v[3].vx = rect.max.x;
                    v[3].vy = rect.max.y;
                    v[3].tx = textureU.max;
                    v[3].ty = textureV.max;
                    v[4].vx = rect.min.x;
                    v[4].vy = rect.max.y;
                    v[4].tx = textureU.min;
                    v[4].ty = textureV.max;
                    v[5].vx = rect.min.x;
                    v[5].vy = rect.min.y;
                    v[5].tx = textureU.min;
                    v[5].ty = textureV.min;
                    addVertices(v);
                }
            }

            void Render2D::Private::addVertices(const std::vector<Vertex>& list)
            {
                const size_t listSize = list.size();
                const size_t vertexByteCount = AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                if (render->vboDataSize / 3 / vertexByteCount + listSize / 3 > vboSize)
                {
                    render->copyVBO();
                }
                ++render->vboPrimitiveCount;
                uint8_t * pData = &render->vboData[render->vboDataSize];
                for (const auto& v : list)
                {
                    float* pf = reinterpret_cast<float*>(pData);
                    pf[0] = v.vx;
                    pf[1] = v.vy;
                    pData += 2 * sizeof(float);

                    uint16_t* pu16 = reinterpret_cast<uint16_t*>(pData);
                    pu16[0] = Math::clamp(static_cast<int>(v.tx * 65535.f), 0, 65535);
                    pu16[1] = Math::clamp(static_cast<int>(v.ty * 65535.f), 0, 65535);
                    pData += 2 * sizeof(uint16_t);
                }
                render->vboDataSize += listSize * vertexByteCount;
            }

        } // namespace Render
    } // namespace AV
} // namespace djv
