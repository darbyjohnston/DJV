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

#include <djvCore/Cache.h>
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

namespace djv
{
    namespace AV
    {
        namespace Render
        {
            namespace
            {
                //! \todo [1.0 S] Should this be configurable?
                const size_t primitiveSize = 10000;
                const size_t textureAtlasCount = 4;
                const size_t textureAtlasSize = 8192;
                const size_t dynamicTextureCacheCount = 16;
                const size_t vboSize = 10000;

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
                    ColorAndTexture,
                    Shadow
                };

                struct Primitive
                {
                    BBox2f       clipRect;
                    ImageFormat  imageFormat = ImageFormat::RGBA;
                    ColorMode    colorMode = ColorMode::SolidColor;
                    float        color[4] = { 0.f, 0.f, 0.f, 0.f };
                    ImageChannel imageChannel = ImageChannel::Color;
                    ImageCache   imageCache = ImageCache::Atlas;
                    size_t       atlasIndex = 0;
                    GLuint       textureID = 0;
                    size_t       vao = 0;
                    size_t       vaoOffset = 0;
                    size_t       vaoSize = 0;
                };

                struct VBOVertex
                {
                    float    vx;
                    float    vy;
                    uint16_t tx;
                    uint16_t ty;

                    void setTextureCoord(float tx, float ty)
                    {
                        this->tx = Math::clamp(static_cast<int>(tx * 65535.f), 0, 65535);
                        this->ty = Math::clamp(static_cast<int>(ty * 65535.f), 0, 65535);
                    }
                };

                struct Render
                {
                    Render(Context* context)
                    {
                        primitives.resize(primitiveSize);

                        GLint maxTextureUnits = 0;
                        GLint maxTextureSize = 0;
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
                        vbo = OpenGL::VBO::create(vboSize, 3, OpenGL::VBOType::Pos2_F32_UV_U16);
                        vao = OpenGL::VAO::create(vbo->getType(), vbo->getID());

                        auto resourceSystem = context->getSystemT<ResourceSystem>();
                        const FileSystem::Path shaderPath = resourceSystem->getPath(FileSystem::ResourcePath::ShadersDirectory);
                        shader = OpenGL::Shader::create(Shader::create(
                            FileSystem::Path(shaderPath, "djvAVRender2DVertex.glsl"),
                            FileSystem::Path(shaderPath, "djvAVRender2DFragment.glsl")));
                    }

                    BBox2f                                           viewport;
                    std::vector<Primitive>                           primitives;
                    size_t                                           primitivesSize = 0;
                    std::shared_ptr<TextureAtlas>                    textureAtlas;
                    std::map<UID, uint64_t>                          textureIDs;
                    std::map<UID, uint64_t>                          glyphTextureIDs;
                    std::map<UID, std::shared_ptr<OpenGL::Texture> > dynamicTextureCache;
                    std::vector<uint8_t>                             vboData;
                    size_t                                           vboDataSize = 0;
                    std::shared_ptr<OpenGL::VBO>                     vbo;
                    std::shared_ptr<OpenGL::VAO>                     vao;
                    std::shared_ptr<OpenGL::Shader>                  shader;
                };

                BBox2f flip(const BBox2f& value, const Image::Size& size)
                {
                    BBox2f out;
                    out.min.x = value.min.x;
                    out.min.y = static_cast<float>(size.h) - value.max.y;
                    out.max.x = value.max.x;
                    out.max.y = static_cast<float>(size.h) - value.min.y;
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
                Image::Size                  size;
                std::list<glm::mat3x3>       transforms;
                glm::mat3x3                  currentTransform = glm::mat3x3(1.f);
                std::list<BBox2f>            clipRects;
                BBox2f                       currentClipRect = BBox2f(0.f, 0.f, 0.f, 0.f);
                float                        fillColor[4] = { 1.f, 1.f, 1.f, 1.f };
                float                        colorMult = 1.f;
                float                        alphaMult = 1.f;
                float                        finalColor[4] = { 1.f, 1.f, 1.f, 1.f };
                std::weak_ptr<Font::System>  fontSystem;
                Font::Info                   currentFont;

                std::unique_ptr<Render>      render;

                std::shared_ptr<Time::Timer> statsTimer;
                std::vector<float>           fpsSamples;
                std::chrono::time_point<std::chrono::system_clock>
                    fpsTime = std::chrono::system_clock::now();

                void updateCurrentTransform();
                void updateCurrentClipRect();
                void drawRoundedRect(const Core::BBox2f&, float radius, size_t facets);
                void drawImage(
                    const std::shared_ptr<Image::Data>&,
                    const glm::vec2& pos,
                    const ImageOptions&,
                    ColorMode);

                void updatePrimitivesSize(size_t);
                void updateVBODataSize(size_t);
            };

