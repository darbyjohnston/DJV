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
                const size_t textureAtlasSize         = 2048;
                const size_t dynamicTextureCacheCount = 16;
                const size_t renderDataReserve          = 10000;
                const size_t trianglesReserve          = 1000000;
                const size_t vertexReserve            = 3000000;
                const size_t textureCoordsReserve     = 3000000;

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

                struct Render;

                struct Primitive
                {
                    Primitive(Render & render) :
                        render(render)
                    {}

                    virtual ~Primitive() = 0;

                    Render &     render;
                    BBox2f       clipRect;
                    ColorMode    colorMode;
                    Image::Color color;

                    virtual void process(std::vector<RenderData> &, Geom::TriangleMesh &) = 0;
                };

                Primitive::~Primitive()
                {}

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

                    BBox2f                                           viewport;
                    std::vector<std::shared_ptr<Primitive> >         primitives;
                    std::shared_ptr<OpenGL::Shader>                  shader;
                    std::shared_ptr<TextureAtlas>                    textureAtlas;
                    std::map<UID, uint64_t>                          textureIDs;
                    std::map<UID, uint64_t>                          glyphTextureIDs;
                    std::map<UID, std::shared_ptr<OpenGL::Texture> > dynamicTextureCache;
                };

                struct RectPrimitive : public Primitive
                {
                    RectPrimitive(Render & render) :
                        Primitive(render)
                    {}

                    BBox2f rect;

                    void process(std::vector<RenderData> & out, Geom::TriangleMesh & mesh) override
                    {
                        if (rect.intersects(render.viewport))
                        {
                            RenderData data;
                            data.bbox      = rect;
                            data.clipRect  = clipRect;
                            data.colorMode = colorMode;
                            data.color     = color;
                            data.vaoSize   = 6;
                            out.push_back(std::move(data));

                            const size_t vCount = mesh.v.size();
                            Geom::TriangleMesh::Triangle triangle;
                            triangle.v0 = Geom::TriangleMesh::Vertex(vCount + 1);
                            triangle.v1 = Geom::TriangleMesh::Vertex(vCount + 2);
                            triangle.v2 = Geom::TriangleMesh::Vertex(vCount + 3);
                            mesh.triangles.push_back(std::move(triangle));
                            Geom::TriangleMesh::Triangle triangle2;
                            triangle.v0 = Geom::TriangleMesh::Vertex(vCount + 3);
                            triangle.v1 = Geom::TriangleMesh::Vertex(vCount + 4);
                            triangle.v2 = Geom::TriangleMesh::Vertex(vCount + 1);
                            mesh.triangles.push_back(std::move(triangle));

                            mesh.v.push_back(glm::vec3(rect.min.x, rect.min.y, 0.f));
                            mesh.v.push_back(glm::vec3(rect.max.x, rect.min.y, 0.f));
                            mesh.v.push_back(glm::vec3(rect.max.x, rect.max.y, 0.f));
                            mesh.v.push_back(glm::vec3(rect.min.x, rect.max.y, 0.f));
                        }
                    }
                };

                struct RoundedRectPrimitive : public Primitive
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
                };

                struct CirclePrimitive : public Primitive
                {
                    CirclePrimitive(Render & render) :
                        Primitive(render)
                    {}

                    glm::vec2 pos;
                    float     radius;
                    size_t    facets;

                    void process(std::vector<RenderData> & out, Geom::TriangleMesh & mesh) override
                    {
                        const BBox2f rect(pos.x - radius, pos.y - radius, radius * 2.f, radius * 2.f);
                        if (rect.intersects(render.viewport))
                        {
                            RenderData data;
                            data.bbox      = rect;
                            data.clipRect  = clipRect;
                            data.colorMode = colorMode;
                            data.color     = color;
                            data.vaoSize   = 3 * facets;
                            out.push_back(std::move(data));

                            const size_t vCount = mesh.v.size();
                            const size_t trianglesCount = mesh.triangles.size();
                            mesh.v.resize(vCount + facets * 3);
                            mesh.triangles.resize(trianglesCount + facets);
                            auto vIt = mesh.v.begin() + vCount;
                            auto triangleIt = mesh.triangles.begin() + trianglesCount;
                            for (size_t i = 0; i < facets; ++i)
                            {
                                triangleIt->v0 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 1);
                                triangleIt->v1 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 2);
                                triangleIt->v2 = Geom::TriangleMesh::Vertex(vCount + i * 3 + 3);
                                ++triangleIt;

                                vIt->x = pos.x;
                                vIt->y = pos.y;
                                vIt->z = 0.f;
                                ++vIt;
                                float degrees = i / static_cast<float>(facets) * 360.f;
                                vIt->x = pos.x + cosf(Math::deg2rad(degrees)) * radius;
                                vIt->y = pos.y + sinf(Math::deg2rad(degrees)) * radius;
                                vIt->z = 0.f;
                                ++vIt;
                                degrees = (i + 1) / static_cast<float>(facets) * 360.f;
                                vIt->x = pos.x + cosf(Math::deg2rad(degrees)) * radius;
                                vIt->y = pos.y + sinf(Math::deg2rad(degrees)) * radius;
                                vIt->z = 0.f;
                                ++vIt;
                            }
                        }
                    }
                };

                struct ImagePrimitive : public Primitive
                {
                    ImagePrimitive(Render & render, ImageCache cache) :
                        Primitive(render),
                        cache(cache)
                    {}

                    std::shared_ptr<Image::Data> imageData;
                    BBox2f                       rect;
                    ImageFormat                  imageFormat;
                    ImageCache                   cache;

                    void process(std::vector<RenderData> & out, Geom::TriangleMesh & mesh) override
                    {
                        if (rect.intersects(render.viewport))
                        {
                            RenderData data;
                            data.bbox = rect;
                            data.clipRect = clipRect;
                            const auto & info = imageData->getInfo();
                            switch (info.getGLFormat())
                            {
                            case GL_RED:  data.imageFormat = ImageFormat::L;    break;
                            case GL_RG:   data.imageFormat = ImageFormat::LA;   break;
                            case GL_RGB:  data.imageFormat = ImageFormat::RGB;  break;
                            case GL_RGBA: data.imageFormat = ImageFormat::RGBA; break;
                            default: break;
                            }
                            data.colorMode = colorMode;
                            data.color = color;

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
                                const auto i = render.textureIDs.find(uid);
                                if (i != render.textureIDs.end())
                                {
                                    id = i->second;
                                }
                                if (!render.textureAtlas->getItem(id, item))
                                {
                                    render.textureIDs[uid] = render.textureAtlas->addItem(imageData, item);
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
                                const auto i = render.dynamicTextureCache.find(uid);
                                if (i != render.dynamicTextureCache.end())
                                {
                                    data.textureID = i->second->getID();
                                }
                                else
                                {
                                    auto texture = OpenGL::Texture::create(imageData->getInfo());
                                    texture->copy(*imageData);
                                    render.dynamicTextureCache[uid] = texture;
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
                            out.push_back(std::move(data));

                            const size_t vCount = mesh.v.size();
                            const size_t tCount = mesh.t.size();
                            Geom::TriangleMesh::Triangle triangle;
                            triangle.v0 = Geom::TriangleMesh::Vertex(vCount + 1, tCount + 1);
                            triangle.v1 = Geom::TriangleMesh::Vertex(vCount + 2, tCount + 2);
                            triangle.v2 = Geom::TriangleMesh::Vertex(vCount + 3, tCount + 3);
                            mesh.triangles.push_back(std::move(triangle));
                            Geom::TriangleMesh::Triangle triangle2;
                            triangle.v0 = Geom::TriangleMesh::Vertex(vCount + 3, tCount + 3);
                            triangle.v1 = Geom::TriangleMesh::Vertex(vCount + 4, tCount + 4);
                            triangle.v2 = Geom::TriangleMesh::Vertex(vCount + 1, tCount + 1);
                            mesh.triangles.push_back(std::move(triangle));

                            mesh.v.push_back(glm::vec3(rect.min.x, rect.min.y, 0.f));
                            mesh.v.push_back(glm::vec3(rect.max.x, rect.min.y, 0.f));
                            mesh.v.push_back(glm::vec3(rect.max.x, rect.max.y, 0.f));
                            mesh.v.push_back(glm::vec3(rect.min.x, rect.max.y, 0.f));
                            mesh.t.push_back(glm::vec2(textureU.min, textureV.min));
                            mesh.t.push_back(glm::vec2(textureU.max, textureV.min));
                            mesh.t.push_back(glm::vec2(textureU.max, textureV.max));
                            mesh.t.push_back(glm::vec2(textureU.min, textureV.max));
                        }
                    }
                };

                struct TextPrimitive : public Primitive
                {
                    TextPrimitive(Render & render) :
                        Primitive(render)
                    {}

                    std::string text;
                    Font::Info fontInfo;
                    std::future<std::vector<std::shared_ptr<Font::Glyph> > > glyphsFuture;
                    glm::vec2 pos;

                    void process(std::vector<RenderData> & out, Geom::TriangleMesh & mesh) override
                    {
                        try
                        {
                            float x = 0.f;
                            float y = 0.f;
                            int32_t rsbDeltaPrev = 0;
                            const auto glyphs = glyphsFuture.get();
                            for (const auto & glyph : glyphs)
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

                                const glm::vec2 & size = glyph->imageData->getSize();
                                const glm::vec2 & offset = glyph->offset;
                                const BBox2f bbox(
                                    pos.x + x + offset.x,
                                    pos.y + y - offset.y,
                                    size.x,
                                    size.y);
                                if (bbox.intersects(render.viewport))
                                {
                                    const auto uid = glyph->imageData->getUID();
                                    uint64_t id = 0;
                                    const auto i = render.glyphTextureIDs.find(uid);
                                    if (i != render.glyphTextureIDs.end())
                                    {
                                        id = i->second;
                                    }
                                    TextureAtlasItem item;
                                    if (!render.textureAtlas->getItem(id, item))
                                    {
                                        id = render.textureAtlas->addItem(glyph->imageData, item);
                                        render.glyphTextureIDs[uid] = id;
                                    }

                                    RenderData data;
                                    data.bbox       = bbox;
                                    data.clipRect   = clipRect;
                                    data.colorMode  = colorMode;
                                    data.color      = color;
                                    data.atlasIndex = item.textureIndex;
                                    data.vaoSize    = 6;
                                    out.push_back(std::move(data));

                                    const size_t vCount = mesh.v.size();
                                    const size_t tCount = mesh.t.size();
                                    Geom::TriangleMesh::Triangle triangle;
                                    triangle.v0 = Geom::TriangleMesh::Vertex(vCount + 1, tCount + 1);
                                    triangle.v1 = Geom::TriangleMesh::Vertex(vCount + 2, tCount + 2);
                                    triangle.v2 = Geom::TriangleMesh::Vertex(vCount + 3, tCount + 3);
                                    mesh.triangles.push_back(std::move(triangle));
                                    Geom::TriangleMesh::Triangle triangle2;
                                    triangle.v0 = Geom::TriangleMesh::Vertex(vCount + 3, tCount + 3);
                                    triangle.v1 = Geom::TriangleMesh::Vertex(vCount + 4, tCount + 4);
                                    triangle.v2 = Geom::TriangleMesh::Vertex(vCount + 1, tCount + 1);
                                    mesh.triangles.push_back(std::move(triangle));

                                    mesh.v.push_back(glm::vec3(bbox.min.x, bbox.min.y, 0.f));
                                    mesh.v.push_back(glm::vec3(bbox.max.x, bbox.min.y, 0.f));
                                    mesh.v.push_back(glm::vec3(bbox.max.x, bbox.max.y, 0.f));
                                    mesh.v.push_back(glm::vec3(bbox.min.x, bbox.max.y, 0.f));
                                    mesh.t.push_back(glm::vec2(item.textureU.min, item.textureV.min));
                                    mesh.t.push_back(glm::vec2(item.textureU.max, item.textureV.min));
                                    mesh.t.push_back(glm::vec2(item.textureU.max, item.textureV.max));
                                    mesh.t.push_back(glm::vec2(item.textureU.min, item.textureV.max));
                                }

                                x += glyph->advance;
                            }
                        }
                        catch (const std::exception &)
                        {}
                    }
                };

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
                Image::Color                 fillColor          = Image::Color(1.f, 1.f, 1.f);
                std::weak_ptr<Font::System>  fontSystem;
                Font::Info                   currentFont;
                Font::Metrics                currentFontMetrics;

                std::vector<RenderData>      renderData;
                Geom::TriangleMesh           mesh;
                std::unique_ptr<Render>      render;

                std::shared_ptr<Time::Timer> statsTimer;
                //std::shared_ptr<Time::Timer> fpsTimer;
                std::vector<float>           fpsSamples;
                std::chrono::time_point<std::chrono::system_clock>
                                             fpsTime            = std::chrono::system_clock::now();

                void updateCurrentClipRect();
            };

            void Render2D::_init(Context * context)
            {
                ISystem::_init("djv::AV::Render::Render2D", context);

                DJV_PRIVATE_PTR();
                
                auto fontSystem = context->getSystemT<Font::System>();
                p.fontSystem = fontSystem;
                addDependency(fontSystem);

                p.renderData.reserve(renderDataReserve);
                p.mesh.triangles.reserve(trianglesReserve);
                p.mesh.v.reserve(vertexReserve);
                p.mesh.t.reserve(textureCoordsReserve);
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
                    s << "Dynamic texture cache: " << p.render->dynamicTextureCache.size();
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
                for (auto & primitive : p.render->primitives)
                {
                    primitive->process(p.renderData, p.mesh);
                }

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

                auto vbo = OpenGL::VBO::create(p.mesh.triangles.size(), 3, OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                vbo->copy(OpenGL::VBO::convert(p.mesh, vbo->getType()));
                auto vao = OpenGL::VAO::create(vbo->getType(), vbo->getID());
                vao->bind();

                auto imageFormat = static_cast<ImageFormat>(0);
                auto colorMode = static_cast<ColorMode>(0);
                Image::Color color;
                ImageCache imageCache = ImageCache::Atlas;
                size_t atlasIndex = 0;
                GLuint textureID = 0;
                size_t vaoOffset = 0;
                for (size_t i = 0; i < p.renderData.size(); ++i)
                {
                    const auto & data = p.renderData[i];
                    const BBox2f clipRect = flip(data.clipRect, p.size);
                    glScissor(
                        static_cast<GLint>(clipRect.min.x),
                        static_cast<GLint>(clipRect.min.y),
                        static_cast<GLsizei>(clipRect.w()),
                        static_cast<GLsizei>(clipRect.h()));
                    if (i == 0 || data.imageFormat != imageFormat)
                    {
                        imageFormat = data.imageFormat;
                        p.render->shader->setUniform(imageFormatLoc, static_cast<int>(data.imageFormat));
                    }
                    if (i == 0 || data.colorMode != colorMode)
                    {
                        colorMode = data.colorMode;
                        p.render->shader->setUniform(colorModeLoc, static_cast<int>(data.colorMode));
                    }
                    if (i == 0 || data.color != color)
                    {
                        color = data.color;
                        p.render->shader->setUniform(colorLoc, data.color);
                    }
                    switch (data.imageCache)
                    {
                    case ImageCache::Atlas:
                        if (i == 0 || data.imageCache != imageCache || data.atlasIndex != atlasIndex)
                        {
                            imageCache = data.imageCache;
                            atlasIndex = data.atlasIndex;
                            p.render->shader->setUniform(textureSamplerLoc, static_cast<int>(data.atlasIndex));
                        }
                        break;
                    case ImageCache::Dynamic:
                        if (i == 0 || data.imageCache != imageCache || data.textureID != textureID)
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
                p.mesh.triangles.clear();
                p.mesh.v.clear();
                p.mesh.t.clear();
                p.render->primitives.clear();
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
                p.fillColor = value;
            }

            void Render2D::drawRect(const BBox2f & value)
            {
                DJV_PRIVATE_PTR();
                auto primitive = std::unique_ptr<RectPrimitive>(new RectPrimitive(*p.render));
                primitive->rect = value;
                primitive->clipRect = p.currentClipRect;
                primitive->colorMode = ColorMode::SolidColor;
                primitive->color = p.fillColor;
                p.render->primitives.push_back(std::move(primitive));
            }

            void Render2D::drawRoundedRect(const Core::BBox2f & rect, float radius, size_t facets)
            {
                DJV_PRIVATE_PTR();
                auto primitive = std::unique_ptr<RoundedRectPrimitive>(new RoundedRectPrimitive(*p.render));
                primitive->rect = rect;
                primitive->radius = radius;
                primitive->side = Side::None;
                primitive->facets = facets;
                primitive->clipRect = p.currentClipRect;
                primitive->colorMode = ColorMode::SolidColor;
                primitive->color = p.fillColor;
                p.render->primitives.push_back(std::move(primitive));
            }

            void Render2D::drawRoundedRect(const Core::BBox2f & rect, float radius, Side side, size_t facets)
            {
                DJV_PRIVATE_PTR();
                auto primitive = std::unique_ptr<RoundedRectPrimitive>(new RoundedRectPrimitive(*p.render));
                primitive->rect = rect;
                primitive->radius = radius;
                primitive->side = side;
                primitive->facets = facets;
                primitive->clipRect = p.currentClipRect;
                primitive->colorMode = ColorMode::SolidColor;
                primitive->color = p.fillColor;
                p.render->primitives.push_back(std::move(primitive));
            }

            void Render2D::drawCircle(const glm::vec2 & pos, float radius, size_t facets)
            {
                DJV_PRIVATE_PTR();
                auto primitive = std::unique_ptr<CirclePrimitive>(new CirclePrimitive(*p.render));
                primitive->pos = pos;
                primitive->radius = radius;
                primitive->facets = facets;
                primitive->clipRect = p.currentClipRect;
                primitive->colorMode = ColorMode::SolidColor;
                primitive->color = p.fillColor;
                p.render->primitives.push_back(std::move(primitive));
            }

            void Render2D::drawImage(const std::shared_ptr<Image::Data> & data, const BBox2f & rect, ImageCache cache)
            {
                DJV_PRIVATE_PTR();
                auto primitive = std::unique_ptr<ImagePrimitive>(new ImagePrimitive(*p.render, cache));
                primitive->imageData = data;
                primitive->rect = rect;
                primitive->clipRect = p.currentClipRect;
                primitive->colorMode = ColorMode::ColorAndTexture;
                primitive->color = p.fillColor;
                p.render->primitives.push_back(std::move(primitive));
            }

            void Render2D::drawFilledImage(const std::shared_ptr<Image::Data> & data, const BBox2f & rect, ImageCache cache)
            {
                DJV_PRIVATE_PTR();
                auto primitive = std::unique_ptr<ImagePrimitive>(new ImagePrimitive(*p.render, cache));
                primitive->imageData = data;
                primitive->rect = rect;
                primitive->clipRect = p.currentClipRect;
                primitive->colorMode = ColorMode::ColorWithTextureAlpha;
                primitive->color = p.fillColor;
                p.render->primitives.push_back(std::move(primitive));
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
                    auto primitive = std::unique_ptr<TextPrimitive>(new TextPrimitive(*p.render));
                    primitive->text = value;
                    primitive->fontInfo = p.currentFont;
                    primitive->pos = pos;
                    primitive->glyphsFuture = fontSystem->getGlyphs(value, p.currentFont);
                    primitive->clipRect = p.currentClipRect;
                    primitive->colorMode = ColorMode::ColorWithTextureAlpha;
                    primitive->color = p.fillColor;
                    p.render->primitives.push_back(std::move(primitive));
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

        } // namespace Render
    } // namespace AV
} // namespace djv
