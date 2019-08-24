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
#include <djvCore/FileIO.h>
#include <djvCore/LogSystem.h>
#include <djvCore/Range.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/Timer.h>

#include <OpenColorIO/OpenColorIO.h>

#include <glm/gtc/matrix_transform.hpp>

#include <codecvt>
#include <list>
#include <locale>

using namespace djv::Core;
namespace _OCIO = OCIO_NAMESPACE;

namespace djv
{
    namespace AV
    {
        namespace Render
        {
            namespace
            {
                //! \todo Should this be configurable?
                const size_t textureAtlasCount      = 4;
                const size_t textureAtlasSize       = 8192;
                const size_t dynamicTextureIDCount  = 16;
                const size_t dynamicTextureCacheMax = 16;
                const size_t lut3DSize              = 32;
                const size_t colorSpaceCacheMax     = 32;

                // This enumeration provides how the color is used to draw the render primitive.
                enum class ColorMode
                {
                    SolidColor,             // Use the uniform variable "color"
                    ColorWithTextureAlpha,  // Use the uniform variable "color" with the alpha multiplied
                                            // by the red channel from the texture (e.g., used for
                                            // drawing text)
                    ColorWithTextureAlphaR, // Experimental: used for drawing text with LCD sub-sampling
                    ColorWithTextureAlphaG,
                    ColorWithTextureAlphaB,
                    ColorAndTexture,        // Use the uniform variable "color" multiplied by the texture     
                    Shadow                  // Use the uniform variable "color" multiplied by the "U" texture
                                            // coordinate
                };

                //! This struct provides data used to draw the render primitive.
                struct PrimitiveData
                {
                    // Used as an offset to find textures.
                    size_t textureAtlasCount = 0;

                    // Shader uniform variable locations.
                    GLint colorModeLoc          = 0;
                    GLint colorLoc              = 0;
                    GLint imageChannelsLoc      = 0;
                    GLint imageChannelLoc       = 0;
                    GLint textureSamplerLoc     = 0;
                    GLint colorSpaceLoc         = 0;
                    GLint colorSpaceSamplerLoc  = 0;
                };

                //! This class provides the base functionality for render primitives.
                struct Primitive
                {
                    virtual ~Primitive() {}
                    
                    BBox2f clipRect;
                    float  color[4]  = { 0.f, 0.f, 0.f, 0.f };
                    size_t vaoOffset = 0;
                    size_t vaoSize   = 0;

                    virtual void bind(const PrimitiveData& data, const std::shared_ptr<OpenGL::Shader>& shader)
                    {
                        shader->setUniform(data.colorModeLoc, static_cast<int>(ColorMode::SolidColor));
                        shader->setUniform(data.colorLoc, reinterpret_cast<const GLfloat*>(color));
                    }
                };

                //! This class provides a text render primitive.
                struct TextPrimitive : public Primitive
                {
                    uint8_t atlasIndex = 0;

                    void bind(const PrimitiveData& data, const std::shared_ptr<OpenGL::Shader>& shader) override
                    {
                        shader->setUniform(data.colorModeLoc, static_cast<int>(ColorMode::ColorWithTextureAlpha));
                        shader->setUniform(data.colorLoc, reinterpret_cast<const GLfloat*>(color));
                        shader->setUniform(data.textureSamplerLoc, static_cast<int>(atlasIndex));
                    }
                };

                //! This class provides an image render primitive.
                struct ImagePrimitive : public Primitive
                {
                    ColorMode       colorMode           = ColorMode::SolidColor;
                    Image::Channels imageChannels       = Image::Channels::RGBA;
                    ImageChannel    imageChannel        = ImageChannel::None;
                    ImageCache      imageCache          = ImageCache::Atlas;
                    uint8_t         atlasIndex          = 0;
                    GLuint          textureID           = 0;
                    uint8_t         colorSpace          = 0;
                    GLuint          colorSpaceTextureID = 0;