            void Render2D::_init(Context* context)
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
                        s << "VBO size: " << p.render->vbo->getSize();
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

            std::shared_ptr<Render2D> Render2D::create(Context* context)
            {
                auto out = std::shared_ptr<Render2D>(new Render2D);
                out->_init(context);
                return out;
            }

            void Render2D::beginFrame(const Image::Size& size)
            {
                DJV_PRIVATE_PTR();
                p.size = size;
                p.currentClipRect = BBox2f(0.f, 0.f, static_cast<float>(size.w), static_cast<float>(size.h));
                p.render->viewport = BBox2f(0.f, 0.f, static_cast<float>(size.w), static_cast<float>(size.h));
            }

            void Render2D::endFrame()
            {
                DJV_PRIVATE_PTR();

#if defined(DJV_OPENGL_ES2)
#else // DJV_OPENGL_ES2
                glEnable(GL_MULTISAMPLE);
#endif // DJV_OPENGL_ES2
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
                const GLint imageChannelLoc = glGetUniformLocation(program, "imageChannel");
                const GLint colorModeLoc = glGetUniformLocation(program, "colorMode");
                const GLint colorLoc = glGetUniformLocation(program, "color");
                const GLint textureSamplerLoc = glGetUniformLocation(program, "textureSampler");

                const auto& atlasTextures = p.render->textureAtlas->getTextures();
                const size_t atlasTexturesCount = atlasTextures.size();
                for (GLuint i = 0; i < static_cast<GLuint>(atlasTexturesCount); ++i)
                {
                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
                    glBindTexture(GL_TEXTURE_2D, atlasTextures[i]);
                }

                const size_t vertexByteCount = AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                if (p.render->vboDataSize / 3 / vertexByteCount > p.render->vbo->getSize())
                {
                    p.render->vbo = OpenGL::VBO::create(p.render->vboDataSize / 3 / vertexByteCount, 3, OpenGL::VBOType::Pos2_F32_UV_U16);
                    p.render->vao = OpenGL::VAO::create(p.render->vbo->getType(), p.render->vbo->getID());
                }
                p.render->vbo->copy(p.render->vboData, 0, p.render->vboDataSize);
                p.render->vao->bind();

                auto imageFormat = static_cast<ImageFormat>(0);
                auto imageChannel = static_cast<ImageChannel>(0);
                auto colorMode = static_cast<ColorMode>(0);
                float color[] = { 0.f, 0.f, 0.f, 0.f };
                ImageCache imageCache = ImageCache::Atlas;
                size_t atlasIndex = 0;
                GLuint textureID = 0;

                for (size_t i = 0; i < p.render->primitivesSize; ++i)
                {
                    const auto& primitive = p.render->primitives[i];
                    const BBox2f clipRect = flip(primitive.clipRect, p.size);
                    glScissor(
                        static_cast<GLint>(clipRect.min.x),
                        static_cast<GLint>(clipRect.min.y),
                        static_cast<GLsizei>(clipRect.w()),
                        static_cast<GLsizei>(clipRect.h()));
                    if (0 == i || primitive.imageFormat != imageFormat)
                    {
                        imageFormat = primitive.imageFormat;
                        p.render->shader->setUniform(imageFormatLoc, static_cast<int>(primitive.imageFormat));
                    }
                    if (0 == i || primitive.imageChannel != imageChannel)
                    {
                        imageChannel = primitive.imageChannel;
                        p.render->shader->setUniform(imageChannelLoc, static_cast<int>(primitive.imageChannel));
                    }
                    if (0 == i || primitive.colorMode != colorMode)
                    {
                        colorMode = primitive.colorMode;
                        p.render->shader->setUniform(colorModeLoc, static_cast<int>(primitive.colorMode));
                    }
                    if (0 == i ||
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
                        if (0 == i || primitive.imageCache != imageCache || primitive.atlasIndex != atlasIndex)
                        {
                            imageCache = primitive.imageCache;
                            atlasIndex = primitive.atlasIndex;
                            p.render->shader->setUniform(textureSamplerLoc, static_cast<int>(primitive.atlasIndex));
                        }
                        break;
                    case ImageCache::Dynamic:
                        if (0 == i || primitive.imageCache != imageCache || primitive.textureID != textureID)
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
                        p.render->vao->draw(primitive.vaoOffset, primitive.vaoSize);
                        p.render->shader->setUniform(colorModeLoc, static_cast<int>(ColorMode::ColorWithTextureAlphaG));
                        glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE);
                        p.render->vao->draw(primitive.vaoOffset, primitive.vaoSize);
                        p.render->shader->setUniform(colorModeLoc, static_cast<int>(ColorMode::ColorWithTextureAlphaB));
                        glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
                        p.render->vao->draw(primitive.vaoOffset, primitive.vaoSize);
                        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                        break;
                    default:
                        p.render->vao->draw(primitive.vaoOffset, primitive.vaoSize);
                        break;
                    }
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
                p.render->vboDataSize = 0;
                while (p.render->dynamicTextureCache.size() > dynamicTextureCacheCount)
                {
                    p.render->dynamicTextureCache.erase(p.render->dynamicTextureCache.begin());
                }
            }

