// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAV/Render2DPrivate.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Render2D
        {
            Primitive::~Primitive()
            {}

            void Primitive::bind(const PrimitiveData& data, const std::shared_ptr<OpenGL::Shader>& shader)
            {
                shader->setUniform(data.colorModeLoc, static_cast<int>(ColorMode::SolidColor));
                shader->setUniform(data.colorLoc, reinterpret_cast<const GLfloat*>(color));
            }

            void TextPrimitive::bind(const PrimitiveData& data, const std::shared_ptr<OpenGL::Shader>& shader)
            {
                if (!textLCDRendering)
                {
                    shader->setUniform(data.colorModeLoc, static_cast<int>(ColorMode::ColorWithTextureAlpha));
                }
                shader->setUniform(data.colorLoc, reinterpret_cast<const GLfloat*>(color));
                shader->setUniform(data.textureSamplerLoc, static_cast<int>(atlasIndex));
            }

            void ImagePrimitive::bind(const PrimitiveData& data, const std::shared_ptr<OpenGL::Shader>& shader)
            {
                shader->setUniform(data.colorModeLoc, static_cast<int>(colorMode));
                shader->setUniform(data.colorLoc, reinterpret_cast<const GLfloat*>(color));
                shader->setUniform(data.imageChannelsLoc, static_cast<int>(imageChannels));
                if (colorMatrixEnabled)
                {
                    shader->setUniform(data.colorMatrixLoc, colorMatrix);
                }
                shader->setUniform(data.colorMatrixEnabledLoc, colorMatrixEnabled);
                shader->setUniform(data.colorInvertLoc, colorInvert);
                if (levelsEnabled)
                {
                    shader->setUniform(data.levelsInLowLoc, levels.inLow);
                    shader->setUniform(data.levelsInHighLoc, levels.inHigh);
                    shader->setUniform(data.levelsGammaLoc, 1.F / levels.gamma);
                    shader->setUniform(data.levelsOutLowLoc, levels.outLow);
                    shader->setUniform(data.levelsOutHighLoc, levels.outHigh);
                }
                shader->setUniform(data.levelsEnabledLoc, levelsEnabled);
                if (exposureEnabled)
                {
                    shader->setUniform(data.exposureVLoc, exposureV);
                    shader->setUniform(data.exposureDLoc, exposureD);
                    shader->setUniform(data.exposureKLoc, exposureK);
                    shader->setUniform(data.exposureFLoc, exposureF);
                }
                shader->setUniform(data.exposureEnabledLoc, exposureEnabled);
                shader->setUniform(data.softClipLoc, softClip);
#if !defined(DJV_OPENGL_ES2)
                shader->setUniform(data.colorSpaceLoc, colorSpace);
                if (colorSpace > 0)
                {
                    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + data.textureAtlasCount + 1));
                    glBindTexture(GL_TEXTURE_3D, colorSpaceTextureID);
                    shader->setUniform(data.colorSpaceSamplerLoc, static_cast<int>(data.textureAtlasCount + 1));
                }
#endif // DJV_OPENGL_ES2
                shader->setUniform(data.imageChannelDisplayLoc, static_cast<int>(imageChannelDisplay));
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
            }

            void ShadowPrimitive::bind(const PrimitiveData& data, const std::shared_ptr<OpenGL::Shader>& shader)
            {
                shader->setUniform(data.colorModeLoc, static_cast<int>(ColorMode::Shadow));
                shader->setUniform(data.colorLoc, reinterpret_cast<const GLfloat*>(color));
            }

            void TexturePrimitive::bind(const PrimitiveData& data, const std::shared_ptr<OpenGL::Shader>& shader)
            {
                shader->setUniform(data.colorModeLoc, static_cast<int>(ColorMode::ColorAndTexture));
                shader->setUniform(data.colorLoc, reinterpret_cast<const GLfloat*>(color));
                glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + data.textureAtlasCount));
                glBindTexture(target, textureID);
                shader->setUniform(data.textureSamplerLoc, static_cast<int>(data.textureAtlasCount));
            }

