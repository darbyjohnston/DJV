// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Render2D.h>

#include <djvAV/Color.h>
#include <djvAV/GLFWSystem.h>
#include <djvAV/OpenGLMesh.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/OpenGLTexture.h>
#include <djvAV/OpenGLTextureAtlas.h>
#include <djvAV/Render2DPrivate.h>
#include <djvAV/Shader.h>
#include <djvAV/Shape.h>
#include <djvAV/TriangleMesh.h>

#include <djvCore/Cache.h>
#include <djvCore/Context.h>
#include <djvCore/FileIO.h>
#include <djvCore/LogSystem.h>
#include <djvCore/Range.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/Timer.h>

#include <OpenColorIO/OpenColorIO.h>

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/perpendicular.hpp>

using namespace djv::Core;
namespace _OCIO = OCIO_NAMESPACE;

namespace djv
{
    namespace AV
    {
        namespace Render2D
        {
            struct Render::Private
            {
                Render* system = nullptr;

                ImageFilterOptions                                  imageFilterOptions  = ImageFilterOptions(ImageFilter::Linear, ImageFilter::Nearest);
                bool                                                textLCDRendering    = true;

                BBox2f                                              viewport;
                std::vector<std::shared_ptr<Primitive> >            primitives;
                PrimitiveData                                       primitiveData;
                std::shared_ptr<OpenGL::TextureAtlas>               textureAtlas;
                std::map<UID, uint64_t>                             textureIDs;
                std::map<UID, uint64_t>                             glyphTextureIDs;
                std::vector<std::shared_ptr<OpenGL::Texture> >      dynamicTextures;
                std::map<UID, std::shared_ptr<OpenGL::Texture> >    dynamicTextureCache;
#if !defined(DJV_OPENGL_ES2)
                std::map<OCIO::Convert, ColorSpaceData>             colorSpaceCache;
                size_t                                              colorSpaceID        = 1;
#endif // DJV_OPENGL_ES2
                std::vector<uint8_t>                                vboData;
                size_t                                              vboDataSize         = 0;
                std::shared_ptr<OpenGL::VBO>                        vbo;
                std::shared_ptr<OpenGL::VAO>                        vao;
                std::string                                         vertexFileName;
                std::string                                         vertexSource;
                std::string                                         fragmentFileName;
                std::string                                         fragmentSource;
                std::shared_ptr<OpenGL::Shader>                     shader;
                GLint                                               mvpLoc              = 0;

                std::shared_ptr<Time::Timer>                        statsTimer;

                void updateVBODataSize(size_t);

                void drawImage(
                    const std::shared_ptr<Image::Image>&,
                    const glm::vec2& pos,
                    const ImageOptions&,
                    ColorMode,
                    const glm::mat3x3& currentTransform,
                    const BBox2f& currentClipRect,
                    const float finalColor[4]);

                std::string getFragmentSource() const;
            };

            void Render::_init(const std::shared_ptr<Core::Context>& context)
            {
                ISystem::_init("djv::AV::Render2D::Render", context);
                DJV_PRIVATE_PTR();
                p.system = this;

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
                    logSystem->log("djv::AV::Render2D::Render", ss.str());
                }
                const uint8_t _textureAtlasCount = std::min(maxTextureUnits, static_cast<GLint>(textureAtlasCount));
                const uint16_t _textureAtlasSize = std::min(maxTextureSize, static_cast<GLint>(textureAtlasSize));
                {
                    auto logSystem = context->getSystemT<LogSystem>();
                    std::stringstream ss;
                    ss << "Texture atlas count: " << static_cast<size_t>(_textureAtlasCount) << "\n";
                    ss << "Texture atlas size: " << static_cast<size_t>(_textureAtlasSize);
                    logSystem->log("djv::AV::Render::Render2D", ss.str());
                }
                p.textureAtlas.reset(new OpenGL::TextureAtlas(
                    _textureAtlasCount,
                    _textureAtlasSize,
                    Image::Type::RGBA_U8,
                    GL_NEAREST,
                    0));
                p.primitiveData.textureAtlasCount = _textureAtlasCount;

                _updateImageFilter();

                auto resourceSystem = context->getSystemT<ResourceSystem>();
                const FileSystem::Path shaderPath = resourceSystem->getPath(FileSystem::ResourcePath::Shaders);
                try
                {
                    p.vertexFileName = std::string(FileSystem::Path(shaderPath, "djvAVRender2DVertex.glsl"));
                    auto io = FileSystem::FileIO::create();
                    io->open(p.vertexFileName, FileSystem::FileIO::Mode::Read);
                    p.vertexSource = FileSystem::FileIO::readContents(io);
                    p.fragmentFileName = std::string(FileSystem::Path(shaderPath, "djvAVRender2DFragment.glsl"));
                    io->open(p.fragmentFileName, FileSystem::FileIO::Mode::Read);
                    p.fragmentSource = FileSystem::FileIO::readContents(io);
                }
                catch (const std::exception& e)
                {
                    auto logSystem = context->getSystemT<LogSystem>();
                    logSystem->log("djv::AV::Render::Render2D", e.what(), LogLevel::Error);
                }