            void Render2D::pushTransform(const glm::mat3x3& value)
            {
                DJV_PRIVATE_PTR();
                p.transforms.push_back(value);
                p.updateCurrentTransform();
            }

            void Render2D::popTransform()
            {
                DJV_PRIVATE_PTR();
                p.transforms.pop_back();
                p.updateCurrentTransform();
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
                if (Image::Type::RGBA_F32 == value.getType())
                {
                    const float* d = reinterpret_cast<const float*>(value.getData());
                    p.fillColor[0] = d[0];
                    p.fillColor[1] = d[1];
                    p.fillColor[2] = d[2];
                    p.fillColor[3] = d[3];
                }
                else
                {
                    const Image::Color tmp = value.convert(Image::Type::RGBA_F32);
                    const float* d = reinterpret_cast<const float*>(tmp.getData());
                    p.fillColor[0] = d[0];
                    p.fillColor[1] = d[1];
                    p.fillColor[2] = d[2];
                    p.fillColor[3] = d[3];
                }
                p.finalColor[0] = p.fillColor[0] * p.colorMult;
                p.finalColor[1] = p.fillColor[1] * p.colorMult;
                p.finalColor[2] = p.fillColor[2] * p.colorMult;
                p.finalColor[3] = p.fillColor[3] * p.alphaMult;
            }

            void Render2D::setColorMult(float value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.colorMult)
                    return;
                p.colorMult = value;
                p.finalColor[0] = p.fillColor[0] * p.colorMult;
                p.finalColor[1] = p.fillColor[1] * p.colorMult;
                p.finalColor[2] = p.fillColor[2] * p.colorMult;
                p.finalColor[3] = p.fillColor[3] * p.alphaMult;
            }

            void Render2D::setAlphaMult(float value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.alphaMult)
                    return;
                p.alphaMult = value;
                p.finalColor[0] = p.fillColor[0] * p.colorMult;
                p.finalColor[1] = p.fillColor[1] * p.colorMult;
                p.finalColor[2] = p.fillColor[2] * p.colorMult;
                p.finalColor[3] = p.fillColor[3] * p.alphaMult;
            }

