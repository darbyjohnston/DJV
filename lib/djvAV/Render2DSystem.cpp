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
        namespace Render
        {
            namespace
            {
                //! \todo [1.0 S] Should this be configurable?
                const size_t textureCacheSize = 8192;
                const size_t textureCacheCount = 4;

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
                    ColorAndTexture
                };

                struct RenderData
                {
                    BBox2f bbox = BBox2f(0.f, 0.f, 0.f, 0.f);
                    BBox2f clipRect = BBox2f(0.f, 0.f, 0.f, 0.f);
                    ImageFormat imageFormat = ImageFormat::RGBA;
                    ColorMode colorMode = ColorMode::SolidColor;
                    Image::Color color = Image::Color(1.f, 1.f, 1.f);

                    GLint texture = 0;
                    Range::FloatRange textureU;
                    Range::FloatRange textureV;
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
                    Image::Color color = Image::Color(1.f, 1.f, 1.f);

                    virtual void getRenderData(std::vector<RenderData> &) = 0;
                };

                Primitive::~Primitive()
                {}

                struct Render
                {
                    Render(Context * context)
                    {
                        const auto shaderPath = context->getPath(FileSystem::ResourcePath::ShadersDirectory);
                        shader = OpenGL::Shader::create(Shader::create(
                            FileSystem::Path(shaderPath, "djvAVRender2DSystemVertex.glsl"),
                            FileSystem::Path(shaderPath, "djvAVRender2DSystemFragment.glsl")));

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
                            Image::Type::RGBA_U8,
                            GL_NEAREST,
                            0));
                        dynamicTextureCache.reset(new TextureCache(
                            _textureCacheCount,
                            _textureCacheSize,
                            Image::Type::RGBA_U8,
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
                    ImagePrimitive(Render& render, Render2DSystem::ImageType imageType) :
                        Primitive(render),
                        imageType(imageType),
                        textureCache(Render2DSystem::ImageType::Static == imageType ? render.staticTextureCache : render.dynamicTextureCache)
                    {
                        type = PrimitiveType::Image;
                    }

                    std::shared_ptr<Image::Data> imageData;
                    BBox2f rect = BBox2f(0.f, 0.f, 0.f, 0.f);
                    ImageFormat imageFormat = ImageFormat::RGBA;
                    Render2DSystem::ImageType imageType = Render2DSystem::ImageType::Static;
                    std::shared_ptr<TextureCache> textureCache;

                    void getRenderData(std::vector<RenderData> & out) override
                    {
                        TextureCacheItem item;
                        const UID uid = imageData->getUID();
                        if (uid)
                        {
                            uint64_t id = 0;
                            auto & textureIDs = Render2DSystem::ImageType::Static == imageType ? render.staticTextureIDs : render.dynamicTextureIDs;
                            const auto i = textureIDs.find(uid);
                            if (i != textureIDs.end())
                            {
                                id = i->second;
                            }
                            if (!textureCache->getItem(id, item))
                            {
                                textureIDs[uid] = textureCache->addItem(imageData, item);
                            }
                        }
                        else
                        {
                            textureCache->addItem(imageData, item);
                        }

                        RenderData data;
                        data.bbox = rect;
                        data.clipRect = clipRect;
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
                        data.color = color;
                        data.texture =
                            static_cast<GLint>((Render2DSystem::ImageType::Dynamic == imageType ? render.staticTextureCache->getTextureCount() : 0) +
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
                    Font::Info fontInfo;
                    std::future<std::vector<std::shared_ptr<Font::Glyph> > > glyphsFuture;
                    glm::vec2 pos = glm::vec2(0.f, 0.f);

                    void getRenderData(std::vector<RenderData> & out) override
                    {
                        try
                        {
                            float x = 0.f;
                            float y = 0.f;
                            const auto glyphs = glyphsFuture.get();
                            const size_t outSize = out.size();
                            out.resize(out.size() + glyphs.size());
                            auto it = out.begin() + outSize;
                            for (const auto& glyph : glyphs)
                            {
                                const glm::vec2& size = glyph->getImageData()->getSize();
                                const glm::vec2& offset = glyph->getOffset();
                                BBox2f bbox(
                                    pos.x + x + offset.x,
                                    pos.y + y - offset.y,
                                    size.x,
                                    size.y);
                                if (bbox.intersects(render.viewport))
                                {
                                    const auto uid = glyph->getInfo().getUID();
                                    uint64_t id = 0;
                                    const auto i = render.glyphTextureIDs.find(uid);
                                    if (i != render.glyphTextureIDs.end())
                                    {
                                        id = i->second;
                                    }
                                    TextureCacheItem item;
                                    if (!render.staticTextureCache->getItem(id, item))
                                    {
                                        id = render.staticTextureCache->addItem(glyph->getImageData(), item);
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
                                x += glyph->getAdvance();
                            }
                            out.resize(it - out.begin());
                        }
                        catch (const std::exception &)
                        {}
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
                Image::Color fillColor = Image::Color(1.f, 1.f, 1.f);
                std::weak_ptr<Font::System> fontSystem;
                Font::Info currentFont;
                Font::Metrics currentFontMetrics;

                std::unique_ptr<Render> render;

                std::shared_ptr<Time::Timer> statsTimer;
                std::shared_ptr<Time::Timer> fpsTimer;
                std::vector<float> fpsSamples;
                std::chrono::time_point<std::chrono::system_clock> fpsTime = std::chrono::system_clock::now();

                void updateCurrentClipRect();
            };

            void Render2DSystem::_init(Context * context)
            {
                ISystem::_init("djv::AV::Render2DSystem", context);

                DJV_PRIVATE_PTR();
                p.fontSystem = context->getSystemT<Font::System>();

                p.render.reset(new Render(context));

                p.statsTimer = Time::Timer::create(context);
                p.statsTimer->setRepeating(true);
                p.statsTimer->start(
                    Time::Timer::getMilliseconds(Time::Timer::Value::VerySlow),
                    [this](float)
                {
                    DJV_PRIVATE_PTR();
                    std::stringstream s;
                    s << "Static texture cache: " << p.render->staticTextureCache->getPercentageUsed() << "%\n";
                    s << "Dynamic texture cache: " << p.render->dynamicTextureCache->getPercentageUsed() << "%";
                    _log(s.str());
                });

                p.fpsTimer = Time::Timer::create(context);
                p.fpsTimer->setRepeating(true);
                p.fpsTimer->start(
                    Time::Timer::getMilliseconds(Time::Timer::Value::VerySlow),
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

                Geom::TriangleMesh mesh;
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
                        mesh.v.push_back(glm::vec3(bbox.min.x, bbox.min.y, 0.f));
                        mesh.v.push_back(glm::vec3(bbox.max.x, bbox.min.y, 0.f));
                        mesh.v.push_back(glm::vec3(bbox.max.x, bbox.max.y, 0.f));
                        mesh.v.push_back(glm::vec3(bbox.min.x, bbox.max.y, 0.f));
                        mesh.t.push_back(glm::vec2(data.textureU.min, data.textureV.min));
                        mesh.t.push_back(glm::vec2(data.textureU.max, data.textureV.min));
                        mesh.t.push_back(glm::vec2(data.textureU.max, data.textureV.max));
                        mesh.t.push_back(glm::vec2(data.textureU.min, data.textureV.max));

                        Geom::TriangleMesh::Triangle triangle;
                        triangle.v0 = Geom::TriangleMesh::Vertex(quadsCount * 4 + 1, quadsCount * 4 + 1);
                        triangle.v1 = Geom::TriangleMesh::Vertex(quadsCount * 4 + 2, quadsCount * 4 + 2);
                        triangle.v2 = Geom::TriangleMesh::Vertex(quadsCount * 4 + 3, quadsCount * 4 + 3);
                        mesh.triangles.push_back(std::move(triangle));
                        Geom::TriangleMesh::Triangle triangle2;
                        triangle.v0 = Geom::TriangleMesh::Vertex(quadsCount * 4 + 3, quadsCount * 4 + 3);
                        triangle.v1 = Geom::TriangleMesh::Vertex(quadsCount * 4 + 4, quadsCount * 4 + 4);
                        triangle.v2 = Geom::TriangleMesh::Vertex(quadsCount * 4 + 1, quadsCount * 4 + 1);
                        mesh.triangles.push_back(std::move(triangle));
                        ++quadsCount;

                        clippedRenderData.push_back(&data);
                    }
                }

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
                    p.render->viewport.min.y,
                    p.render->viewport.max.y,
                    -1.f, 1.f);
                p.render->shader->setUniform("transform.mvp", viewMatrix);
                const auto program = p.render->shader->getProgram();
                const GLint imageFormatLoc = glGetUniformLocation(program, "imageFormat");
                const GLint colorModeLoc = glGetUniformLocation(program, "colorMode");
                const GLint colorLoc = glGetUniformLocation(program, "color");
                const GLint textureSamplerLoc = glGetUniformLocation(program, "textureSampler");

                const auto& staticTextures = p.render->staticTextureCache->getTextures();
                GLuint i = 0;
                for (; i < static_cast<GLuint>(staticTextures.size()); ++i)
                {
                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
                    glBindTexture(GL_TEXTURE_2D, staticTextures[i]);
                }
                const auto& dynamicTextures = p.render->dynamicTextureCache->getTextures();
                for (GLuint j = 0; j < static_cast<GLuint>(dynamicTextures.size()); ++i, ++j)
                {
                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
                    glBindTexture(GL_TEXTURE_2D, dynamicTextures[j]);
                }

                auto vbo = OpenGL::VBO::create(quadsCount * 2, 3, OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                vbo->copy(OpenGL::VBO::convert(mesh, vbo->getType()));
                auto vao = OpenGL::VAO::create(vbo->getType(), vbo->getID());
                vao->bind();

                auto imageFormat = static_cast<ImageFormat>(0);
                auto colorMode = static_cast<ColorMode>(0);
                Image::Color color;
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
                    if (i == 0 || data->imageFormat != imageFormat)
                    {
                        imageFormat = data->imageFormat;
                        p.render->shader->setUniform(imageFormatLoc, static_cast<int>(data->imageFormat));
                    }
                    if (i == 0 || data->colorMode != colorMode)
                    {
                        colorMode = data->colorMode;
                        p.render->shader->setUniform(colorModeLoc, static_cast<int>(data->colorMode));
                    }
                    if (i == 0 || data->color != color)
                    {
                        color = data->color;
                        p.render->shader->setUniform(colorLoc, data->color);
                    }
                    if (i == 0 || data->texture != texture)
                    {
                        texture = data->texture;
                        p.render->shader->setUniform(textureSamplerLoc, data->texture);
                    }
                    vao->draw(i * 6, 6);
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

            void Render2DSystem::setFillColor(const Image::Color& value)
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

            void Render2DSystem::drawImage(const std::shared_ptr<Image::Data>& data, const BBox2f & rect, ImageType imageType)
            {
                DJV_PRIVATE_PTR();
                auto primitive = std::unique_ptr<ImagePrimitive>(new ImagePrimitive(*p.render, imageType));
                primitive->imageData = data;
                primitive->rect = rect;
                primitive->clipRect = p.currentClipRect;
                primitive->colorMode = ColorMode::ColorAndTexture;
                primitive->color = p.fillColor;
                p.render->primitives.push_back(std::move(primitive));
            }

            void Render2DSystem::drawFilledImage(const std::shared_ptr<Image::Data>& data, const BBox2f & rect, ImageType imageType)
            {
                DJV_PRIVATE_PTR();
                auto primitive = std::unique_ptr<ImagePrimitive>(new ImagePrimitive(*p.render, imageType));
                primitive->imageData = data;
                primitive->rect = rect;
                primitive->clipRect = p.currentClipRect;
                primitive->colorMode = ColorMode::ColorWithTextureAlpha;
                primitive->color = p.fillColor;
                p.render->primitives.push_back(std::move(primitive));
            }

            void Render2DSystem::setCurrentFont(const Font::Info& value)
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
                    primitive->fontInfo = p.currentFont;
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

        } // namespace Render
    } // namespace AV
} // namespace djv