                p.statsTimer = Time::Timer::create(context);
                p.statsTimer->setRepeating(true);
                p.statsTimer->start(
                    Time::getTime(Time::TimerValue::VerySlow),
                    [this](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                    {
                        DJV_PRIVATE_PTR();
                        std::stringstream ss;
                        ss << "Texture atlas: " << p.textureAtlas->getPercentageUsed() << "%\n";
                        ss << "Texture IDs: " << p.textureIDs.size() << "%\n";
                        ss << "Glyph texture IDs: " << p.glyphTextureIDs.size() << "\n";
                        ss << "Dynamic textures: " << p.dynamicTextures.size() << "\n";
                        ss << "Dynamic texture cache: " << p.dynamicTextureCache.size() << "\n";
#if !defined(DJV_OPENGL_ES2)
                        ss << "Color space cache: " << p.colorSpaceCache.size() << "\n";
#endif // DJV_OPENGL_ES2
                        ss << "VBO size: " << (p.vbo ? p.vbo->getSize() : 0);
                        _log(ss.str());
                    });
            }

            Render::Render() :
                _p(new Private)
            {}

            Render::~Render()
            {}

            std::shared_ptr<Render> Render::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<Render>(new Render);
                out->_init(context);
                return out;
            }

            void Render::beginFrame(const Image::Size& size)
            {
                DJV_PRIVATE_PTR();
                _size = size;
                _currentClipRect = BBox2f(0.F, 0.F, static_cast<float>(size.w), static_cast<float>(size.h));
                p.viewport = BBox2f(0.F, 0.F, static_cast<float>(size.w), static_cast<float>(size.h));
            }

            void Render::endFrame()
            {
                DJV_PRIVATE_PTR();
                if (!p.shader)
                {
                    auto shader = AV::Render::Shader::create(p.vertexSource, p.getFragmentSource());
                    shader->setVertexName(p.vertexFileName);
                    shader->setFragmentName(p.fragmentFileName);
                    p.shader = OpenGL::Shader::create(shader);
                    const auto program = p.shader->getProgram();
                    p.mvpLoc = glGetUniformLocation(program, "transform.mvp");
                    p.primitiveData.imageChannelsLoc = glGetUniformLocation(program, "imageChannels");
                    p.primitiveData.colorModeLoc = glGetUniformLocation(program, "colorMode");
                    p.primitiveData.colorLoc = glGetUniformLocation(program, "color");
#if !defined(DJV_OPENGL_ES2)
                    p.primitiveData.colorSpaceLoc = glGetUniformLocation(program, "colorSpace");
                    p.primitiveData.colorSpaceSamplerLoc = glGetUniformLocation(program, "colorSpaceSampler");
#endif // DJV_OPENGL_ES2
                    p.primitiveData.imageChannelDisplayLoc = glGetUniformLocation(program, "imageChannelDisplay");
                    p.primitiveData.colorMatrixLoc = glGetUniformLocation(program, "colorMatrix");
                    p.primitiveData.colorMatrixEnabledLoc = glGetUniformLocation(program, "colorMatrixEnabled");
                    p.primitiveData.colorInvertLoc = glGetUniformLocation(program, "colorInvert");
                    p.primitiveData.levelsInLowLoc = glGetUniformLocation(program, "levels.inLow");
                    p.primitiveData.levelsInHighLoc = glGetUniformLocation(program, "levels.inHigh");
                    p.primitiveData.levelsGammaLoc = glGetUniformLocation(program, "levels.gamma");
                    p.primitiveData.levelsOutLowLoc = glGetUniformLocation(program, "levels.outLow");
                    p.primitiveData.levelsOutHighLoc = glGetUniformLocation(program, "levels.outHigh");
                    p.primitiveData.levelsEnabledLoc = glGetUniformLocation(program, "levelsEnabled");
                    p.primitiveData.exposureVLoc = glGetUniformLocation(program, "exposure.v");
                    p.primitiveData.exposureDLoc = glGetUniformLocation(program, "exposure.d");
                    p.primitiveData.exposureKLoc = glGetUniformLocation(program, "exposure.k");
                    p.primitiveData.exposureFLoc = glGetUniformLocation(program, "exposure.f");
                    p.primitiveData.exposureEnabledLoc = glGetUniformLocation(program, "exposureEnabled");
                    p.primitiveData.softClipLoc = glGetUniformLocation(program, "softClip");
                    p.primitiveData.textureSamplerLoc = glGetUniformLocation(program, "textureSampler");
                }
                p.shader->bind();

#if !defined(DJV_OPENGL_ES2)
                glEnable(GL_MULTISAMPLE);
#endif // DJV_OPENGL_ES2
                glEnable(GL_SCISSOR_TEST);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);

                glViewport(
                    static_cast<GLint>(p.viewport.min.x),
                    static_cast<GLint>(p.viewport.min.y),
                    static_cast<GLsizei>(p.viewport.w()),
                    static_cast<GLsizei>(p.viewport.h()));
                glScissor(
                    static_cast<GLint>(p.viewport.min.x),
                    static_cast<GLint>(p.viewport.min.y),
                    static_cast<GLsizei>(p.viewport.w()),
                    static_cast<GLsizei>(p.viewport.h()));
                glClearColor(0.F, 0.F, 0.F, 0.F);
                glClear(GL_COLOR_BUFFER_BIT);

                const auto viewMatrix = glm::ortho(
                    p.viewport.min.x,
                    p.viewport.max.x,
                    p.viewport.max.y,
                    p.viewport.min.y,
                    -1.F, 1.F);
                p.shader->setUniform(p.mvpLoc, viewMatrix);