            void Render2D::drawRect(const BBox2f & value)
            {
                DJV_PRIVATE_PTR();
                if (value.intersects(p.render->viewport))
                {
                    const size_t primitivesSize = p.render->primitivesSize;
                    p.updatePrimitivesSize(1);
                    Primitive& primitive = p.render->primitives[primitivesSize];
                    primitive.clipRect = p.currentClipRect;
                    primitive.colorMode = ColorMode::SolidColor;
                    primitive.color[0] = p.finalColor[0];
                    primitive.color[1] = p.finalColor[1];
                    primitive.color[2] = p.finalColor[2];
                    primitive.color[3] = p.finalColor[3];
                    primitive.vaoOffset = p.render->vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive.vaoSize = 6;

                    const size_t vboDataSize = p.render->vboDataSize;
                    p.updateVBODataSize(6);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.render->vboData[vboDataSize]);
                    pData->vx = value.min.x;
                    pData->vy = value.min.y;
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.min.y;
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.max.y;
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.max.y;
                    ++pData;
                    pData->vx = value.min.x;
                    pData->vy = value.max.y;
                    ++pData;
                    pData->vx = value.min.x;
                    pData->vy = value.min.y;
                }
            }

            void Render2D::drawPill(const Core::BBox2f& rect, size_t facets)
            {
                DJV_PRIVATE_PTR();
                if (rect.intersects(p.render->viewport))
                {
                    const size_t primitivesSize = p.render->primitivesSize;
                    p.updatePrimitivesSize(1);
                    Primitive& primitive = p.render->primitives[primitivesSize];
                    primitive.clipRect = p.currentClipRect;
                    primitive.colorMode = ColorMode::SolidColor;
                    primitive.color[0] = p.finalColor[0];
                    primitive.color[1] = p.finalColor[1];
                    primitive.color[2] = p.finalColor[2];
                    primitive.color[3] = p.finalColor[3];
                    primitive.vaoOffset = p.render->vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive.vaoSize = 3 * 2 + facets * 2 * 3;

                    const size_t vboDataSize = p.render->vboDataSize;
                    p.updateVBODataSize(primitive.vaoSize);
                    const float h = rect.h();
                    const float radius = h / 2.f;
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.render->vboData[vboDataSize]);
                    pData->vx = rect.min.x + radius;
                    pData->vy = rect.min.y;
                    ++pData;
                    pData->vx = rect.max.x - radius;
                    pData->vy = rect.min.y;
                    ++pData;
                    pData->vx = rect.max.x - radius;
                    pData->vy = rect.max.y;
                    ++pData;
                    pData->vx = rect.max.x - radius;
                    pData->vy = rect.max.y;
                    ++pData;
                    pData->vx = rect.min.x + radius;
                    pData->vy = rect.max.y;
                    ++pData;
                    pData->vx = rect.min.x + radius;
                    pData->vy = rect.min.y;
                    ++pData;

                    for (size_t i = 0; i < facets; ++i)
                    {
                        const float x = rect.min.x + radius;
                        const float y = rect.min.y + radius;
                        pData->vx = x;
                        pData->vy = y;
                        ++pData;
                        float degrees = i / static_cast<float>(facets) * 180.f + 90.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 180.f + 90.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        ++pData;
                    }

                    for (size_t i = 0; i < facets; ++i)
                    {
                        const float x = rect.max.x - radius;
                        const float y = rect.min.y + radius;
                        pData->vx = x;
                        pData->vy = y;
                        ++pData;
                        float degrees = i / static_cast<float>(facets) * 180.f + 270.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 180.f + 270.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        ++pData;
                    }
                }
            }

            void Render2D::drawCircle(const glm::vec2 & pos, float radius, size_t facets)
            {
                DJV_PRIVATE_PTR();
                const BBox2f rect(pos.x - radius, pos.y - radius, radius * 2.f, radius * 2.f);
                if (rect.intersects(p.render->viewport))
                {
                    const size_t primitivesSize = p.render->primitivesSize;
                    p.updatePrimitivesSize(1);
                    Primitive& primitive = p.render->primitives[primitivesSize];
                    primitive.clipRect = p.currentClipRect;
                    primitive.colorMode = ColorMode::SolidColor;
                    primitive.color[0] = p.finalColor[0];
                    primitive.color[1] = p.finalColor[1];
                    primitive.color[2] = p.finalColor[2];
                    primitive.color[3] = p.finalColor[3];
                    primitive.vaoOffset = p.render->vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);;
                    primitive.vaoSize = 3 * facets;

                    const size_t vboDataSize = p.render->vboDataSize;
                    p.updateVBODataSize(3 * facets);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.render->vboData[vboDataSize]);
                    for (size_t i = 0; i < facets * 3; i += 3)
                    {
                        pData->vx = pos.x;
                        pData->vy = pos.y;
                        ++pData;
                        float degrees = i / static_cast<float>(facets) * 360.f;
                        pData->vx = pos.x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = pos.y + sinf(Math::deg2rad(degrees)) * radius;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 360.f;
                        pData->vx = pos.x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = pos.y + sinf(Math::deg2rad(degrees)) * radius;
                        ++pData;
                    }
                }
            }

            void Render2D::drawImage(
                const std::shared_ptr<Image::Data> & imageData,
                const glm::vec2& pos,
                const ImageOptions& options)
            {
                DJV_PRIVATE_PTR();
                p.drawImage(imageData, pos, options, ColorMode::ColorAndTexture);
            }

            void Render2D::drawFilledImage(
                const std::shared_ptr<Image::Data> & imageData,
                const glm::vec2& pos,
                const ImageOptions& options)
            {
                DJV_PRIVATE_PTR();
                p.drawImage(imageData, pos, options, ColorMode::ColorWithTextureAlpha);
            }

            void Render2D::setCurrentFont(const Font::Info & value)
            {
                _p->currentFont = value;
            }

            void Render2D::drawText(const std::string & value, const glm::vec2 & pos, size_t maxLineWidth)
            {
                DJV_PRIVATE_PTR();
                if (auto fontSystem = p.fontSystem.lock())
                {
                    try
                    {
                        Primitive* primitive = nullptr;
                        float x = 0.f;
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

                            if (glyph->imageData && glyph->imageData->isValid())
                            {
                                const uint16_t width = glyph->imageData->getWidth();
                                const uint16_t height = glyph->imageData->getHeight();
                                const glm::vec2& offset = glyph->offset;
                                const BBox2f bbox(pos.x + x + offset.x, pos.y - offset.y, width, height);
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

                                    if (!primitive)
                                    {
                                        const size_t primitivesSize = p.render->primitivesSize;
                                        p.updatePrimitivesSize(1);
                                        primitive = &p.render->primitives[primitivesSize];
                                        primitive->clipRect = p.currentClipRect;
                                        primitive->colorMode = ColorMode::ColorWithTextureAlpha;
                                        primitive->color[0] = p.finalColor[0];
                                        primitive->color[1] = p.finalColor[1];
                                        primitive->color[2] = p.finalColor[2];
                                        primitive->color[3] = p.finalColor[3];
                                        primitive->imageCache = ImageCache::Atlas;
                                        primitive->atlasIndex = item.textureIndex;
                                        primitive->vaoOffset = p.render->vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);;
                                        primitive->vaoSize = 6;
                                    }
                                    else
                                    {
                                        primitive->vaoSize += 6;
                                    }

                                    const size_t vboDataSize = p.render->vboDataSize;
                                    p.updateVBODataSize(6);
                                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.render->vboData[vboDataSize]);
                                    pData->vx = bbox.min.x;
                                    pData->vy = bbox.min.y;
                                    pData->setTextureCoord(item.textureU.min, item.textureV.min);
                                    ++pData;
                                    pData->vx = bbox.max.x;
                                    pData->vy = bbox.min.y;
                                    pData->setTextureCoord(item.textureU.max, item.textureV.min);
                                    ++pData;
                                    pData->vx = bbox.max.x;
                                    pData->vy = bbox.max.y;
                                    pData->setTextureCoord(item.textureU.max, item.textureV.max);
                                    ++pData;
                                    pData->vx = bbox.max.x;
                                    pData->vy = bbox.max.y;
                                    pData->setTextureCoord(item.textureU.max, item.textureV.max);
                                    ++pData;
                                    pData->vx = bbox.min.x;
                                    pData->vy = bbox.max.y;
                                    pData->setTextureCoord(item.textureU.min, item.textureV.max);
                                    ++pData;
                                    pData->vx = bbox.min.x;
                                    pData->vy = bbox.min.y;
                                    pData->setTextureCoord(item.textureU.min, item.textureV.min);
                                }
                            }

                            x += glyph->advance;
                        }
                    }
                    catch (const std::exception&)
                    {
                    }
                }
            }

            void Render2D::drawShadow(const BBox2f& value, Side side)
            {
                DJV_PRIVATE_PTR();
                if (value.intersects(p.render->viewport))
                {
                    const size_t primitivesSize = p.render->primitivesSize;
                    p.updatePrimitivesSize(1);
                    Primitive& primitive = p.render->primitives[primitivesSize];
                    primitive.clipRect = p.currentClipRect;
                    primitive.colorMode = ColorMode::Shadow;
                    primitive.color[0] = p.finalColor[0];
                    primitive.color[1] = p.finalColor[1];
                    primitive.color[2] = p.finalColor[2];
                    primitive.color[3] = p.finalColor[3];
                    primitive.vaoOffset = p.render->vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive.vaoSize = 6;

                    static const uint16_t u[][6] =
                    {
                        { 0, 0, 0, 0, 0, 0 },
                        { 0, 65535, 65535, 65535, 0, 0 },
                        { 0, 0, 65535, 65535, 65535, 0 },
                        { 65535, 0, 0, 0, 65535, 65535 },
                        { 65535, 65535, 0, 0, 0, 65535 }
                    };

                    const size_t vboDataSize = p.render->vboDataSize;
                    p.updateVBODataSize(6);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.render->vboData[vboDataSize]);
                    pData->vx = value.min.x;
                    pData->vy = value.min.y;
                    pData->tx = u[static_cast<size_t>(side)][0];
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.min.y;
                    pData->tx = u[static_cast<size_t>(side)][1];
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.max.y;
                    pData->tx = u[static_cast<size_t>(side)][2];
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.max.y;
                    pData->tx = u[static_cast<size_t>(side)][3];
                    ++pData;
                    pData->vx = value.min.x;
                    pData->vy = value.max.y;
                    pData->tx = u[static_cast<size_t>(side)][4];
                    ++pData;
                    pData->vx = value.min.x;
                    pData->vy = value.min.y;
                    pData->tx = u[static_cast<size_t>(side)][5];
                }
            }

            void Render2D::drawShadow(const BBox2f& value, float radius, size_t facets)
            {
                DJV_PRIVATE_PTR();
                if (value.intersects(p.render->viewport))
                {
                    const size_t primitivesSize = p.render->primitivesSize;
                    p.updatePrimitivesSize(1);
                    Primitive& primitive = p.render->primitives[primitivesSize];
                    primitive.clipRect = p.currentClipRect;
                    primitive.colorMode = ColorMode::Shadow;
                    primitive.color[0] = p.finalColor[0];
                    primitive.color[1] = p.finalColor[1];
                    primitive.color[2] = p.finalColor[2];
                    primitive.color[3] = p.finalColor[3];
                    primitive.vaoOffset = p.render->vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive.vaoSize = 5 * 2 * 3 + 4 * facets * 3;

                    const size_t vboDataSize = p.render->vboDataSize;
                    p.updateVBODataSize(primitive.vaoSize);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.render->vboData[vboDataSize]);

                    // Center.
                    pData->vx = value.min.x + radius;
                    pData->vy = value.min.y + radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.max.x - radius;
                    pData->vy = value.min.y + radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.max.x - radius;
                    pData->vy = value.max.y - radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.max.x - radius;
                    pData->vy = value.max.y - radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.min.x + radius;
                    pData->vy = value.max.y - radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.min.x + radius;
                    pData->vy = value.min.y + radius;
                    pData->tx = 65535;
                    ++pData;

                    // Right.
                    pData->vx = value.max.x - radius;
                    pData->vy = value.min.y + radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.min.y + radius;
                    pData->tx = 0;
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.max.y - radius;
                    pData->tx = 0;
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.max.y - radius;
                    pData->tx = 0;
                    ++pData;
                    pData->vx = value.max.x - radius;
                    pData->vy = value.max.y - radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.max.x - radius;
                    pData->vy = value.min.y + radius;
                    pData->tx = 65535;
                    ++pData;

                    // Left.
                    pData->vx = value.min.x;
                    pData->vy = value.min.y + radius;
                    pData->tx = 0;
                    ++pData;
                    pData->vx = value.min.x + radius;
                    pData->vy = value.min.y + radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.min.x + radius;
                    pData->vy = value.max.y - radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.min.x + radius;
                    pData->vy = value.max.y - radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.min.x;
                    pData->vy = value.max.y - radius;
                    pData->tx = 0;
                    ++pData;
                    pData->vx = value.min.x;
                    pData->vy = value.min.y + radius;
                    pData->tx = 0;
                    ++pData;

                    // Top.
                    pData->vx = value.min.x + radius;
                    pData->vy = value.min.y;
                    pData->tx = 0;
                    ++pData;
                    pData->vx = value.max.x - radius;
                    pData->vy = value.min.y;
                    pData->tx = 0;
                    ++pData;
                    pData->vx = value.max.x - radius;
                    pData->vy = value.min.y + radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.max.x - radius;
                    pData->vy = value.min.y + radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.min.x + radius;
                    pData->vy = value.min.y + radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.min.x + radius;
                    pData->vy = value.min.y;
                    pData->tx = 0;
                    ++pData;

                    // Bottom.
                    pData->vx = value.min.x + radius;
                    pData->vy = value.max.y - radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.max.x - radius;
                    pData->vy = value.max.y - radius;
                    pData->tx = 65535;
                    ++pData;
                    pData->vx = value.max.x - radius;
                    pData->vy = value.max.y;
                    pData->tx = 0;
                    ++pData;
                    pData->vx = value.max.x - radius;
                    pData->vy = value.max.y;
                    pData->tx = 0;
                    ++pData;
                    pData->vx = value.min.x + radius;
                    pData->vy = value.max.y;
                    pData->tx = 0;
                    ++pData;
                    pData->vx = value.min.x + radius;
                    pData->vy = value.max.y - radius;
                    pData->tx = 65535;
                    ++pData;

                    // Upper left.
                    float x = value.min.x + radius;
                    float y = value.min.y + radius;
                    for (size_t i = 0; i < facets; ++i)
                    {
                        pData->vx = x;
                        pData->vy = y;
                        pData->tx = 65535;
                        ++pData;
                        float degrees = i / static_cast<float>(facets) * 90.f + 180.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 90.f + 180.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                    }

                    // Upper right.
                    x = value.max.x - radius;
                    y = value.min.y + radius;
                    for (size_t i = 0; i < facets; ++i)
                    {
                        pData->vx = x;
                        pData->vy = y;
                        pData->tx = 65535;
                        ++pData;
                        float degrees = i / static_cast<float>(facets) * 90.f + 270.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 90.f + 270.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                    }

                    // Lower right.
                    x = value.max.x - radius;
                    y = value.max.y - radius;
                    for (size_t i = 0; i < facets; ++i)
                    {
                        pData->vx = x;
                        pData->vy = y;
                        pData->tx = 65535;
                        ++pData;
                        float degrees = i / static_cast<float>(facets) * 90.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 90.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                    }

                    // Lower left.
                    x = value.min.x + radius;
                    y = value.max.y - radius;
                    for (size_t i = 0; i < facets; ++i)
                    {
                        pData->vx = x;
                        pData->vy = y;
                        pData->tx = 65535;
                        ++pData;
                        float degrees = i / static_cast<float>(facets) * 90.f + 90.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 90.f + 90.f;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                    }
                }
            }

            float Render2D::getTextureAtlasPercentage() const
            {
                return _p->render->textureAtlas->getPercentageUsed();
            }

            size_t Render2D::getDynamicTextureCount() const
            {
                return _p->render->dynamicTextureCache.size();
            }

            size_t Render2D::getVBOSize() const
            {
                return _p->render->vbo->getSize();
            }

            void Render2D::Private::updateCurrentTransform()
            {
                currentTransform = glm::mat3x3(1.f);
                for (const auto& i : transforms)
                {
                    currentTransform *= i;
                }
            }

            void Render2D::Private::updateCurrentClipRect()
            {
                currentClipRect.min.x = 0.f;
                currentClipRect.min.y = 0.f;
                currentClipRect.max.x = static_cast<float>(size.w);
                currentClipRect.max.y = static_cast<float>(size.h);
                for (const auto & i : clipRects)
                {
                    currentClipRect = currentClipRect.intersect(i);
                }
            }

            void Render2D::Private::drawRoundedRect(const BBox2f& rect, float radius, size_t facets)
            {
                //! \todo
            }

            void Render2D::Private::drawImage(
                const std::shared_ptr<Image::Data>& imageData,
                const glm::vec2& pos,
                const ImageOptions& options,
                ColorMode colorMode)
            {
                const auto& info = imageData->getInfo();

                static glm::vec3 pts[4];
                pts[0].x = pos.x;
                pts[0].y = pos.y;
                pts[0].z = 1.f;
                pts[1].x = pos.x + info.size.w;
                pts[1].y = pos.y;
                pts[1].z = 1.f;
                pts[2].x = pos.x + info.size.w;
                pts[2].y = pos.y + info.size.h;
                pts[2].z = 1.f;
                pts[3].x = pos.x;
                pts[3].y = pos.y + info.size.h;
                pts[3].z = 1.f;
                for (auto& i : pts)
                {
                    i = currentTransform * i;
                }

                static BBox2f bbox;
                bbox.min = pts[0];
                bbox.max = pts[0];
                for (size_t i = 1; i < 4; ++i)
                {
                    bbox.min.x = std::min(bbox.min.x, pts[i].x);
                    bbox.max.x = std::max(bbox.max.x, pts[i].x);
                    bbox.min.y = std::min(bbox.min.y, pts[i].y);
                    bbox.max.y = std::max(bbox.max.y, pts[i].y);
                }

                if (bbox.intersects(render->viewport))
                {
                    const size_t primitivesSize = render->primitivesSize;
                    updatePrimitivesSize(1);
                    Primitive& primitive = render->primitives[primitivesSize];
                    primitive.clipRect = currentClipRect;
                    switch (Image::getChannelType(info.type))
                    {
                    case Image::ChannelType::L:    primitive.imageFormat = ImageFormat::L;    break;
                    case Image::ChannelType::LA:   primitive.imageFormat = ImageFormat::LA;   break;
                    case Image::ChannelType::RGB:  primitive.imageFormat = ImageFormat::RGB;  break;
                    case Image::ChannelType::RGBA: primitive.imageFormat = ImageFormat::RGBA; break;
                    default: break;
                    }
                    primitive.colorMode = colorMode;
                    primitive.color[0] = finalColor[0];
                    primitive.color[1] = finalColor[1];
                    primitive.color[2] = finalColor[2];
                    primitive.color[3] = finalColor[3];
                    primitive.imageChannel = options.channel;
                    primitive.imageCache = options.cache;
                    FloatRange textureU;
                    FloatRange textureV;
                    const UID uid = imageData->getUID();
                    switch (options.cache)
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
                            auto texture = OpenGL::Texture::create(imageData->getInfo(), GL_LINEAR, GL_NEAREST);
                            texture->copy(*imageData);
                            render->dynamicTextureCache[uid] = texture;
                            primitive.textureID = texture->getID();
                        }
                        if (info.layout.mirror.x)
                        {
                            textureU.min = 1.f;
                            textureU.max = 0.f;
                        }
                        else
                        {
                            textureU.min = 0.f;
                            textureU.max = 1.f;
                        }
                        if (info.layout.mirror.y)
                        {
                            textureV.min = 1.f;
                            textureV.max = 0.f;
                        }
                        else
                        {
                            textureV.min = 0.f;
                            textureV.max = 1.f;
                        }
                        break;
                    }
                    default: break;
                    }
                    if (options.mirror.x)
                    {
                        textureU.min = 1.f - textureU.min;
                        textureU.max = 1.f - textureU.max;
                    }
                    if (options.mirror.y)
                    {
                        textureV.min = 1.f - textureV.min;
                        textureV.max = 1.f - textureV.max;
                    }
                    primitive.vaoOffset = render->vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);;
                    primitive.vaoSize = 6;

                    const size_t vboDataSize = render->vboDataSize;
                    updateVBODataSize(6);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&render->vboData[vboDataSize]);
                    pData->vx = pts[0].x;
                    pData->vy = pts[0].y;
                    pData->setTextureCoord(textureU.min, textureV.min);
                    ++pData;
                    pData->vx = pts[1].x;
                    pData->vy = pts[1].y;
                    pData->setTextureCoord(textureU.max, textureV.min);
                    ++pData;
                    pData->vx = pts[2].x;
                    pData->vy = pts[2].y;
                    pData->setTextureCoord(textureU.max, textureV.max);
                    ++pData;
                    pData->vx = pts[2].x;
                    pData->vy = pts[2].y;
                    pData->setTextureCoord(textureU.max, textureV.max);
                    ++pData;
                    pData->vx = pts[3].x;
                    pData->vy = pts[3].y;
                    pData->setTextureCoord(textureU.min, textureV.max);
                    ++pData;
                    pData->vx = pts[0].x;
                    pData->vy = pts[0].y;
                    pData->setTextureCoord(textureU.min, textureV.min);
                }
            }

            void Render2D::Private::updatePrimitivesSize(size_t value)
            {
                render->primitivesSize += value;
                if (render->primitivesSize >= render->primitives.size())
                {
                    render->primitives.resize(render->primitivesSize);
                }
            }

            void Render2D::Private::updateVBODataSize(size_t value)
            {
                const size_t vertexByteCount = AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                render->vboDataSize += value * vertexByteCount;
                if (render->vboDataSize > render->vboData.size())
                {
                    render->vboData.resize(render->vboDataSize);
                }
            }

        } // namespace Render
    } // namespace AV
} // namespace djv