#if !defined(DJV_OPENGL_ES2)

            LUT3D::LUT3D(size_t edgeLen) :
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

            LUT3D::~LUT3D()
            {
                if (_id)
                {
                    glDeleteTextures(1, &_id);
                    _id = 0;
                }
                delete[] _data;
            }

            size_t LUT3D::getEdgeLen() const
            {
                return _edgeLen;
            }

            size_t LUT3D::getSize() const
            {
                return _size;
            }

            float* LUT3D::getData()
            {
                return _data;
            }

            const float* LUT3D::getData() const
            {
                return _data;
            }

            GLuint LUT3D::getID() const
            {
                return _id;
            }

            void LUT3D::copy()
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

            void LUT3D::bind()
            {
                glBindTexture(GL_TEXTURE_3D, _id);
            }

#endif // DJV_OPENGL_ES2

            BBox2f flip(const BBox2f& value, const Image::Size& size)
            {
                BBox2f out;
                out.min.x = value.min.x;
                out.min.y = static_cast<float>(size.h) - value.max.y;
                out.max.x = value.max.x;
                out.max.y = static_cast<float>(size.h) - value.min.y;
                return out;
            }

            float knee(float x, float f)
            {
                return logf(x * f + 1.F) / f;
            }

            float knee2(float x, float y)
            {
                float f0 = 0.F;
                float f1 = 1.F;
                while (knee(x, f1) > y)
                {
                    f0 = f1;
                    f1 = f1 * 2.F;
                }
                for (size_t i = 0; i < 30; ++i)
                {
                    const float f2 = (f0 + f1) / 2.F;
                    if (knee(x, f2) < y)
                    {
                        f1 = f2;
                    }
                    else
                    {
                        f0 = f2;
                    }
                }
                return (f0 + f1) / 2.F;
            }

            glm::mat4x4 brightnessMatrix(float r, float g, float b)
            {
                return glm::mat4x4(
                    r, 0.F, 0.F, 0.F,
                    0.F, g, 0.F, 0.F,
                    0.F, 0.F, b, 0.F,
                    0.F, 0.F, 0.F, 1.F);
            }

            glm::mat4x4 contrastMatrix(float r, float g, float b)
            {
                return
                    glm::mat4x4(
                        1.F, 0.F, 0.F, -.5F,
                        0.F, 1.F, 0.F, -.5F,
                        0.F, 0.F, 1.F, -.5F,
                        0.F, 0.F, 0.F, 1.F) *
                    glm::mat4x4(
                        r, 0.F, 0.F, 0.F,
                        0.F, g, 0.F, 0.F,
                        0.F, 0.F, b, 0.F,
                        0.F, 0.F, 0.F, 1.F) *
                    glm::mat4x4(
                        1.F, 0.F, 0.F, .5F,
                        0.F, 1.F, 0.F, .5F,
                        0.F, 0.F, 1.F, .5F,
                        0.F, 0.F, 0.F, 1.F);
            }

            glm::mat4x4 saturationMatrix(float r, float g, float b)
            {
                const float s[] =
                {
                    (1.F - r) * .3086F,
                    (1.F - g) * .6094F,
                    (1.F - b) * .0820F
                };
                return glm::mat4x4(
                    s[0] + r, s[1], s[2], 0.F,
                    s[0], s[1] + g, s[2], 0.F,
                    s[0], s[1], s[2] + b, 0.F,
                    0.F, 0.F, 0.F, 1.F);
            }

            glm::mat4x4 colorMatrix(const ImageColor& in)
            {
                return
                    brightnessMatrix(in.brightness, in.brightness, in.brightness) *
                    contrastMatrix(in.contrast, in.contrast, in.contrast) *
                    saturationMatrix(in.saturation, in.saturation, in.saturation);
            }

        } // namespace Render2D
    } // namespace AV
} // namespace djv