                const auto& atlasTextures = p.textureAtlas->getTextures();
                for (GLuint i = 0; i < static_cast<GLuint>(atlasTextures.size()); ++i)
                {
                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
                    glBindTexture(GL_TEXTURE_2D, atlasTextures[i]);
                }

                const size_t vertexByteCount = AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                if (!p.vbo || p.vboDataSize / vertexByteCount > p.vbo->getSize())
                {
                    p.vbo = OpenGL::VBO::create(p.vboDataSize / vertexByteCount, OpenGL::VBOType::Pos2_F32_UV_U16);
                    p.vao = OpenGL::VAO::create(p.vbo->getType(), p.vbo->getID());
                }
                p.vbo->copy(p.vboData, 0, p.vboDataSize);
                p.vao->bind();

                BBox2f currentClipRect(0.F, 0.F, 0.F, 0.F);
                AlphaBlend currentAlphaBlend = AlphaBlend::Straight;
                bool currentTextLCDRendering = false;
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                for (const auto& primitive : p.primitives)
                {
                    const BBox2f clipRect = flip(primitive->clipRect, _size);
                    if (clipRect != currentClipRect)
                    {
                        currentClipRect = clipRect;
                        glScissor(
                            static_cast<GLint>(currentClipRect.min.x),
                            static_cast<GLint>(currentClipRect.min.y),
                            static_cast<GLsizei>(currentClipRect.w()),
                            static_cast<GLsizei>(currentClipRect.h()));
                    }
                    if (primitive->alphaBlend != currentAlphaBlend)
                    {
                        currentAlphaBlend = primitive->alphaBlend;
                        switch (currentAlphaBlend)
                        {
                        case AlphaBlend::None:
                            glBlendFunc(GL_ONE, GL_ZERO);
                            break;
                        case AlphaBlend::Straight:
                            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                            break;
                        case AlphaBlend::Premultiplied:
                            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                            break;
                        default: break;
                        }
                    }
                    if (primitive->textLCDRendering != currentTextLCDRendering)
                    {
                        currentTextLCDRendering = primitive->textLCDRendering;
                        if (!currentTextLCDRendering)
                        {
                            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                        }
                    }
                    primitive->bind(p.primitiveData, p.shader);
                    if (currentTextLCDRendering)
                    {
                        p.shader->setUniform(p.primitiveData.colorModeLoc, static_cast<int>(ColorMode::ColorWithTextureAlphaR));
                        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
                        p.vao->draw(primitive->type, primitive->vaoOffset, primitive->vaoSize);
                        p.shader->setUniform(p.primitiveData.colorModeLoc, static_cast<int>(ColorMode::ColorWithTextureAlphaG));
                        glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE);
                        p.vao->draw(primitive->type, primitive->vaoOffset, primitive->vaoSize);
                        p.shader->setUniform(p.primitiveData.colorModeLoc, static_cast<int>(ColorMode::ColorWithTextureAlphaB));
                        glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
                        p.vao->draw(primitive->type, primitive->vaoOffset, primitive->vaoSize);
                    }
                    else
                    {
                        p.vao->draw(primitive->type, primitive->vaoOffset, primitive->vaoSize);
                    }
                }
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

