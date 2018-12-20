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

#include <djvAV/Render2DSystem.h>

#include <djvAV/Color.h>
#include <djvAV/OpenGLMesh.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/Shader.h>
#include <djvAV/Shape.h>
#include <djvAV/TextureCache.h>
#include <djvAV/TriangleMesh.h>

#include <djvCore/Context.h>
#include <djvCore/Range.h>
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
        namespace
        {
            //! \todo [1.0 S] Should this be configurable?
            const size_t textureCacheSize = 8192;
            const size_t textureCacheCount = 4;
            const size_t statsTimeout = 10000;

            enum class PixelFormat
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
                ColorAndTexture
            };

            struct RenderData
            {
                BBox2f bbox = BBox2f(0.f, 0.f, 0.f, 0.f);
                BBox2f clipRect = BBox2f(0.f, 0.f, 0.f, 0.f);
                PixelFormat pixelFormat = PixelFormat::RGBA;
                ColorMode colorMode = ColorMode::SolidColor;
                Color color = Color(1.f, 1.f, 1.f);

                GLint texture = 0;
                FloatRange textureU;
                FloatRange textureV;
            };

            enum class PrimitiveType
            {
                None,
                Rectangle,
                Image,
                Text
            };

            struct Render;

            struct Primitive
            {
                Primitive(Render& render) :
                    render(render)
                {}

                virtual ~Primitive() = 0;

                Render& render;
                PrimitiveType type = PrimitiveType::None;
                BBox2f clipRect = BBox2f(0.f, 0.f, 0.f, 0.f);
                ColorMode colorMode = ColorMode::SolidColor;
                Color color = Color(1.f, 1.f, 1.f);

                virtual void getRenderData(std::vector<RenderData> &) = 0;
            };

            Primitive::~Primitive()
            {}
            
            struct Render
            {
                Render(Context * context)
                {
                    const auto shaderPath = context->getPath(ResourcePath::ShadersDirectory);
                    shader = OpenGL::Shader::create(Shader::create(
                        Path(shaderPath, "djvAVRender2DSystemVertex.glsl"),
                        Path(shaderPath, "djvAVRender2DSystemFragment.glsl")));

                    GLint maxTextureUnits = 0;
                    GLint maxTextureSize = 0;
                    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
                    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
                    {
                        std::stringstream ss;
                        ss << "Maximum OpenGL texture units: " << maxTextureUnits << "\n";
                        ss << "Maximum OpenGL texture size: " << maxTextureSize;
                        context->log("djv::AV::Render2DSystem", ss.str());
                    }
                    const size_t _textureCacheCount = std::min(size_t(maxTextureUnits), textureCacheCount);
                    const int _textureCacheSize = std::min(maxTextureSize, int(textureCacheSize));
                    {
                        std::stringstream ss;
                        ss << "Texture cache count: " << _textureCacheCount << "\n";
                        ss << "Texture cache size: " << _textureCacheSize;
                        context->log("djv::AV::Render2DSystem", ss.str());
                    }
                    staticTextureCache.reset(new TextureCache(
                        _textureCacheCount,
                        _textureCacheSize,
                        Pixel::Type::RGBA_U8,
                        GL_NEAREST,
                        0));
                    dynamicTextureCache.reset(new TextureCache(
                        _textureCacheCount,
                        _textureCacheSize,
                        Pixel::Type::RGBA_U8,
                        GL_NEAREST,
                        0));
                }

                BBox2f viewport;
                std::vector<std::shared_ptr<Primitive> > primitives;
                std::shared_ptr<OpenGL::Shader> shader;
                std::shared_ptr<TextureCache> staticTextureCache;
                std::shared_ptr<TextureCache> dynamicTextureCache;
                std::shared_ptr<TextureCache> glyphTextureCache;
                std::map<UID, uint64_t> staticTextureIDs;
                std::map<UID, uint64_t> dynamicTextureIDs;
                std::map<UID, uint64_t> glyphTextureIDs;
            };

            struct RectanglePrimitive : public Primitive
            {
                RectanglePrimitive(Render& render) :
                    Primitive(render)
                {
                    type = PrimitiveType::Rectangle;
                }

                BBox2f rect = BBox2f(0.f, 0.f, 0.f, 0.f);

                void getRenderData(std::vector<RenderData> & out) override
                {
                    RenderData data;
                    data.bbox = rect;
                    data.clipRect = clipRect;
                    data.colorMode = colorMode;
                    data.color = color;
                    out.push_back(std::move(data));
                }
            };

            struct ImagePrimitive : public Primitive
            {
                ImagePrimitive(Render& render, bool dynamic) :
                    Primitive(render),
                    dynamic(dynamic),
                    textureCache(dynamic ? render.dynamicTextureCache : render.staticTextureCache)
                {
                    type = PrimitiveType::Image;
                }

                UID uid = 0;
                std::shared_ptr<Pixel::Data> pixelData;
                glm::vec2 pos = glm::vec2(0.f, 0.f);
                PixelFormat pixelFormat = PixelFormat::RGBA;
                bool dynamic = true;
                std::shared_ptr<TextureCache> textureCache;

                void getRenderData(std::vector<RenderData> & out) override
                {
                    TextureCacheItem item;
                    if (uid)
                    {
                        uint64_t id = 0;
                        auto & textureIDs = dynamic ? render.dynamicTextureIDs : render.staticTextureIDs;
                        const auto i = textureIDs.find(uid);
                        if (i != textureIDs.end())
                        {
                            id = i->second;
                        }
                        if (!textureCache->getItem(id, item))
                        {
                            textureIDs[uid] = textureCache->addItem(pixelData, item);
                        }
                    }
                    else
                    {
                        textureCache->addItem(pixelData, item);
                    }

                    RenderData data;
                    data.bbox = BBox2f(pos.x, pos.y, static_cast<float>(item.size.x), static_cast<float>(item.size.y));
                    data.clipRect = clipRect;
                    const auto& info = pixelData->getInfo();
                    switch (info.getGLFormat())
                    {
                    case GL_RED:  data.pixelFormat = PixelFormat::L;    break;
                    case GL_RG:   data.pixelFormat = PixelFormat::LA;   break;
                    case GL_RGB:  data.pixelFormat = PixelFormat::RGB;  break;
                    case GL_RGBA: data.pixelFormat = PixelFormat::RGBA; break;
                    default: break;
                    }
                    data.colorMode = colorMode;
                    data.color = color;
                    data.texture =
                        static_cast<GLint>((dynamic ? render.staticTextureCache->getTextureCount() : 0) +
                        item.texture);
                    if (info.layout.mirror.x)
                    {
                        data.textureU.min = item.textureU.max;
                        data.textureU.max = item.textureU.min;
                    }
                    else
                    {
                        data.textureU = item.textureU;
                    }
                    if (info.layout.mirror.y)
                    {
                        data.textureV = item.textureV;
                    }
                    else
                    {
                        data.textureV.min = item.textureV.max;
                        data.textureV.max = item.textureV.min;
                    }
                    out.push_back(std::move(data));
                }
            };

            struct TextPrimitive : public Primitive
            {
                TextPrimitive(Render& render) :
                    Primitive(render)
                {
                    type = PrimitiveType::Text;
                }

                std::string text;
                Font font;
                std::future<std::vector<std::shared_ptr<FontGlyph> > > glyphsFuture;
                glm::vec2 pos = glm::vec2(0.f, 0.f);

                void getRenderData(std::vector<RenderData> & out) override
                {
                    float x = 0.f;
                    float y = 0.f;
                    const auto glyphs = glyphsFuture.get();
                    const size_t outSize = out.size();
                    out.resize(out.size() + glyphs.size());
                    auto it = out.begin() + outSize;
                    for (const auto& glyph : glyphs)
                    {
                        const glm::vec2& size = glyph->pixelData->getSize();
                        BBox2f bbox(
                            pos.x + x + glyph->offset.x,
                            pos.y + y - glyph->offset.y,
                            size.x,
                            size.y);
                        if (bbox.intersects(render.viewport))
                        {
                            const auto uid = getFontGlyphUID(glyph->code, font);
                            uint64_t id = 0;
                            const auto i = render.glyphTextureIDs.find(uid);
                            if (i != render.glyphTextureIDs.end())
                            {
                                id = i->second;
                            }
                            TextureCacheItem item;
                            if (!render.staticTextureCache->getItem(id, item))
                            {
                                id = render.staticTextureCache->addItem(glyph->pixelData, item);
                                render.glyphTextureIDs[uid] = id;
                            }

                            it->bbox = bbox;
                            it->clipRect = clipRect;
                            it->colorMode = colorMode;
                            it->color = color;
                            it->texture = static_cast<GLint>(item.texture);
                            it->textureU = item.textureU;
                            it->textureV = item.textureV;
                            ++it;
                        }
                        x += glyph->advance;
                    }
                    out.resize(it - out.begin());
                }
            };

            BBox2f flip(const BBox2f& value, const glm::ivec2& size)
            {
                BBox2f out;
                out.min.x = value.min.x;
                out.min.y = static_cast<float>(size.y) - value.max.y;
                out.max.x = value.max.x;
                out.max.y = static_cast<float>(size.y) - value.min.y;
                return out;
            }

        } // namespace

        struct Render2DSystem::Private
        {
            glm::ivec2 size = glm::ivec2(0, 0);
            std::list<BBox2f> clipRects;
            BBox2f currentClipRect = BBox2f(0.f, 0.f, 0.f, 0.f);
            Color fillColor = Color(1.f, 1.f, 1.f);
            std::weak_ptr<FontSystem> fontSystem;
            Font currentFont;
            FontMetrics currentFontMetrics;

            std::unique_ptr<Render> render;

            std::shared_ptr<Timer> statsTimer;
            std::shared_ptr<Timer> fpsTimer;
            std::vector<float> fpsSamples;
            std::chrono::time_point<std::chrono::system_clock> fpsTime = std::chrono::system_clock::now();

            void updateCurrentClipRect();
        };

        void Render2DSystem::_init(Context * context)
        {
            ISystem::_init("djv::AV::Render2DSystem", context);

            DJV_PRIVATE_PTR();
            p.fontSystem = context->getSystemT<FontSystem>();

            p.render.reset(new Render(context));

            p.statsTimer = Timer::create(context);
            p.statsTimer->setRepeating(true);
            p.statsTimer->start(
                std::chrono::milliseconds(statsTimeout),
                [this](float)
            {
                DJV_PRIVATE_PTR();
                std::stringstream s;
                s << "Static texture cache: " << p.render->staticTextureCache->getPercentageUsed() << "%\n";
                s << "Dynamic texture cache: " << p.render->dynamicTextureCache->getPercentageUsed() << "%";
                _log(s.str());
            });

            p.fpsTimer = Timer::create(context);
            p.fpsTimer->setRepeating(true);
            p.fpsTimer->start(
                std::chrono::milliseconds(10000),
                [this](float)
            {
                DJV_PRIVATE_PTR();
                float average = 1.f;
                for (const auto& i : p.fpsSamples)
                {
                    average += i;
                }
                average /= static_cast<float>(p.fpsSamples.size());
                std::stringstream s;
                s << "FPS: " << average;
                _log(s.str());
            });
        }

        Render2DSystem::Render2DSystem() :
            _p(new Private)
        {}

        Render2DSystem::~Render2DSystem()
        {}

        std::shared_ptr<Render2DSystem> Render2DSystem::create(Context * context)
        {
            auto out = std::shared_ptr<Render2DSystem>(new Render2DSystem);
            out->_init(context);
            return out;
        }

        void Render2DSystem::beginFrame(const glm::ivec2& size)
        {
            DJV_PRIVATE_PTR();
            p.size = size;
            p.currentClipRect = BBox2f(0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y));
            p.render->viewport = BBox2f(0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y));
        }

        void Render2DSystem::endFrame()
        {
            DJV_PRIVATE_PTR();
            std::vector<RenderData> renderData;
            for (auto& primitive : p.render->primitives)
            {
                primitive->getRenderData(renderData);
            }

            TriangleMesh mesh;
            mesh.v.reserve(renderData.size() * 4);
            mesh.t.reserve(renderData.size() * 4);
            mesh.triangles.reserve(renderData.size() * 2);
            size_t quadsCount = 0;
            std::vector<const RenderData *> clippedRenderData;
            clippedRenderData.reserve(renderData.size());
            for (const auto& data : renderData)
            {
                const BBox2f bbox = flip(data.bbox, p.size);
                if (bbox.intersects(p.render->viewport))
                {
                    mesh.v.push_back(glm::vec3(floorf(bbox.min.x), floorf(bbox.min.y), 0.f));
                    mesh.v.push_back(glm::vec3(floorf(bbox.max.x), floorf(bbox.min.y), 0.f));
                    mesh.v.push_back(glm::vec3(floorf(bbox.max.x), floorf(bbox.max.y), 0.f));
                    mesh.v.push_back(glm::vec3(floorf(bbox.min.x), floorf(bbox.max.y), 0.f));
                    mesh.t.push_back(glm::vec2(data.textureU.min, data.textureV.min));
                    mesh.t.push_back(glm::vec2(data.textureU.max, data.textureV.min));
                    mesh.t.push_back(glm::vec2(data.textureU.max, data.textureV.max));
                    mesh.t.push_back(glm::vec2(data.textureU.min, data.textureV.max));

                    TriangleMesh::Triangle triangle;
                    triangle.v0 = TriangleMesh::Vertex(quadsCount * 4 + 1, quadsCount * 4 + 1);
                    triangle.v1 = TriangleMesh::Vertex(quadsCount * 4 + 2, quadsCount * 4 + 2);
                    triangle.v2 = TriangleMesh::Vertex(quadsCount * 4 + 3, quadsCount * 4 + 3);
                    mesh.triangles.push_back(std::move(triangle));
                    TriangleMesh::Triangle triangle2;
                    triangle.v0 = TriangleMesh::Vertex(quadsCount * 4 + 3, quadsCount * 4 + 3);
                    triangle.v1 = TriangleMesh::Vertex(quadsCount * 4 + 4, quadsCount * 4 + 4);
                    triangle.v2 = TriangleMesh::Vertex(quadsCount * 4 + 1, quadsCount * 4 + 1);
                    mesh.triangles.push_back(std::move(triangle));
                    ++quadsCount;

                    clippedRenderData.push_back(&data);
                }
            }

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
            //glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
            glEnable(GL_SCISSOR_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            p.render->shader->bind();
            const auto viewMatrix = glm::ortho(
                p.render->viewport.min.x,
                p.render->viewport.max.x,
                p.render->viewport.min.y,
                p.render->viewport.max.y,
                -1.f, 1.f);
            p.render->shader->setUniform("transform.mvp", viewMatrix);

            const auto& staticTextures = p.render->staticTextureCache->getTextures();
            size_t i = 0;
            for (; i < staticTextures.size(); ++i)
            {
                glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
                glBindTexture(GL_TEXTURE_2D, staticTextures[i]);
            }
            const auto& dynamicTextures = p.render->dynamicTextureCache->getTextures();
            for (size_t j = 0; j < dynamicTextures.size(); ++i, ++j)
            {
                glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
                glBindTexture(GL_TEXTURE_2D, dynamicTextures[j]);
            }

            auto vbo = OpenGL::VBO::create(quadsCount * 2, 3, OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
            vbo->copy(OpenGL::VBO::convert(mesh, vbo->getType()));
            auto vao = OpenGL::VAO::create(vbo->getType(), vbo->getID());
            vao->bind();

            auto pixelFormat = static_cast<PixelFormat>(0);
            auto colorMode = static_cast<ColorMode>(0);
            AV::Color color;
            GLint texture = 0;
            for (size_t i = 0; i < clippedRenderData.size(); ++i)
            {
                const auto * data = clippedRenderData[i];
                //! \todo [1.0 M] Should we do our own clipping?
                const BBox2f clipRect = flip(data->clipRect, p.size);
                glScissor(
                    static_cast<GLint>(clipRect.min.x),
                    static_cast<GLint>(clipRect.min.y),
                    static_cast<GLsizei>(clipRect.w()),
                    static_cast<GLsizei>(clipRect.h()));
                if (i == 0 || data->pixelFormat != pixelFormat)
                {
                    pixelFormat = data->pixelFormat;
                    p.render->shader->setUniform("pixelFormat", static_cast<int>(data->pixelFormat));
                }
                if (i == 0 || data->colorMode != colorMode)
                {
                    colorMode = data->colorMode;
                    p.render->shader->setUniform("colorMode", static_cast<int>(data->colorMode));
                }
                if (i == 0 || data->color != color)
                {
                    color = data->color;
                    p.render->shader->setUniform("color", data->color);
                }
                if (i == 0 || data->texture != texture)
                {
                    texture = data->texture;
                    p.render->shader->setUniform("textureSampler", data->texture);
                }
                vao->draw(i * 6, 6);
            }

            //glPopAttrib();

            const auto now = std::chrono::system_clock::now();
            const std::chrono::duration<float> delta = now - p.fpsTime;
            p.fpsTime = now;
            p.fpsSamples.push_back(1.f / delta.count());
            while (p.fpsSamples.size() > 10)
            {
                p.fpsSamples.erase(p.fpsSamples.begin());
            }

            p.clipRects.clear();
            p.render->primitives.clear();
        }

        void Render2DSystem::pushClipRect(const BBox2f& value)
        {
            DJV_PRIVATE_PTR();
            p.clipRects.push_back(value);
            p.updateCurrentClipRect();
        }

        void Render2DSystem::popClipRect()
        {
            DJV_PRIVATE_PTR();
            p.clipRects.pop_back();
            p.updateCurrentClipRect();
        }

        void Render2DSystem::setFillColor(const Color& value)
        {
            DJV_PRIVATE_PTR();
            p.fillColor = value;
        }

        void Render2DSystem::drawRectangle(const BBox2f& value)
        {
            DJV_PRIVATE_PTR();
            auto primitive = std::unique_ptr<RectanglePrimitive>(new RectanglePrimitive(*p.render));
            primitive->rect = value;
            primitive->clipRect = p.currentClipRect;
            primitive->colorMode = ColorMode::SolidColor;
            primitive->color = p.fillColor;
            p.render->primitives.push_back(std::move(primitive));
        }

        void Render2DSystem::drawImage(const std::shared_ptr<Pixel::Data>& data, const glm::vec2& pos, bool dynamic, UID uid)
        {
            DJV_PRIVATE_PTR();
            auto primitive = std::unique_ptr<ImagePrimitive>(new ImagePrimitive(*p.render, dynamic));
            primitive->uid = uid;
            primitive->pixelData = data;
            primitive->pos = pos;
            primitive->clipRect = p.currentClipRect;
            primitive->colorMode = ColorMode::ColorAndTexture;
            primitive->color = p.fillColor;
            p.render->primitives.push_back(std::move(primitive));
        }

        void Render2DSystem::drawFilledImage(const std::shared_ptr<Pixel::Data>& data, const glm::vec2& pos, bool dynamic, UID uid)
        {
            DJV_PRIVATE_PTR();
            auto primitive = std::unique_ptr<ImagePrimitive>(new ImagePrimitive(*p.render, dynamic));
            primitive->uid = uid;
            primitive->pixelData = data;
            primitive->pos = pos;
            primitive->clipRect = p.currentClipRect;
            primitive->colorMode = ColorMode::ColorWithTextureAlpha;
            primitive->color = p.fillColor;
            p.render->primitives.push_back(std::move(primitive));
        }

        void Render2DSystem::setCurrentFont(const Font& value)
        {
            DJV_PRIVATE_PTR();
            p.currentFont = value;
        }

        void Render2DSystem::drawText(const std::string& value, const glm::vec2& pos, size_t maxLineWidth)
        {
            DJV_PRIVATE_PTR();
            if (auto fontSystem = p.fontSystem.lock())
            {
                auto primitive = std::unique_ptr<TextPrimitive>(new TextPrimitive(*p.render));
                primitive->text = value;
                primitive->font = p.currentFont;
                primitive->pos = pos;
                primitive->glyphsFuture = fontSystem->getGlyphs(value, p.currentFont);
                primitive->clipRect = p.currentClipRect;
                primitive->colorMode = ColorMode::ColorWithTextureAlpha;
                primitive->color = p.fillColor;
                p.render->primitives.push_back(std::move(primitive));
            }
        }

        void Render2DSystem::Private::updateCurrentClipRect()
        {
            BBox2f clipRect = BBox2f(0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y));
            for (const auto& i : clipRects)
            {
                clipRect = clipRect.intersect(i);
            }
            currentClipRect = clipRect;
        }

    } // namespace AV
} // namespace djv