                    void bind(const PrimitiveData& data, const std::shared_ptr<OpenGL::Shader>& shader) override
                    {
                        shader->setUniform(data.colorModeLoc, static_cast<int>(colorMode));
                        shader->setUniform(data.colorLoc, reinterpret_cast<const GLfloat*>(color));
                        shader->setUniform(data.imageChannelsLoc, static_cast<int>(imageChannels));
                        shader->setUniform(data.imageChannelLoc, static_cast<int>(imageChannel));
                        switch (imageCache)
                        {
                        case ImageCache::Atlas:
                            shader->setUniform(data.textureSamplerLoc, static_cast<int>(atlasIndex));
                            break;
                        case ImageCache::Dynamic:
                            glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + data.textureAtlasCount));
                            glBindTexture(GL_TEXTURE_2D, textureID);
                            shader->setUniform(data.textureSamplerLoc, static_cast<int>(data.textureAtlasCount));
                            break;
                        default: break;
                        }
                        shader->setUniform(data.colorSpaceLoc, colorSpace);
                        if (colorSpace > 0)
                        {
                            glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + data.textureAtlasCount + 1));
                            glBindTexture(GL_TEXTURE_3D, colorSpaceTextureID);
                            shader->setUniform(data.colorSpaceSamplerLoc, static_cast<int>(data.textureAtlasCount + 1));
                        }
                    }
                };

                //! This class provides a shadow render primitive.
                struct ShadowPrimitive : public Primitive
                {
                    void bind(const PrimitiveData& data, const std::shared_ptr<OpenGL::Shader>& shader) override
                    {
                        shader->setUniform(data.colorModeLoc, static_cast<int>(ColorMode::Shadow));
                        shader->setUniform(data.colorLoc, reinterpret_cast<const GLfloat*>(color));
                    }
                };

                //! This struct provides the layout for a VBO vertex.
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

                //! This class provides a 3D lookup table for color space conversions.
                class LUT3D
                {
                    DJV_NON_COPYABLE(LUT3D);

                public:
                    LUT3D(size_t edgeLen = lut3DSize) :
                        _edgeLen(edgeLen),
                        _size(3 * edgeLen * edgeLen * edgeLen),
                        _data(new float[_size])
                    {
                        glGenTextures(1, &_id);
                        glBindTexture(GL_TEXTURE_3D, _id);
                        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        GLenum internalFormat = GL_RGB;
                        glTexImage3D(
                            GL_TEXTURE_3D,
                            0,
                            internalFormat,
                            edgeLen,
                            edgeLen,
                            edgeLen,
                            0,
                            GL_RGB,
                            GL_FLOAT,
                            0);
                    }

                    ~LUT3D()
                    {
                        if (_id)
                        {
                            glDeleteTextures(1, &_id);
                            _id = 0;
                        }
                        delete[] _data;
                    }

                    size_t getEdgeLen() const { return _edgeLen; }
                    size_t getSize() const { return _size; }
                    float* getData() { return _data; }
                    const float* getData() const { return _data; }
                    GLuint getID() const { return _id; }

                    void copy()
                    {
                        glBindTexture(GL_TEXTURE_3D, _id);
                        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                        glPixelStorei(GL_UNPACK_SWAP_BYTES, 0);
                        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
                        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
                        glPixelStorei(GL_UNPACK_SKIP_IMAGES, 0);
                        glTexSubImage3D(
                            GL_TEXTURE_3D,
                            0,
                            0,
                            0,
                            0,
                            _edgeLen,
                            _edgeLen,
                            _edgeLen,
                            GL_RGB,
                            GL_FLOAT,
                            _data);
                    }

                    void bind()
                    {
                        glBindTexture(GL_TEXTURE_3D, _id);
                    }

                private:
                    size_t _edgeLen = 0;
                    size_t _size = 0;
                    float* _data = nullptr;
                    GLuint _id = 0;
                };

                //! This variable provides color space conversion ids.
                size_t colorSpaceID = 1;

                //! This struct provides data for color space conversions.
                struct ColorSpaceData
                {
                    size_t                  id;
                    std::string             shaderSource;
                    std::shared_ptr<LUT3D>  lut3D;
                };

                // Utility function to flip the y-coordinate.
                BBox2f flip(const BBox2f& value, const Image::Size& size)
                {
                    BBox2f out;
                    out.min.x = value.min.x;
                    out.min.y = static_cast<float>(size.h) - value.max.y;
                    out.max.x = value.max.x;
                    out.max.y = static_cast<float>(size.h) - value.min.y;
                    return out;
                }

            } // namespace

            struct Render2D::Private
            {
                Render2D* system = nullptr;

                Image::Size                  size;
                std::list<glm::mat3x3>       transforms;
                glm::mat3x3                  currentTransform = glm::mat3x3(1.f);
                std::list<BBox2f>            clipRects;
                BBox2f                       currentClipRect  = BBox2f(0.f, 0.f, 0.f, 0.f);
                float                        fillColor[4]     = { 1.f, 1.f, 1.f, 1.f };
                float                        colorMult        = 1.f;
                float                        alphaMult        = 1.f;
                float                        finalColor[4]    = { 1.f, 1.f, 1.f, 1.f };
                std::weak_ptr<Font::System>  fontSystem;
                Font::Info                   currentFont;

                BBox2f                                              viewport;
                std::vector<Primitive*>                             primitives;
                PrimitiveData                                       primitiveData;
                std::shared_ptr<TextureAtlas>                       textureAtlas;
                std::map<UID, uint64_t>                             textureIDs;
                std::map<UID, uint64_t>                             glyphTextureIDs;
                std::vector<std::shared_ptr<OpenGL::Texture> >      dynamicTextureIDs;
                std::map<UID, std::shared_ptr<OpenGL::Texture> >    dynamicTextureCache;
                std::map<OCIO::Convert, ColorSpaceData>             colorSpaceCache;
                std::vector<uint8_t>                                vboData;
                size_t                                              vboDataSize         = 0;
                std::shared_ptr<OpenGL::VBO>                        vbo;
                std::shared_ptr<OpenGL::VAO>                        vao;
                std::string                                         vertexSource;
                std::string                                         fragmentSource;
                std::shared_ptr<OpenGL::Shader>                     shader;
                GLint                                               mvpLoc              = 0;

                std::shared_ptr<Time::Timer>                        statsTimer;
                std::vector<float>                                  fpsSamples;
                std::chrono::time_point<std::chrono::system_clock>  fpsTime             = std::chrono::system_clock::now();

                void updateCurrentTransform();
                void updateCurrentClipRect();
                void updateVBODataSize(size_t);

                void drawImage(
                    const std::shared_ptr<Image::Image>&,
                    const glm::vec2& pos,
                    const ImageOptions&,
                    ColorMode);

                std::string getFragmentSource() const;
            };

            void Render2D::_init(const std::shared_ptr<Core::Context>& context)
            {
                ISystem::_init("djv::AV::Render::Render2D", context);

                DJV_PRIVATE_PTR();
                p.system = this;

                auto fontSystem = context->getSystemT<Font::System>();
                p.fontSystem = fontSystem;
                addDependency(fontSystem);

                GLint maxTextureUnits = 0;
                GLint maxTextureSize = 0;
                glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
                glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
                {
                    auto logSystem = context->getSystemT<LogSystem>();
                    std::stringstream ss;
                    ss << "Maximum OpenGL texture units: " << maxTextureUnits << "\n";
                    ss << "Maximum OpenGL texture size: " << maxTextureSize;
                    logSystem->log("djv::AV::Render::Render2D", ss.str());
                }
                const size_t _textureAtlasCount = std::min(size_t(maxTextureUnits), textureAtlasCount);
                const int _textureAtlasSize = std::min(maxTextureSize, int(textureAtlasSize));
                {
                    auto logSystem = context->getSystemT<LogSystem>();
                    std::stringstream ss;
                    ss << "Texture atlas count: " << _textureAtlasCount << "\n";
                    ss << "Texture atlas size: " << _textureAtlasSize;
                    logSystem->log("djv::AV::Render::Render2D", ss.str());
                }
                p.textureAtlas.reset(new TextureAtlas(
                    _textureAtlasCount,
                    _textureAtlasSize,
                    Image::Type::RGBA_U8,
                    GL_NEAREST,
                    0));
                p.primitiveData.textureAtlasCount = _textureAtlasCount;

                for (size_t i = 0; i < dynamicTextureIDCount; ++i)
                {
                    p.dynamicTextureIDs.push_back(AV::OpenGL::Texture::create(AV::Image::Info(), GL_LINEAR, GL_NEAREST));
                }

                auto resourceSystem = context->getSystemT<ResourceSystem>();
                const FileSystem::Path shaderPath = resourceSystem->getPath(FileSystem::ResourcePath::ShadersDirectory);
                try
                {
                    FileSystem::FileIO io;
                    io.open(FileSystem::Path(shaderPath, "djvAVRender2DVertex.glsl"), FileSystem::FileIO::Mode::Read);
                    p.vertexSource = FileSystem::FileIO::readContents(io);
                    io.close();
                    io.open(FileSystem::Path(shaderPath, "djvAVRender2DFragment.glsl"), FileSystem::FileIO::Mode::Read);
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
                    Time::getMilliseconds(Time::TimerValue::VerySlow),
                    [this](float)
                    {
                        DJV_PRIVATE_PTR();
                        std::stringstream s;
                        s << "Texture atlas: " << p.textureAtlas->getPercentageUsed() << "%\n";
                        s << "Texture IDs: " << p.textureIDs.size() << "%\n";
                        s << "Glyph texture IDs: " << p.glyphTextureIDs.size() << "\n";
                        s << "Dynamic texture IDs: " << p.dynamicTextureIDs.size() << "\n";
                        s << "Dynamic texture cache: " << p.dynamicTextureCache.size() << "\n";
                        s << "Color space cache: " << p.colorSpaceCache.size() << "\n";
                        s << "VBO size: " << (p.vbo ? p.vbo->getSize() : 0);
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

            std::shared_ptr<Render2D> Render2D::create(const std::shared_ptr<Core::Context>& context)
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
                p.viewport = BBox2f(0.f, 0.f, static_cast<float>(size.w), static_cast<float>(size.h));
            }

            void Render2D::endFrame()
            {
                DJV_PRIVATE_PTR();

                if (!p.shader)
                {
                    p.shader = OpenGL::Shader::create(Shader::create(p.vertexSource, p.getFragmentSource()));
                    const auto program = p.shader->getProgram();
                    p.mvpLoc = glGetUniformLocation(program, "transform.mvp");
                    p.primitiveData.imageChannelsLoc = glGetUniformLocation(program, "imageChannels");
                    p.primitiveData.imageChannelLoc = glGetUniformLocation(program, "imageChannel");
                    p.primitiveData.colorModeLoc = glGetUniformLocation(program, "colorMode");
                    p.primitiveData.colorLoc = glGetUniformLocation(program, "color");
                    p.primitiveData.textureSamplerLoc = glGetUniformLocation(program, "textureSampler");
                    p.primitiveData.colorSpaceLoc = glGetUniformLocation(program, "colorSpace");
                    p.primitiveData.colorSpaceSamplerLoc = glGetUniformLocation(program, "colorSpaceSampler");
                }
                p.shader->bind();

#if defined(DJV_OPENGL_ES2)
#else // DJV_OPENGL_ES2
                glEnable(GL_MULTISAMPLE);
#endif // DJV_OPENGL_ES2
                glEnable(GL_SCISSOR_TEST);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
                glClearColor(0.f, 0.f, 0.f, 0.f);
                glClear(GL_COLOR_BUFFER_BIT);

                const auto viewMatrix = glm::ortho(
                    p.viewport.min.x,
                    p.viewport.max.x,
                    p.viewport.max.y,
                    p.viewport.min.y,
                    -1.f, 1.f);
                p.shader->setUniform(p.mvpLoc, viewMatrix);

                const auto& atlasTextures = p.textureAtlas->getTextures();
                for (GLuint i = 0; i < static_cast<GLuint>(atlasTextures.size()); ++i)
                {
                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
                    glBindTexture(GL_TEXTURE_2D, atlasTextures[i]);
                }

                const size_t vertexByteCount = AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                if (!p.vbo || p.vboDataSize / 3 / vertexByteCount > p.vbo->getSize())
                {
                    p.vbo = OpenGL::VBO::create(p.vboDataSize / 3 / vertexByteCount, 3, OpenGL::VBOType::Pos2_F32_UV_U16);
                    p.vao = OpenGL::VAO::create(p.vbo->getType(), p.vbo->getID());
                }
                p.vbo->copy(p.vboData, 0, p.vboDataSize);
                p.vao->bind();

                for (size_t i = 0; i < p.primitives.size(); ++i)
                {
                    const auto& primitive = p.primitives[i];
                    const BBox2f clipRect = flip(primitive->clipRect, p.size);
                    glScissor(
                        static_cast<GLint>(clipRect.min.x),
                        static_cast<GLint>(clipRect.min.y),
                        static_cast<GLsizei>(clipRect.w()),
                        static_cast<GLsizei>(clipRect.h()));
                    primitive->bind(p.primitiveData, p.shader);
                    p.vao->draw(primitive->vaoOffset, primitive->vaoSize);
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
                for (size_t i = 0; i < p.primitives.size(); ++i)
                {
                    delete p.primitives[i];
                }
                p.primitives.clear();
                p.vboDataSize = 0;
                while (p.dynamicTextureCache.size() > dynamicTextureCacheMax)
                {
                    auto texture = p.dynamicTextureCache.begin();
                    p.dynamicTextureIDs.push_back(texture->second);
                    p.dynamicTextureCache.erase(texture);
                }
                while (p.dynamicTextureIDs.size() > dynamicTextureIDCount)
                {
                    p.dynamicTextureIDs.pop_back();
                }
                while (p.colorSpaceCache.size() > colorSpaceCacheMax)
                {
                    p.colorSpaceCache.erase(p.colorSpaceCache.begin());
                    p.shader.reset();
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
                if (value.intersects(p.viewport))
                {
                    auto primitive = new Primitive;
                    p.primitives.push_back(primitive);
                    primitive->clipRect = p.currentClipRect;
                    primitive->color[0] = p.finalColor[0];
                    primitive->color[1] = p.finalColor[1];
                    primitive->color[2] = p.finalColor[2];
                    primitive->color[3] = p.finalColor[3];
                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive->vaoSize = 6;

                    const size_t vboDataSize = p.vboDataSize;
                    p.updateVBODataSize(6);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataSize]);
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
                if (rect.intersects(p.viewport))
                {
                    auto primitive = new Primitive;
                    p.primitives.push_back(primitive);
                    primitive->clipRect = p.currentClipRect;
                    primitive->color[0] = p.finalColor[0];
                    primitive->color[1] = p.finalColor[1];
                    primitive->color[2] = p.finalColor[2];
                    primitive->color[3] = p.finalColor[3];
                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive->vaoSize = 3 * 2 + facets * 2 * 3;

                    const size_t vboDataSize = p.vboDataSize;
                    p.updateVBODataSize(primitive->vaoSize);
                    const float h = rect.h();
                    const float radius = h / 2.f;
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataSize]);
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
                if (rect.intersects(p.viewport))
                {
                    auto primitive = new Primitive;
                    p.primitives.push_back(primitive);
                    primitive->clipRect = p.currentClipRect;
                    primitive->color[0] = p.finalColor[0];
                    primitive->color[1] = p.finalColor[1];
                    primitive->color[2] = p.finalColor[2];
                    primitive->color[3] = p.finalColor[3];
                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);;
                    primitive->vaoSize = 3 * facets;

                    const size_t vboDataSize = p.vboDataSize;
                    p.updateVBODataSize(3 * facets);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataSize]);
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
                const std::shared_ptr<Image::Image> & image,
                const glm::vec2& pos,
                const ImageOptions& options)
            {
                DJV_PRIVATE_PTR();
                p.drawImage(image, pos, options, ColorMode::ColorAndTexture);
            }

            void Render2D::drawFilledImage(
                const std::shared_ptr<Image::Image> & image,
                const glm::vec2& pos,
                const ImageOptions& options)
            {
                DJV_PRIVATE_PTR();
                p.drawImage(image, pos, options, ColorMode::ColorWithTextureAlpha);
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
                        TextPrimitive* primitive = nullptr;
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
                                if (bbox.intersects(p.viewport))
                                {
                                    const auto uid = glyph->imageData->getUID();
                                    uint64_t id = 0;
                                    const auto i = p.glyphTextureIDs.find(uid);
                                    if (i != p.glyphTextureIDs.end())
                                    {
                                        id = i->second;
                                    }
                                    TextureAtlasItem item;
                                    if (!p.textureAtlas->getItem(id, item))
                                    {
                                        id = p.textureAtlas->addItem(glyph->imageData, item);
                                        p.glyphTextureIDs[uid] = id;
                                    }

                                    if (!primitive)
                                    {
                                        primitive = new TextPrimitive;
                                        p.primitives.push_back(primitive);
                                        primitive->clipRect = p.currentClipRect;
                                        primitive->color[0] = p.finalColor[0];
                                        primitive->color[1] = p.finalColor[1];
                                        primitive->color[2] = p.finalColor[2];
                                        primitive->color[3] = p.finalColor[3];
                                        primitive->atlasIndex = item.textureIndex;
                                        primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);;
                                        primitive->vaoSize = 6;
                                    }
                                    else
                                    {
                                        primitive->vaoSize += 6;
                                    }

                                    const size_t vboDataSize = p.vboDataSize;
                                    p.updateVBODataSize(6);
                                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataSize]);
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
                if (value.intersects(p.viewport))
                {
                    auto primitive = new ShadowPrimitive;
                    p.primitives.push_back(primitive);
                    primitive->clipRect = p.currentClipRect;
                    primitive->color[0] = p.finalColor[0];
                    primitive->color[1] = p.finalColor[1];
                    primitive->color[2] = p.finalColor[2];
                    primitive->color[3] = p.finalColor[3];
                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive->vaoSize = 6;

                    static const uint16_t u[][6] =
                    {
                        { 0, 0, 0, 0, 0, 0 },
                        { 0, 65535, 65535, 65535, 0, 0 },
                        { 0, 0, 65535, 65535, 65535, 0 },
                        { 65535, 0, 0, 0, 65535, 65535 },
                        { 65535, 65535, 0, 0, 0, 65535 }
                    };

                    const size_t vboDataSize = p.vboDataSize;
                    p.updateVBODataSize(6);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataSize]);
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
                if (value.intersects(p.viewport))
                {
                    auto primitive = new ShadowPrimitive;
                    p.primitives.push_back(primitive);
                    primitive->clipRect = p.currentClipRect;
                    primitive->color[0] = p.finalColor[0];
                    primitive->color[1] = p.finalColor[1];
                    primitive->color[2] = p.finalColor[2];
                    primitive->color[3] = p.finalColor[3];
                    primitive->vaoOffset = p.vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive->vaoSize = 5 * 2 * 3 + 4 * facets * 3;

                    const size_t vboDataSize = p.vboDataSize;
                    p.updateVBODataSize(primitive->vaoSize);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&p.vboData[vboDataSize]);

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
                return _p->textureAtlas->getPercentageUsed();
            }

            size_t Render2D::getDynamicTextureCount() const
            {
                return _p->dynamicTextureCache.size();
            }

            size_t Render2D::getVBOSize() const
            {
                return _p->vbo ? _p->vbo->getSize() : 0;
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

            void Render2D::Private::updateVBODataSize(size_t value)
            {
                const size_t vertexByteCount = AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                vboDataSize += value * vertexByteCount;
                if (vboDataSize > vboData.size())
                {
                    vboData.resize(vboDataSize);
                }
            }

            void Render2D::Private::drawImage(
                const std::shared_ptr<Image::Image>& image,
                const glm::vec2& pos,
                const ImageOptions& options,
                ColorMode colorMode)
            {
                const auto& info = image->getInfo();

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

                if (bbox.intersects(viewport))
                {
                    auto primitive = new ImagePrimitive;
                    primitives.push_back(primitive);
                    primitive->clipRect = currentClipRect;
                    primitive->imageChannels = Image::getChannels(info.type);
                    primitive->colorMode = colorMode;
                    primitive->color[0] = finalColor[0];
                    primitive->color[1] = finalColor[1];
                    primitive->color[2] = finalColor[2];
                    primitive->color[3] = finalColor[3];
                    primitive->imageChannel = options.channel;
                    primitive->imageCache = options.cache;
                    FloatRange textureU;
                    FloatRange textureV;
                    const UID uid = image->getUID();
                    switch (options.cache)
                    {
                    case ImageCache::Atlas:
                    {
                        TextureAtlasItem item;
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
                        const auto i = dynamicTextureCache.find(uid);
                        if (i != dynamicTextureCache.end())
                        {
                            primitive->textureID = i->second->getID();
                        }
                        else
                        {
                            std::shared_ptr<OpenGL::Texture> texture;
                            if (dynamicTextureIDs.size())
                            {
                                texture = dynamicTextureIDs.back();
                                dynamicTextureIDs.pop_back();
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
                    const std::string& imageColorSpace = image->getColorSpace();
                    const OCIO::Convert colorSpace(
                        !options.colorSpace.input.empty() ? options.colorSpace.input : imageColorSpace,
                        options.colorSpace.output);
                    if (colorSpace.isValid())
                    {
                        ColorSpaceData colorSpaceData;
                        const auto i = colorSpaceCache.find(colorSpace);
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
                                auto processor = config->getProcessor(colorSpace.input.c_str(), colorSpace.output.c_str());
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
                                colorSpaceCache[colorSpace] = colorSpaceData;
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
                    primitive->vaoOffset = vboDataSize / AV::OpenGL::getVertexByteCount(OpenGL::VBOType::Pos2_F32_UV_U16);
                    primitive->vaoSize = 6;

                    const size_t vboDataSize = this->vboDataSize;
                    updateVBODataSize(6);
                    VBOVertex* pData = reinterpret_cast<VBOVertex*>(&vboData[vboDataSize]);
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

            std::string Render2D::Private::getFragmentSource() const
            {
                std::string out = fragmentSource;

                std::string functions;
                std::string body;
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

                return out;
            }

        } // namespace Render
    } // namespace AV
} // namespace djv