                _clipRects.clear();
                p.primitives.clear();
                p.vboDataSize = 0;
                while (p.dynamicTextureCache.size() > dynamicTextureCacheMax)
                {
                    auto texture = p.dynamicTextureCache.begin();
                    p.dynamicTextures.emplace_back(texture->second);
                    p.dynamicTextureCache.erase(texture);
                }
                while (p.dynamicTextures.size() > dynamicTextureCount)
                {
                    p.dynamicTextures.pop_back();
                }
#if !defined(DJV_OPENGL_ES2)
                while (p.colorSpaceCache.size() > colorSpaceCacheMax)
                {
                    p.colorSpaceCache.erase(p.colorSpaceCache.begin());
                    p.shader.reset();
                }
#endif // DJV_OPENGL_ES2
            }
            
            void Render::drawPolyline(const std::vector<glm::vec2>& value)
            {
                DJV_PRIVATE_PTR();
                const size_t size = value.size();
                if (size > 1)
                {
                    std::vector<glm::vec2> pts;
                    for (size_t i = 0; i < size; ++i)
                    {
                        const glm::vec2 v = 0 == i ? (value[i + 1] - value[i]) : (value[i] - value[i - 1]);
                        const glm::vec2 perp = glm::normalize(glm::vec2(-v.y, v.x)) * _lineWidth;
                        const glm::vec2 pt[2] =
                        {
                            value[i] - perp,
                            value[i] + perp
                        };
                        for (size_t j = 0; j < 2; ++j)
                        {
                            const glm::vec3 tmp = _getCurrentTransform() * glm::vec3(pt[j].x, pt[j].y, 1.F);
                            pts.push_back(glm::vec2(tmp.x, tmp.y));
                        }
                    }
                    
                    BBox2f bbox;
                    bbox.min = bbox.max = glm::vec2(pts[0].x, pts[0].y);
                    const size_t ptsSize = pts.size();
                    for (size_t i = 1; i < ptsSize; ++i)
                    {
                        bbox.expand(pts[i]);
                    }
                    if (bbox.intersects(_currentClipRect))
                    {
                        auto primitive = std::make_shared<Primitive>();
                        primitive->clipRect = _currentClipRect;
                        primitive->color[0] = _finalColor[0];
                        primitive->color[1] = _finalColor[1];
                        primitive->color[2] = _finalColor[2];
                        primitive->color[3] = _finalColor[3];
                        primitive->type = GL_TRIANGLE_STRIP;
                        primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                        primitive->vaoSize = ptsSize;

                        const size_t vboDataOffset = p.vboDataSize;
                        p.updateVBODataSize(ptsSize);
                        const glm::vec2* pPts = pts.data();
                        VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataOffset]);
                        for (size_t i = 0; i < size; ++i, pPts += 2)
                        {
                            pData->vx = pPts[0].x;
                            pData->vy = pPts[0].y;
                            ++pData;
                            pData->vx = pPts[1].x;
                            pData->vy = pPts[1].y;
                            ++pData;
                        }

                        p.primitives.emplace_back(primitive);
                    }
                }
            }
            
            void Render::drawRect(const BBox2f& value)
            {
                drawRects({ value });
            }

            void Render::drawRects(const std::vector<BBox2f>& value)
            {
                DJV_PRIVATE_PTR();
                auto primitive = std::make_shared<Primitive>();
                primitive->clipRect = _currentClipRect;
                primitive->color[0] = _finalColor[0];
                primitive->color[1] = _finalColor[1];
                primitive->color[2] = _finalColor[2];
                primitive->color[3] = _finalColor[3];
                primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                primitive->vaoSize = 0;

                for (const auto& i : value)
                {
                    if (i.intersects(_currentClipRect))
                    {
                        primitive->vaoSize += 6;

                        const size_t vboDataOffset = p.vboDataSize;
                        p.updateVBODataSize(6);
                        VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataOffset]);
                        pData->vx = i.min.x;
                        pData->vy = i.min.y;
                        ++pData;
                        pData->vx = i.max.x;
                        pData->vy = i.min.y;
                        ++pData;
                        pData->vx = i.max.x;
                        pData->vy = i.max.y;
                        ++pData;
                        pData->vx = i.max.x;
                        pData->vy = i.max.y;
                        ++pData;
                        pData->vx = i.min.x;
                        pData->vy = i.max.y;
                        ++pData;
                        pData->vx = i.min.x;
                        pData->vy = i.min.y;
                        ++pData;
                    }
                }

                p.primitives.emplace_back(primitive);
            }

            void Render::drawPill(const Core::BBox2f& rect, size_t facets)
            {
                DJV_PRIVATE_PTR();
                if (rect.intersects(_currentClipRect))
                {
                    auto primitive = std::make_shared<Primitive>();
                    primitive->clipRect = _currentClipRect;
                    primitive->color[0] = _finalColor[0];
                    primitive->color[1] = _finalColor[1];
                    primitive->color[2] = _finalColor[2];
                    primitive->color[3] = _finalColor[3];
                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive->vaoSize = 3 * 2 + facets * 2 * 3;

                    const size_t vboDataOffset = p.vboDataSize;
                    p.updateVBODataSize(primitive->vaoSize);
                    const float h = rect.h();
                    const float radius = h / 2.F;
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataOffset]);
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
                        float degrees = i / static_cast<float>(facets) * 180.F + 90.F;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 180.F + 90.F;
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
                        float degrees = i / static_cast<float>(facets) * 180.F + 270.F;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 180.F + 270.F;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        ++pData;
                    }

                    p.primitives.emplace_back(primitive);
                }
            }

            void Render::drawCircle(const glm::vec2& pos, float radius, size_t facets)
            {
                DJV_PRIVATE_PTR();
                const BBox2f rect(pos.x - radius, pos.y - radius, radius * 2.F, radius * 2.F);
                if (rect.intersects(_currentClipRect))
                {
                    auto primitive = std::make_shared<Primitive>();
                    primitive->clipRect = _currentClipRect;
                    primitive->color[0] = _finalColor[0];
                    primitive->color[1] = _finalColor[1];
                    primitive->color[2] = _finalColor[2];
                    primitive->color[3] = _finalColor[3];
                    //! \todo Implement me!
                    //primitive->type = GL_TRIANGLE_FAN;
                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive->vaoSize = 3 * facets;

                    const size_t vboDataOffset = p.vboDataSize;
                    p.updateVBODataSize(3 * facets);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataOffset]);
                    for (size_t i = 0; i < facets * 3; i += 3)
                    {
                        pData->vx = pos.x;
                        pData->vy = pos.y;
                        ++pData;
                        float degrees = i / static_cast<float>(facets) * 360.F;
                        pData->vx = pos.x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = pos.y + sinf(Math::deg2rad(degrees)) * radius;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 360.F;
                        pData->vx = pos.x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = pos.y + sinf(Math::deg2rad(degrees)) * radius;
                        ++pData;
                    }

                    p.primitives.emplace_back(primitive);
                }
            }

            const ImageFilterOptions& Render::getImageFilterOptions() const
            {
                return _p->imageFilterOptions;
            }
            
            void Render::setImageFilterOptions(const ImageFilterOptions& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.imageFilterOptions)
                    return;
                p.imageFilterOptions = value;
                _updateImageFilter();
            }

            void Render::drawImage(
                const std::shared_ptr<Image::Image>& image,
                const glm::vec2& pos,
                const ImageOptions& options)
            {
                DJV_PRIVATE_PTR();
                p.drawImage(image, pos, options, ColorMode::ColorAndTexture, _getCurrentTransform(), _currentClipRect, _finalColor);
            }

            void Render::drawFilledImage(
                const std::shared_ptr<Image::Image>& image,
                const glm::vec2& pos,
                const ImageOptions& options)
            {
                DJV_PRIVATE_PTR();
                p.drawImage(image, pos, options, ColorMode::ColorWithTextureAlpha, _getCurrentTransform(), _currentClipRect, _finalColor);
            }

            void Render::setTextLCDRendering(bool value)
            {
                _p->textLCDRendering = value;
            }

            void Render::drawText(const std::vector<std::shared_ptr<Font::Glyph> >& glyphs, const glm::vec2& pos)
            {
                DJV_PRIVATE_PTR();

                std::shared_ptr<TextPrimitive> primitive;
                float x = 0.F;
                int32_t rsbDeltaPrev = 0;
                uint8_t textureIndex = 0;
                for (const auto& glyph : glyphs)
                {
                    if (glyph)
                    {
                        if (rsbDeltaPrev - glyph->lsbDelta > 32)
                        {
                            x -= 1.F;
                        }
                        else if (rsbDeltaPrev - glyph->lsbDelta < -31)
                        {
                            x += 1.F;
                        }
                        rsbDeltaPrev = glyph->rsbDelta;

                        if (glyph->imageData && glyph->imageData->isValid())
                        {
                            const uint16_t width = glyph->imageData->getWidth();
                            const uint16_t height = glyph->imageData->getHeight();
                            const glm::vec2& offset = glyph->offset;
                            const BBox2f bbox(pos.x + x + offset.x, pos.y - offset.y, width, height);
                            if (bbox.intersects(_currentClipRect))
                            {
                                const auto uid = glyph->imageData->getUID();
                                uint64_t id = 0;
                                const auto i = p.glyphTextureIDs.find(uid);
                                if (i != p.glyphTextureIDs.end())
                                {
                                    id = i->second;
                                }
                                OpenGL::TextureAtlasItem item;
                                if (!p.textureAtlas->getItem(id, item))
                                {
                                    id = p.textureAtlas->addItem(glyph->imageData, item);
                                    p.glyphTextureIDs[uid] = id;
                                }

                                if (!primitive || item.textureIndex != textureIndex)
                                {
                                    primitive = std::make_shared<TextPrimitive>();
                                    primitive->clipRect = _currentClipRect;
                                    primitive->color[0] = _finalColor[0];
                                    primitive->color[1] = _finalColor[1];
                                    primitive->color[2] = _finalColor[2];
                                    primitive->color[3] = _finalColor[3];
                                    primitive->atlasIndex = item.textureIndex;
                                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                                    primitive->vaoSize = 0;
                                    primitive->textLCDRendering = p.textLCDRendering;
                                    p.primitives.push_back(primitive);
                                    textureIndex = item.textureIndex;
                                }

                                primitive->vaoSize += 6;
                                const size_t vboDataOffset = p.vboDataSize;
                                p.updateVBODataSize(6);
                                VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataOffset]);
                                pData->vx = bbox.min.x;
                                pData->vy = bbox.min.y;
                                pData->tx = static_cast<uint16_t>(item.textureU.getMin() * 65535.F);
                                pData->ty = static_cast<uint16_t>(item.textureV.getMin() * 65535.F);
                                ++pData;
                                pData->vx = bbox.max.x;
                                pData->vy = bbox.min.y;
                                pData->tx = static_cast<uint16_t>(item.textureU.getMax() * 65535.F);
                                pData->ty = static_cast<uint16_t>(item.textureV.getMin() * 65535.F);
                                ++pData;
                                pData->vx = bbox.max.x;
                                pData->vy = bbox.max.y;
                                pData->tx = static_cast<uint16_t>(item.textureU.getMax() * 65535.F);
                                pData->ty = static_cast<uint16_t>(item.textureV.getMax() * 65535.F);
                                ++pData;
                                pData->vx = bbox.max.x;
                                pData->vy = bbox.max.y;
                                pData->tx = static_cast<uint16_t>(item.textureU.getMax() * 65535.F);
                                pData->ty = static_cast<uint16_t>(item.textureV.getMax() * 65535.F);
                                ++pData;
                                pData->vx = bbox.min.x;
                                pData->vy = bbox.max.y;
                                pData->tx = static_cast<uint16_t>(item.textureU.getMin() * 65535.F);
                                pData->ty = static_cast<uint16_t>(item.textureV.getMax() * 65535.F);
                                ++pData;
                                pData->vx = bbox.min.x;
                                pData->vy = bbox.min.y;
                                pData->tx = static_cast<uint16_t>(item.textureU.getMin() * 65535.F);
                                pData->ty = static_cast<uint16_t>(item.textureV.getMin() * 65535.F);
                            }
                        }

                        x += glyph->advance;
                    }
                }
            }

            void Render::drawShadow(const BBox2f& value, Side side)
            {
                DJV_PRIVATE_PTR();
                if (value.intersects(_currentClipRect))
                {
                    auto primitive = std::make_shared<ShadowPrimitive>();
                    primitive->clipRect = _currentClipRect;
                    primitive->color[0] = _finalColor[0];
                    primitive->color[1] = _finalColor[1];
                    primitive->color[2] = _finalColor[2];
                    primitive->color[3] = _finalColor[3];
                    primitive->type = GL_TRIANGLE_STRIP;
                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive->vaoSize = 4;

                    static const uint16_t u[][4] =
                    {
                        {     0,     0,     0,     0 },
                        {     0, 65535,     0, 65535 },
                        {     0,     0, 65535, 65535 },
                        { 65535,     0, 65535,     0 },
                        { 65535, 65535,     0,     0 }
                    };

                    const size_t vboDataOffset = p.vboDataSize;
                    p.updateVBODataSize(4);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataOffset]);
                    pData->vx = value.min.x;
                    pData->vy = value.min.y;
                    pData->tx = u[static_cast<size_t>(side)][0];
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.min.y;
                    pData->tx = u[static_cast<size_t>(side)][1];
                    ++pData;
                    pData->vx = value.min.x;
                    pData->vy = value.max.y;
                    pData->tx = u[static_cast<size_t>(side)][2];
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.max.y;
                    pData->tx = u[static_cast<size_t>(side)][3];

                    p.primitives.emplace_back(primitive);
                }
            }

            void Render::drawShadow(const BBox2f& value, float radius, size_t facets)
            {
                DJV_PRIVATE_PTR();
                if (value.intersects(_currentClipRect))
                {
                    auto primitive = std::make_shared<ShadowPrimitive>();
                    primitive->clipRect = _currentClipRect;
                    primitive->color[0] = _finalColor[0];
                    primitive->color[1] = _finalColor[1];
                    primitive->color[2] = _finalColor[2];
                    primitive->color[3] = _finalColor[3];
                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive->vaoSize = 5 * 2 * 3 + 4 * facets * 3;

                    const size_t vboDataOffset = p.vboDataSize;
                    p.updateVBODataSize(primitive->vaoSize);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataOffset]);

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
                        float degrees = i / static_cast<float>(facets) * 90.F + 180.F;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 90.F + 180.F;
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
                        float degrees = i / static_cast<float>(facets) * 90.F + 270.F;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 90.F + 270.F;
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
                        float degrees = i / static_cast<float>(facets) * 90.F;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 90.F;
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
                        float degrees = i / static_cast<float>(facets) * 90.F + 90.F;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                        degrees = (i + 1) / static_cast<float>(facets) * 90.F + 90.F;
                        pData->vx = x + cosf(Math::deg2rad(degrees)) * radius;
                        pData->vy = y + sinf(Math::deg2rad(degrees)) * radius;
                        pData->tx = 0;
                        ++pData;
                    }

                    p.primitives.emplace_back(primitive);
                }
            }

            void Render::drawTexture(const BBox2f& value, GLuint textureID, GLenum target)
            {
                DJV_PRIVATE_PTR();
                if (value.intersects(_currentClipRect))
                {
                    auto primitive = std::make_shared<TexturePrimitive>();
                    primitive->clipRect = _currentClipRect;
                    primitive->color[0] = _finalColor[0];
                    primitive->color[1] = _finalColor[1];
                    primitive->color[2] = _finalColor[2];
                    primitive->color[3] = _finalColor[3];
                    primitive->type = GL_TRIANGLE_STRIP;
                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive->vaoSize = 4;
                    primitive->textureID = textureID;
                    primitive->target = target;

                    const size_t vboDataOffset = p.vboDataSize;
                    p.updateVBODataSize(4);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataOffset]);
                    pData->vx = value.min.x;
                    pData->vy = value.min.y;
                    pData->tx = 0;
                    pData->ty = 65535;
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.min.y;
                    pData->tx = 65535;
                    pData->ty = 65535;
                    ++pData;
                    pData->vx = value.min.x;
                    pData->vy = value.max.y;
                    pData->tx = 0;
                    pData->ty = 0;
                    ++pData;
                    pData->vx = value.max.x;
                    pData->vy = value.max.y;
                    pData->tx = 65535;
                    pData->ty = 0;

                    p.primitives.push_back(primitive);
                }
            }

            float Render::getTextureAtlasPercentage() const
            {
                return _p->textureAtlas->getPercentageUsed();
            }

            size_t Render::getDynamicTextureCount() const
            {
                return _p->dynamicTextureCache.size();
            }

            size_t Render::getVBOSize() const
            {
                return _p->vbo ? _p->vbo->getSize() : 0;
            }

            void Render::_updateImageFilter()
            {
                DJV_PRIVATE_PTR();
                p.dynamicTextures.clear();
                p.dynamicTextureCache.clear();
                for (size_t i = 0; i < dynamicTextureCount; ++i)
                {
                    p.dynamicTextures.emplace_back(
                        AV::OpenGL::Texture::create(
                            AV::Image::Info(),
                            toGL(p.imageFilterOptions.min),
                            toGL(p.imageFilterOptions.mag)));
                }
            }

            void Render::Private::updateVBODataSize(size_t value)
            {
                const size_t vertexByteCount = AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                vboDataSize += value * vertexByteCount;
                if (vboDataSize > vboData.size())
                {
                    vboData.resize(vboDataSize);
                }
            }

            void Render::Private::drawImage(
                const std::shared_ptr<Image::Image>& image,
                const glm::vec2& pos,
                const ImageOptions& options,
                ColorMode colorMode,
                const glm::mat3x3& currentTransform,
                const BBox2f& currentClipRect,
                const float finalColor[4])
            {
                const auto& info = image->getInfo();

                glm::vec3 pts[4];
                pts[0].x = pos.x;
                pts[0].y = pos.y;
                pts[0].z = 1.F;
                pts[1].x = pos.x + info.size.w;
                pts[1].y = pos.y;
                pts[1].z = 1.F;
                pts[2].x = pos.x + info.size.w;
                pts[2].y = pos.y + info.size.h;
                pts[2].z = 1.F;
                pts[3].x = pos.x;
                pts[3].y = pos.y + info.size.h;
                pts[3].z = 1.F;
                for (auto& i : pts)
                {
                    i = currentTransform * i;
                }

                BBox2f bbox;
                bbox.min = pts[0];
                bbox.max = pts[0];
                for (size_t i = 1; i < 4; ++i)
                {
                    bbox.min.x = std::min(bbox.min.x, pts[i].x);
                    bbox.max.x = std::max(bbox.max.x, pts[i].x);
                    bbox.min.y = std::min(bbox.min.y, pts[i].y);
                    bbox.max.y = std::max(bbox.max.y, pts[i].y);
                }

                if (bbox.intersects(currentClipRect))
                {
                    auto primitive = std::make_shared<ImagePrimitive>();
                    primitive->clipRect = currentClipRect;
                    primitive->imageChannels = Image::getChannels(info.type);
                    primitive->colorMode = colorMode;
                    primitive->color[0] = finalColor[0];
                    primitive->color[1] = finalColor[1];
                    primitive->color[2] = finalColor[2];
                    primitive->color[3] = finalColor[3];
                    primitive->imageChannelDisplay = options.channelDisplay;
                    primitive->alphaBlend = options.alphaBlend;
                    primitive->colorMatrixEnabled = options.colorEnabled && options.color != ImageColor();
                    if (primitive->colorMatrixEnabled)
                    {
                        primitive->colorMatrix = colorMatrix(options.color);
                    }
                    primitive->colorInvert = options.colorEnabled && options.color.invert;
                    primitive->levels = options.levels;
                    primitive->levelsEnabled = options.levelsEnabled && options.levels != ImageLevels();
                    primitive->exposureEnabled = options.exposureEnabled;
                    if (primitive->exposureEnabled)
                    {
                        primitive->exposureV = powf(
                            2.F,
                            options.exposure.exposure + 2.47393F);
                        primitive->exposureD = options.exposure.defog;
                        primitive->exposureK = powf(
                            2.F,
                            options.exposure.kneeLow);
                        primitive->exposureF = knee2(
                            powf(2.F, options.exposure.kneeHigh) -
                            primitive->exposureK,
                            powf(2.F, 3.5F) - primitive->exposureK);
                    }
                    primitive->softClip = options.softClipEnabled ? options.softClip : 0.F;
                    primitive->imageCache = options.cache;
                    float textureU[2] = { 0.F, 0.F };
                    float textureV[2] = { 0.F, 0.F };
                    const UID uid = image->getUID();
                    switch (options.cache)
                    {
                    case ImageCache::Atlas:
                    {
                        OpenGL::TextureAtlasItem item;
                        uint64_t id = 0;
                        const auto i = textureIDs.find(uid);
                        if (i != textureIDs.end())
                        {
                            id = i->second;
                        }
                        if (!textureAtlas->getItem(id, item))
                        {
                            textureIDs[uid] = textureAtlas->addItem(image, item);
                        }
                        primitive->atlasIndex = item.textureIndex;
                        if (info.layout.mirror.x)
                        {
                            textureU[0] = item.textureU.getMax();
                            textureU[1] = item.textureU.getMin();
                        }
                        else
                        {
                            textureU[0] = item.textureU.getMin();
                            textureU[1] = item.textureU.getMax();
                        }
                        if (info.layout.mirror.y)
                        {
                            textureV[0] = item.textureV.getMax();
                            textureV[1] = item.textureV.getMin();
                        }
                        else
                        {
                            textureV[0] = item.textureV.getMin();
                            textureV[1] = item.textureV.getMax();
                        }
                        break;
                    }
                    case ImageCache::Dynamic:
                    {
                        const auto i = dynamicTextureCache.find(uid);
                        if (i != dynamicTextureCache.end())
                        {
                            primitive->textureID = i->second->getID();
                        }
                        else
                        {
                            std::shared_ptr<OpenGL::Texture> texture;
                            if (dynamicTextures.size())
                            {
                                texture = dynamicTextures.back();
                                dynamicTextures.pop_back();
                                texture->set(image->getInfo());
                            }
                            else
                            {
                                texture = OpenGL::Texture::create(image->getInfo(), GL_LINEAR, GL_NEAREST);
                            }
                            texture->copy(*image);
                            dynamicTextureCache[uid] = texture;
                            primitive->textureID = texture->getID();
                        }
                        if (info.layout.mirror.x)
                        {
                            textureU[0] = 1.F;
                            textureU[1] = 0.F;
                        }
                        else
                        {
                            textureU[0] = 0.F;
                            textureU[1] = 1.F;
                        }
                        if (info.layout.mirror.y)
                        {
                            textureV[0] = 1.F;
                            textureV[1] = 0.F;
                        }
                        else
                        {
                            textureV[0] = 0.F;
                            textureV[1] = 1.F;
                        }
                        break;
                    }
                    default: break;
                    }
                    if (options.mirror.x)
                    {
                        textureU[0] = 1.F - textureU[0];
                        textureU[1] = 1.F - textureU[1];
                    }
                    if (options.mirror.y)
                    {
                        textureV[0] = 1.F - textureV[0];
                        textureV[1] = 1.F - textureV[1];
                    }
#if !defined(DJV_OPENGL_ES2)
                    if (options.colorSpace.isValid())
                    {
                        ColorSpaceData colorSpaceData;
                        const auto i = colorSpaceCache.find(options.colorSpace);
                        if (i != colorSpaceCache.end())
                        {
                            colorSpaceData = i->second;
                        }
                        else
                        {
                            try
                            {
                                colorSpaceData.id = colorSpaceID++;
                                colorSpaceData.lut3D.reset(new LUT3D);
                                auto config = _OCIO::GetCurrentConfig();
                                auto processor = config->getProcessor(options.colorSpace.input.c_str(), options.colorSpace.output.c_str());
                                _OCIO::GpuShaderDesc shaderDesc;
                                shaderDesc.setLanguage(_OCIO::GPU_LANGUAGE_GLSL_1_3);
                                std::stringstream ss;
                                ss << "colorSpace" << colorSpaceData.id;
                                shaderDesc.setFunctionName(ss.str().c_str());
                                shaderDesc.setLut3DEdgeLen(colorSpaceData.lut3D->getEdgeLen());
                                colorSpaceData.shaderSource = processor->getGpuShaderText(shaderDesc);
                                size_t index = colorSpaceData.shaderSource.find("texture3D");
                                if (index != std::string::npos)
                                {
                                    colorSpaceData.shaderSource.replace(index, std::string("texture3D").size(), "texture");
                                }
                                auto data = colorSpaceData.lut3D->getData();
                                processor->getGpuLut3D(data, shaderDesc);
                                colorSpaceData.lut3D->copy();
                                colorSpaceCache[options.colorSpace] = colorSpaceData;
                                shader.reset();
                            }
                            catch (const std::exception& e)
                            {
                                system->_log(e.what());
                            }
                        }
                        primitive->colorSpace = colorSpaceData.id;
                        primitive->colorSpaceTextureID = colorSpaceData.lut3D ? colorSpaceData.lut3D->getID() : 0;
                    }
#endif // DJV_OPENGL_ES2
                    primitive->type = GL_TRIANGLE_STRIP;
                    primitive->vaoOffset = vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive->vaoSize = 4;

                    const size_t vboDataOffset = vboDataSize;
                    updateVBODataSize(4);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&vboData[vboDataOffset]);
                    pData->vx = pts[0].x;
                    pData->vy = pts[0].y;
                    pData->tx = static_cast<uint16_t>(textureU[0] * 65535.F);
                    pData->ty = static_cast<uint16_t>(textureV[0] * 65535.F);
                    ++pData;
                    pData->vx = pts[1].x;
                    pData->vy = pts[1].y;
                    pData->tx = static_cast<uint16_t>(textureU[1] * 65535.F);
                    pData->ty = static_cast<uint16_t>(textureV[0] * 65535.F);
                    ++pData;
                    pData->vx = pts[3].x;
                    pData->vy = pts[3].y;
                    pData->tx = static_cast<uint16_t>(textureU[0] * 65535.F);
                    pData->ty = static_cast<uint16_t>(textureV[1] * 65535.F);
                    ++pData;
                    pData->vx = pts[2].x;
                    pData->vy = pts[2].y;
                    pData->tx = static_cast<uint16_t>(textureU[1] * 65535.F);
                    pData->ty = static_cast<uint16_t>(textureV[1] * 65535.F);

                    primitives.push_back(primitive);
                }
            }

            std::string Render::Private::getFragmentSource() const
            {
                std::string out = fragmentSource;

                std::string functions;
                std::string body;
#if !defined(DJV_OPENGL_ES2)
                size_t i = 0;
                for (const auto& j : colorSpaceCache)
                {
                    functions += j.second.shaderSource;
                    {
                        std::stringstream ss;
                        if (0 == i)
                        {
                            ss << "    if (" << j.second.id << " == colorSpace)\n";
                        }
                        else
                        {
                            ss << "    else if (" << j.second.id << " == colorSpace)\n";
                        }
                        ss << "    {\n";
                        ss << "        t = colorSpace" << j.second.id << "(t, colorSpaceSampler);\n";
                        ss << "    }\n";
                        body += ss.str();
                    }
                    ++i;
                }

                std::string token = "//$colorSpaceFunctions";
                i = out.find(token);
                if (i != std::string::npos)
                {
                    out.replace(i, token.size(), functions);
                }
                token = "//$colorSpaceBody";
                i = out.find(token);
                if (i != std::string::npos)
                {
                    out.replace(i, token.size(), body);
                }
#endif // DJV_OPENGL_ES2

                return out;
            }

        } // namespace Render2D
    } // namespace AV
} // namespace djv
