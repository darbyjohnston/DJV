//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvGraphics/OpenGLImage.h>

#include <djvGraphics/OpenGLLUT.h>
#include <djvGraphics/OpenGLOffscreenBuffer.h>
#include <djvGraphics/OpenGLShader.h>
#include <djvGraphics/OpenGLTexture.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/Debug.h>
#include <djvCore/Error.h>
#include <djvCore/Vector.h>

#include <QCoreApplication>

#include <glm/gtc/matrix_transform.hpp>

namespace djv
{
    namespace Graphics
    {
        OpenGLImageMesh::OpenGLImageMesh() :
            _vertexSize(8 + 8)
        {
            //DJV_DEBUG("OpenGLImageMesh::OpenGLImageMesh");

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
            glFuncs->glGenBuffers(1, &_vbo);
            glFuncs->glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glFuncs->glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(6 * _vertexSize), 0, GL_DYNAMIC_DRAW);

            glFuncs->glGenVertexArrays(1, &_vao);
            glFuncs->glBindVertexArray(_vao);
            glFuncs->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(_vertexSize), (GLvoid*)0);
            glFuncs->glEnableVertexAttribArray(0);
            glFuncs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(_vertexSize), (GLvoid*)8);
            glFuncs->glEnableVertexAttribArray(1);
        }

        OpenGLImageMesh::~OpenGLImageMesh()
        {
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
            glFuncs->glDeleteVertexArrays(1, &_vao);
            glFuncs->glDeleteBuffers(1, &_vbo);
        }

        void OpenGLImageMesh::setSize(const glm::ivec2& size, const PixelDataInfo::Mirror & mirror, int proxyScale)
        {
            if (size == _size && mirror == _mirror && proxyScale == _proxyScale)
                return;

            //DJV_DEBUG("OpenGLImageMesh::setSize");
            //DJV_DEBUG_PRINT("size = " << size);
            //DJV_DEBUG_PRINT("mirror = " << mirror);
            //DJV_DEBUG_PRINT("proxy = " << proxyScale);

            _size = size;
            _mirror = mirror;
            _proxyScale = proxyScale;

            const glm::vec2 p[] =
            {
                glm::vec2(0.f, 0.f),
                glm::vec2(_size.x * _proxyScale, 0.f),
                glm::vec2(_size.x * _proxyScale, _size.y * _proxyScale),

                glm::vec2(_size.x * _proxyScale, _size.y * _proxyScale),
                glm::vec2(0.f, _size.y * _proxyScale),
                glm::vec2(0.f, 0.f)
            };

            float u[] = { 0.f, 0.f };
            float v[] = { 0.f, 0.f };
            u[!_mirror.x] = 1.f;
            v[!_mirror.y] = 1.f;
            //DJV_DEBUG_PRINT("u = " << u[0] << " " << u[1]);
            //DJV_DEBUG_PRINT("v = " << v[0] << " " << v[1]);
            const glm::vec2 uv[] =
            {
                glm::vec2(u[0], v[0]),
                glm::vec2(u[1], v[0]),
                glm::vec2(u[1], v[1]),

                glm::vec2(u[1], v[1]),
                glm::vec2(u[0], v[1]),
                glm::vec2(u[0], v[0])
            };

            std::vector<uint8_t> vertices(6 * _vertexSize);
            uint8_t* verticesP = vertices.data();
            for (size_t i = 0; i < 6; ++i)
            {
                float* pf = reinterpret_cast<float*>(verticesP);
                pf[0] = p[i].x;
                pf[1] = p[i].y;
                verticesP += 8;

                pf = reinterpret_cast<float*>(verticesP);
                pf[0] = uv[i].x;
                pf[1] = uv[i].y;
                verticesP += 8;
            }

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
            glFuncs->glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glFuncs->glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizei>(6 * _vertexSize), vertices.data());

            //glFuncs->glBindVertexArray(_vao);
            //glFuncs->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(_vertexSize), (GLvoid*)0);
            //glFuncs->glEnableVertexAttribArray(0);
            //glFuncs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(_vertexSize), (GLvoid*)8);
            //glFuncs->glEnableVertexAttribArray(1);
        }

        void OpenGLImageMesh::draw()
        {
            //DJV_DEBUG("OpenGLImageMesh::draw");
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
            glFuncs->glBindVertexArray(_vao);
            glFuncs->glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        namespace
        {
            typedef float (FilterFnc)(const float t);

            static const float supportBox = .5f;

            static float filterBox(float t)
            {
                if (t > -.5f && t <= .5f)
                {
                    return 1.f;
                }
                return 0.f;
            }

            static const float supportTriangle = 1.f;

            static float filterTriangle(float t)
            {
                if (t < 0.f)
                {
                    t = -t;
                }
                if (t < 1.f)
                {
                    return 1.f - t;
                }
                return 0.f;
            }

            static const float supportBell = 1.5f;

            static float filterBell(float t)
            {
                if (t < 0.f)
                {
                    t = -t;
                }
                if (t < .5f)
                {
                    return .75f - t * t;
                }
                if (t < 1.5f)
                {
                    t = t - 1.5f;
                    return .5f * t * t;
                }
                return 0.f;
            }

            static const float supportBSpline = 2.f;

            static float filterBSpline(float t)
            {
                if (t < 0.f)
                {
                    t = -t;
                }
                if (t < 1.f)
                {
                    const float tt = t * t;
                    return (.5f * tt * t) - tt + 2.f / 3.f;
                }
                else if (t < 2.f)
                {
                    t = 2.f - t;
                    return (1.f / 6.f) * (t * t * t);
                }
                return 0.f;
            }

            static float sinc(float x)
            {
                x *= Core::Math::pi;
                if (x != 0.f)
                {
                    return Core::Math::sin(x) / x;
                }
                return 1.f;
            }

            static const float supportLanczos3 = 3.f;

            static float filterLanczos3(float t)
            {
                if (t < 0.f)
                {
                    t = -t;
                }
                if (t < 3.f)
                {
                    return sinc(t) * sinc(t / 3.f);
                }
                return 0.f;
            }

            static const float supportCubic = 1.f;

            static float filterCubic(float t)
            {
                if (t < 0.f)
                {
                    t = -t;
                }
                if (t < 1.f)
                {
                    return (2.f * t - 3.f) * t * t + 1.f;
                }
                return 0.f;
            }

            static const float supportMitchell = 2.f;

            static float filterMitchell(float t)
            {
                const float tt = t * t;
                static const float b = 1.f / 3.f;
                static const float c = 1.f / 3.f;
                if (t < 0.f)
                {
                    t = -t;
                }
                if (t < 1.f)
                {
                    t =
                        ((12.f - 9.f * b - 6.f * c) * (t * tt)) +
                        ((-18.f + 12.f * b + 6.f * c) * tt) +
                        (6.f - 2.f * b);
                    return t / 6.f;
                }
                else if (t < 2.f)
                {
                    t =
                        ((-1.f * b - 6.f * c) * (t * tt)) +
                        ((6.f * b + 30.f * c) * tt) +
                        ((-12.f * b - 48.f * c) * t) +
                        (8.f * b + 24.f * c);
                    return t / 6.f;
                }
                return 0.f;
            }

            FilterFnc * filterFnc(OpenGLImageFilter::FILTER in)
            {
                static FilterFnc * tmp[] =
                {
                    filterBox,
                    filterBox,
                    filterBox,
                    filterTriangle,
                    filterBell,
                    filterBSpline,
                    filterLanczos3,
                    filterCubic,
                    filterMitchell
                };
                return tmp[in];
            }

            static float filterSupport(OpenGLImageFilter::FILTER in)
            {
                static const float tmp[] =
                {
                    supportBox,
                    supportBox,
                    supportBox,
                    supportTriangle,
                    supportBell,
                    supportBSpline,
                    supportLanczos3,
                    supportCubic,
                    supportMitchell
                };
                return tmp[in];
            }

            int edge(int in, int size)
            {
                return Core::Math::clamp(in, 0, size - 1);
            }

            void scaleContrib(
                int                       input,
                int                       output,
                OpenGLImageFilter::FILTER filter,
                PixelData &               data)
            {
                //DJV_DEBUG("scaleContrib");
                //DJV_DEBUG_PRINT("scale = " << input << " " << output);
                //DJV_DEBUG_PRINT("filter = " << filter);

                // Filter function.
                FilterFnc * fnc = filterFnc(filter);
                const float support = filterSupport(filter);
                //DJV_DEBUG_PRINT("support = " << support);
                const float scale = static_cast<float>(output) / static_cast<float>(input);
                //DJV_DEBUG_PRINT("scale = " << scale);
                const float radius = support * (scale >= 1.f ? 1.f : (1.f / scale));
                //DJV_DEBUG_PRINT("radius = " << radius);

                // Initialize.
                const int width = Core::Math::ceil(radius * 2.f + 1.f);
                //DJV_DEBUG_PRINT("width = " << width);
                data.set(PixelDataInfo(output, width, Pixel::LA_F32));

                // Work.
                for (int i = 0; i < output; ++i)
                {
                    const float center = i / scale;
                    const int   left   = Core::Math::ceil(center - radius);
                    const int   right  = Core::Math::floor(center + radius);
                    //DJV_DEBUG_PRINT(i << " = " << left << " " << center << " " << right);

                    float sum = 0.f;
                    int   pixel = 0;
                    int j = 0;
                    for (int k = left; j < width && k <= right; ++j, ++k)
                    {
                        Pixel::F32_T * p = reinterpret_cast<Pixel::F32_T *>(data.data(i, j));
                        pixel = edge(k, input);
                        const float x = (center - k) * (scale < 1.f ? scale : 1.f);
                        const float w = (scale < 1.f) ? ((*fnc)(x) * scale) : (*fnc)(x);
                        //DJV_DEBUG_PRINT("w = " << w);
                        p[0] = static_cast<Pixel::F32_T>(pixel / static_cast<float>(input));
                        p[1] = static_cast<Pixel::F32_T>(w);
                        sum += w;
                    }

                    for (; j < width; ++j)
                    {
                        Pixel::F32_T * p = reinterpret_cast<Pixel::F32_T *>(data.data(i, j));
                        p[0] = static_cast<Pixel::F32_T>(pixel / static_cast<float>(input));
                        p[1] = 0.f;
                    }

                    for (j = 0; j < width; ++j)
                    {
                        Pixel::F32_T * p = reinterpret_cast<Pixel::F32_T *>(data.data(i, j));
                        //DJV_DEBUG_PRINT(p[0] << " = " << p[1]);
                    }
                    //DJV_DEBUG_PRINT("sum = " << sum);

                    //! \todo Why is it necessary to average the scale contributions?
                    //! Without this the values don't always add up to zero causing image
                    //! artifacts.
                    for (j = 0; j < width; ++j)
                    {
                        Pixel::F32_T * p = reinterpret_cast<Pixel::F32_T *>(data.data(i, j));
                        p[1] /= static_cast<Pixel::F32_T>(sum);
                        //DJV_DEBUG_PRINT(p[1]);
                    }
                }
            }

        } // namespace

        namespace
        {
            const QString sourceVertex =
                "#version 400 core\n"
                "\n"
                "layout (location = 0) in vec2 aPos;\n"
                "layout (location = 1) in vec2 aTexture;\n"
                "\n"
                "out vec2 TextureCoord;\n"
                "\n"
                "uniform struct Transform\n"
                "{\n"
                "    mat4 mvp;\n"
                "} transform;\n"
                "\n"
                "void main(void)\n"
                "{\n"
                "    gl_Position = transform.mvp * vec4(aPos, 1.0, 1.0);\n"
                "    TextureCoord = aTexture;\n"
                "}\n";

            const QString sourceGamma =
                "    if (tmp[0] >= 0.0)\n"
                "        tmp[0] = pow(tmp[0], data.gamma);\n"
                "    if (tmp[1] >= 0.0)\n"
                "        tmp[1] = pow(tmp[1], data.gamma);\n"
                "    if (tmp[2] >= 0.0)\n"
                "        tmp[2] = pow(tmp[2], data.gamma);\n";

            //! \todo How should we handle negative numbers when making color adjustments?
            //! Should they be clamped to zero?
            const QString sourceFragmentHeader =
                "#version 400 core\n"
                "\n"
                "struct Levels\n"
                "{\n"
                "    float in0, in1;\n"
                "    float gamma;\n"
                "    float out0, out1;\n"
                "};\n"
                "\n"
                "struct Exposure\n"
                "{\n"
                "    float v, d, k, f, g;\n"
                "};\n"
                "\n"
                "float knee(float value, float f)\n"
                "{\n"
                "    return log(value * f + 1.0) / f;\n"
                "}\n"
                "\n"
                "vec4 gamma(vec4 value, float gamma)\n"
                "{\n"
                "    if (value[0] >= 0.0)\n"
                "        value[0] = pow(value[0], gamma);\n"
                "    if (value[1] >= 0.0)\n"
                "        value[1] = pow(value[1], gamma);\n"
                "    if (value[2] >= 0.0)\n"
                "        value[2] = pow(value[2], gamma);\n"
                "    return value;\n"
                "}\n"
                "\n"
                "vec4 lut1(vec4 value, sampler2D lut)\n"
                "{\n"
                "    value[0] = texture(lut, vec2(value[0], 0))[0];\n"
                "    value[1] = texture(lut, vec2(value[1], 0))[0];\n"
                "    value[2] = texture(lut, vec2(value[2], 0))[0];\n"
                "    return value;\n"
                "}\n"
                "\n"
                "vec4 lut2(vec4 value, sampler2D lut)\n"
                "{\n"
                "    value[0] = texture(lut, vec2(value[0], 0))[0];\n"
                "    value[1] = texture(lut, vec2(value[1], 0))[1];\n"
                "    return value;\n"
                "}\n"
                "\n"
                "vec4 lut3(vec4 value, sampler2D lut)\n"
                "{\n"
                "    value[0] = texture(lut, vec2(value[0], 0))[0];\n"
                "    value[1] = texture(lut, vec2(value[1], 0))[1];\n"
                "    value[2] = texture(lut, vec2(value[2], 0))[2];\n"
                "    return value;\n"
                "}\n"
                "\n"
                "vec4 lut4(vec4 value, sampler2D lut)\n"
                "{\n"
                "    value[0] = texture(lut, vec2(value[0], 0))[0];\n"
                "    value[1] = texture(lut, vec2(value[1], 0))[1];\n"
                "    value[2] = texture(lut, vec2(value[2], 0))[2];\n"
                "    value[3] = texture(lut, vec2(value[2], 0))[3];\n"
                "    return value;\n"
                "}\n"
                "\n"
                "vec4 displayProfileColor(vec4 value, mat4 color)\n"
                "{\n"
                "    vec4 tmp;\n"
                "    tmp[0] = value[0];\n"
                "    tmp[1] = value[1];\n"
                "    tmp[2] = value[2];\n"
                "    tmp[3] = 1.0;\n"
                "    tmp *= color;\n"
                "    tmp[3] = value[3];\n"
                "    return tmp;\n"
                "}\n"
                "\n"
                "vec4 exposure(vec4 value, Exposure data)\n"
                "{\n"
                "    value[0] = max(0.0, value[0] - data.d) * data.v;\n"
                "    value[1] = max(0.0, value[1] - data.d) * data.v;\n"
                "    value[2] = max(0.0, value[2] - data.d) * data.v;\n"
                "    if (value[0] > data.k)\n"
                "        value[0] = data.k + knee(value[0] - data.k, data.f);\n"
                "    if (value[1] > data.k)\n"
                "        value[1] = data.k + knee(value[1] - data.k, data.f);\n"
                "    if (value[2] > data.k)\n"
                "        value[2] = data.k + knee(value[2] - data.k, data.f);\n"
                "    value[0] *= 0.332;\n"
                "    value[1] *= 0.332;\n"
                "    value[2] *= 0.332;\n"
                "    return value;\n"
                "}\n"
                "\n"
                "vec4 softClip(vec4 value, float softClip)\n"
                "{\n"
                "    float tmp = 1.0 - softClip;\n"
                "    if (value[0] > tmp)\n"
                "        value[0] = tmp + (1.0 - exp(-(value[0] - tmp) / softClip)) * softClip;\n"
                "    if (value[1] > tmp)\n"
                "        value[1] = tmp + (1.0 - exp(-(value[1] - tmp) / softClip)) * softClip;\n"
                "    if (value[2] > tmp)\n"
                "        value[2] = tmp + (1.0 - exp(-(value[2] - tmp) / softClip)) * softClip;\n"
                "    return value;\n"
                "}\n"
                "\n";

            const QString sourceFragmentLevels =
                "vec4 levels(vec4 value, Levels data)\n"
                "{\n"
                "    vec4 tmp;\n"
                "    tmp[0] = (value[0] - data.in0) / data.in1;\n"
                "    tmp[1] = (value[1] - data.in0) / data.in1;\n"
                "    tmp[2] = (value[2] - data.in0) / data.in1;\n"
                "%1"
                "    value[0] = tmp[0] * data.out1 + data.out0;\n"
                "    value[1] = tmp[1] * data.out1 + data.out0;\n"
                "    value[2] = tmp[2] * data.out1 + data.out0;\n"
                "    return value;\n"
                "}\n";

            const QString sourceFragmentScaleX =
                "vec4 scaleX()\n"
                "{\n"
                "    vec4 value = vec4(0.0);\n"
                "    for (int i = 0; i < %1; ++i)\n"
                "    {\n"
                "        float t = float(i) / float(%2 - 1);\n"
                "        vec4 tmp = texture(inScaleContrib, vec2(TextureCoord.s, t));\n"
                "        vec2 position = vec2(tmp[0], TextureCoord.t);\n"
                "        value += tmp[1] * %3;\n"
                "    }\n"
                "    return value;\n"
                "}\n"
                "\n";

            const QString sourceFragmentScaleY =
                "vec4 scaleY()\n"
                "{\n"
                "    vec4 value = vec4(0.0);\n"
                "    for (int i = 0; i < %1; ++i)\n"
                "    {\n"
                "        float t = float(i) / float(%2 - 1);\n"
                "        vec4 tmp = texture(inScaleContrib, vec2(TextureCoord.t, t));\n"
                "        vec2 position = vec2(TextureCoord.s, tmp[0]);\n"
                "        value += tmp[1] * texture(inTexture, position);\n"
                "    }\n"
                "    return value;\n"
                "}\n"
                "\n";

            const QString sourceFragmentMain =
                "void main(void)\n"
                "{\n"
                "    vec4 color;\n"
                "%1"
                "    FragColor = color;\n"
                "}\n";

        } // namespace

        namespace
        {
            QString sourceFragment(
                ColorProfile                      colorProfile,
                const OpenGLImageDisplayProfile & displayProfile,
                OpenGLImageOptions::CHANNEL       channel,
                bool                              multipassFilter,
                int                               scaleSize,
                bool                              scaleX)
            {
                //DJV_DEBUG("sourceFragment");
                //DJV_DEBUG_PRINT("colorProfile = " << colorProfile);
                //DJV_DEBUG_PRINT("displayProfile = " << displayProfile);
                //DJV_DEBUG_PRINT("channel = " << channel);
                //DJV_DEBUG_PRINT("multipass filter = " << multipassFilter);
                //DJV_DEBUG_PRINT("scale size = " << scaleSize);
                //DJV_DEBUG_PRINT("scale x = " << scaleX);

                QString header;
                QString main;

                // Initialize the header.
                header = sourceFragmentHeader;
                header += "in vec2 TextureCoord;\n";
                header += "out vec4 FragColor;\n";
                header += "uniform sampler2D inTexture;\n";

                // Color profile.
                QString sample;
                switch (colorProfile.type)
                {
                case ColorProfile::LUT:
                    header += "uniform sampler2D inColorProfileLut;\n";
                    switch (colorProfile.lut.channels())
                    {
                    case 1: sample = "lut1(texture(inTexture, TextureCoord), inColorProfileLut)"; break;
                    case 2: sample = "lut2(texture(inTexture, TextureCoord), inColorProfileLut)"; break;
                    case 3: sample = "lut3(texture(inTexture, TextureCoord), inColorProfileLut)"; break;
                    case 4: sample = "lut4(texture(inTexture, TextureCoord), inColorProfileLut)"; break;
                    }
                    break;
                case ColorProfile::GAMMA:
                    header += "uniform float inColorProfileGamma;\n";
                    sample = "gamma(texture(inTexture, TextureCoord), inColorProfileGamma)";
                    break;
                case ColorProfile::EXPOSURE:
                    header += "uniform Exposure inColorProfileExposure;\n";
                    sample = "exposure(texture(inTexture, TextureCoord), inColorProfileExposure)";
                    break;
                default:
                    sample = "texture(inTexture, TextureCoord)";
                    break;
                }

                // Image filter.
                if (!multipassFilter)
                {
                    main += QString("color = %1;\n").arg(sample);
                }
                else
                {
                    header += "uniform sampler2D inScaleContrib;\n";
                    if (scaleX)
                    {
                        header += QString(sourceFragmentScaleX).
                            arg(scaleSize).
                            arg(scaleSize).
                            arg(sample);
                        main += "color = scaleX();\n";
                    }
                    else
                    {
                        header += QString(sourceFragmentScaleY).
                            arg(scaleSize).
                            arg(scaleSize);
                        main += "color = scaleY();\n";
                    }
                }

                // Display profile.
                if (displayProfile.lut.isValid())
                {
                    header += "uniform sampler2D inDisplayProfileLut;\n";
                    switch (displayProfile.lut.channels())
                    {
                    case 1: main += "color = lut1(color, inDisplayProfileLut);\n"; break;
                    case 2: main += "color = lut2(color, inDisplayProfileLut);\n"; break;
                    case 3: main += "color = lut3(color, inDisplayProfileLut);\n"; break;
                    case 4: main += "color = lut4(color, inDisplayProfileLut);\n"; break;
                    }
                }
                if (displayProfile.color != OpenGLImageDisplayProfile().color)
                {
                    header += "uniform mat4 inDisplayProfileColor;\n";
                    main += "color = displayProfileColor(color, inDisplayProfileColor);\n";
                }
                if (displayProfile.levels != OpenGLImageDisplayProfile().levels)
                {
                    header += sourceFragmentLevels.
                        arg(!Core::Math::fuzzyCompare(displayProfile.levels.gamma, 1.f) ?
                            sourceGamma : "");
                    header += "uniform Levels inDisplayProfileLevels;\n";
                    main += "color = levels(color, inDisplayProfileLevels);\n";
                }
                if (displayProfile.softClip != OpenGLImageDisplayProfile().softClip)
                {
                    header += "uniform float inDisplayProfileSoftClip;\n";
                    main += "color = softClip(color, inDisplayProfileSoftClip);\n";
                }

                // Image channel.
                if (channel)
                {
                    main += QString("color = vec4(color[%1]);\n").arg(channel - 1);
                }

                // Clamp pixel values.
                //if (clamp)
                //    main += "color = clamp(color, vec4(0.f), vec4(1.f));\n";

                QString out = header + "\n" + QString(sourceFragmentMain).arg(main);
                //int line = 0;
                //Q_FOREACH(QString s, out.split('\n'))
                //{
                //    DJV_DEBUG_PRINT(line << ": " << s);
                //    ++line;
                //}
                return out;
            }

        } // namespace

        namespace
        {
            float knee(float x, float f)
            {
                return Core::Math::log(x * f + 1.f) / f;
            }

            float knee2(float x, float y)
            {
                float f0 = 0.f, f1 = 1.f;
                while (knee(x, f1) > y)
                {
                    f0 = f1;
                    f1 = f1 * 2.f;
                }
                for (int i = 0; i < 30; ++i)
                {
                    const float f2 = (f0 + f1) / 2.f;
                    if (knee(x, f2) < y)
                    {
                        f1 = f2;
                    }
                    else
                    {
                        f0 = f2;
                    }
                }
                return (f0 + f1) / 2.f;
            }

            void colorProfileInit(
                const OpenGLImageOptions & options,
                OpenGLShader &             shader,
                OpenGLLUT &                colorProfile)
            {
                //DJV_DEBUG("colorProfileInit");
                //DJV_DEBUG_PRINT("type = " << options.colorProfile.type);    
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
                switch (options.colorProfile.type)
                {
                case ColorProfile::LUT:
                {
                    glFuncs->glActiveTexture(GL_TEXTURE2);
                    shader.setUniform("inColorProfileLut", 2);
                    colorProfile.init(options.colorProfile.lut);
                }
                break;
                case ColorProfile::GAMMA:
                {
                    shader.setUniform("inColorProfileGamma", 1.f / options.colorProfile.gamma);
                }
                break;
                case ColorProfile::EXPOSURE:
                {
                    struct Exposure
                    {
                        float v, d, k, f;
                    };
                    Exposure exposure;
                    exposure.v = Core::Math::pow(
                        2.f,
                        options.colorProfile.exposure.value + 2.47393f);
                    exposure.d = options.colorProfile.exposure.defog;
                    exposure.k = Core::Math::pow(
                        2.f,
                        options.colorProfile.exposure.kneeLow);
                    exposure.f = knee2(
                        Core::Math::pow(2.f, options.colorProfile.exposure.kneeHigh) -
                        exposure.k,
                        Core::Math::pow(2.f, 3.5f) - exposure.k);
                    //DJV_DEBUG_PRINT("exposure");
                    //DJV_DEBUG_PRINT("  v = " << exposure.v);
                    //DJV_DEBUG_PRINT("  d = " << exposure.d);
                    //DJV_DEBUG_PRINT("  k = " << exposure.k);
                    //DJV_DEBUG_PRINT("  f = " << exposure.f);
                    shader.setUniform("inColorProfileExposure.v", exposure.v);
                    shader.setUniform("inColorProfileExposure.d", exposure.d);
                    shader.setUniform("inColorProfileExposure.k", exposure.k);
                    shader.setUniform("inColorProfileExposure.f", exposure.f);
                }
                break;
                default: break;
                }
            }

        } // namespace

        namespace
        {
            void displayProfileInit(
                const OpenGLImageOptions & options,
                OpenGLShader&              shader,
                OpenGLLUT &                displayProfile)
            {
                //DJV_DEBUG("displayProfileInit");

                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

                // Color matrix.
                shader.setUniform(
                    "inDisplayProfileColor",
                    OpenGLImageColor::colorMatrix(options.displayProfile.color));

                // Levels in.
                shader.setUniform(
                    "inDisplayProfileLevels.in0",
                    options.displayProfile.levels.inLow);
                shader.setUniform(
                    "inDisplayProfileLevels.in1",
                    options.displayProfile.levels.inHigh - options.displayProfile.levels.inLow);

                // Gamma.
                shader.setUniform(
                    "inDisplayProfileLevels.gamma",
                    1.f / options.displayProfile.levels.gamma);

                // Levels out.
                shader.setUniform(
                    "inDisplayProfileLevels.out0",
                    options.displayProfile.levels.outLow);
                shader.setUniform(
                    "inDisplayProfileLevels.out1",
                    options.displayProfile.levels.outHigh - options.displayProfile.levels.outLow);

                // Soft-clip.
                shader.setUniform(
                    "inDisplayProfileSoftClip",
                    options.displayProfile.softClip);

                // Lookup table.
                if (options.displayProfile.lut.isValid())
                {
                    glFuncs->glActiveTexture(GL_TEXTURE3);
                    shader.setUniform("inDisplayProfileLut", 3);
                    displayProfile.init(options.displayProfile.lut);
                }
            }

        } // namespace

        void OpenGLImage::draw(
            const PixelData &          data,
            const glm::mat4x4&         viewMatrix,
            const OpenGLImageOptions & options)
        {
            //DJV_DEBUG("OpenGLImage::draw");
            //DJV_DEBUG_PRINT("data = " << data);
            //DJV_DEBUG_PRINT("color profile = " << options.colorProfile);

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

            const PixelDataInfo & info = data.info();
            const int proxyScale =
                options.proxyScale ?
                PixelDataUtil::proxyScale(info.proxy) :
                1;
            const glm::ivec2 scale(
                Core::Math::ceil(options.xform.scale.x * info.size.x * proxyScale),
                Core::Math::ceil(options.xform.scale.y * info.size.y * proxyScale));
            const glm::ivec2 scaleTmp(scale.x, data.h());
            //DJV_DEBUG_PRINT("scale = " << scale);
            //DJV_DEBUG_PRINT("scale tmp = " << scaleTmp);

            // Initialize.
            const OpenGLImageFilter::FILTER filter =
                info.size == scale ? OpenGLImageFilter::NEAREST :
                (scale.x * scale.y < info.size.x * info.size.y ?
                    options.filter.min : options.filter.mag);
            //DJV_DEBUG_PRINT("filter min = " << options.filter.min);
            //DJV_DEBUG_PRINT("filter mag = " << options.filter.mag);
            //DJV_DEBUG_PRINT("filter = " << filter);

            if (!_init || _info != info || _options != options)
            {
                _init = true;
                _info = info;
                _options = options;
                switch (filter)
                {
                case OpenGLImageFilter::NEAREST:
                case OpenGLImageFilter::LINEAR:
                {
                    //DJV_DEBUG_PRINT("init single pass");
                    _texture->init(
                        data.info(),
                        GL_TEXTURE_2D,
                        OpenGLImageFilter::toGl(filter),
                        OpenGLImageFilter::toGl(filter));
                    _shader->init(
                        sourceVertex,
                        sourceFragment(
                            options.colorProfile,
                            options.displayProfile,
                            options.channel,
                            false,
                            0,
                            false));
                }
                break;
                case OpenGLImageFilter::BOX:
                case OpenGLImageFilter::TRIANGLE:
                case OpenGLImageFilter::BELL:
                case OpenGLImageFilter::BSPLINE:
                case OpenGLImageFilter::LANCZOS3:
                case OpenGLImageFilter::CUBIC:
                case OpenGLImageFilter::MITCHELL:
                {
                    //DJV_DEBUG_PRINT("init two pass");
                    _texture->init(data.info(), GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);

                    // Initialize horizontal pass.
                    PixelData contrib;
                    scaleContrib(data.w(), scale.x, filter, contrib);
                    _scaleXContrib->init(contrib, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);
                    _scaleXShader->init(
                        sourceVertex,
                        sourceFragment(
                            options.colorProfile,
                            OpenGLImageDisplayProfile(),
                            static_cast<OpenGLImageOptions::CHANNEL>(0),
                            true,
                            contrib.h(),
                            true));

                    // Initialize vertical pass.
                    scaleContrib(data.h(), scale.y, filter, contrib);
                    _scaleYContrib->init(contrib, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);
                    _scaleYShader->init(
                        sourceVertex,
                        sourceFragment(
                            ColorProfile(),
                            options.displayProfile,
                            options.channel,
                            true,
                            contrib.h(),
                            false));
                }
                break;
                default: break;
                }
            }

            // Render.
            const PixelDataInfo::Mirror mirror(
                info.mirror.x ? (!options.xform.mirror.x) : options.xform.mirror.x,
                info.mirror.y ? (!options.xform.mirror.y) : options.xform.mirror.y);
            //DJV_DEBUG_PRINT("mirror = " << mirror.x << " " << mirror.y);
            switch (filter)
            {
            case OpenGLImageFilter::NEAREST:
            case OpenGLImageFilter::LINEAR:
            {
                //DJV_DEBUG_PRINT("draw single pass");

                _shader->bind();

                // Initialize color and display profiles.
                colorProfileInit(options, *_shader, *_lutColorProfile);
                displayProfileInit(options, *_shader, *_lutDisplayProfile);

                // Draw.
                glFuncs->glActiveTexture(GL_TEXTURE0);
                _shader->setUniform("inTexture", 0);
                _texture->copy(data);
                _shader->setUniform("transform.mvp", viewMatrix * OpenGLImageXform::xformMatrix(options.xform));
                _mesh->setSize(info.size, mirror, proxyScale);
                _mesh->draw();
            }
            break;
            case OpenGLImageFilter::BOX:
            case OpenGLImageFilter::TRIANGLE:
            case OpenGLImageFilter::BELL:
            case OpenGLImageFilter::BSPLINE:
            case OpenGLImageFilter::LANCZOS3:
            case OpenGLImageFilter::CUBIC:
            case OpenGLImageFilter::MITCHELL:
            {
                //DJV_DEBUG_PRINT("draw two pass");

                // Horizontal pass.
                OpenGLOffscreenBuffer buffer(PixelDataInfo(scaleTmp, data.pixel()));
                {
                    GLint vp[4] = { 0, 0, 0, 0 };
                    glFuncs->glGetIntegerv(GL_VIEWPORT, vp);
                    glFuncs->glViewport(0, 0, scaleTmp.x, scaleTmp.y);

                    OpenGLOffscreenBufferScope bufferScope(&buffer);
                    _scaleXShader->bind();
                    colorProfileInit(options, *_scaleXShader, *_lutColorProfile);
                    glFuncs->glActiveTexture(GL_TEXTURE0);
                    _scaleXShader->setUniform("inTexture", 0);
                    _texture->copy(data);
                    _texture->bind();
                    glFuncs->glActiveTexture(GL_TEXTURE1);
                    _scaleXShader->setUniform("inScaleContrib", 1);
                    _scaleXContrib->bind();
                    auto m = glm::ortho(
                        0.f,
                        static_cast<float>(scaleTmp.x),
                        0.f,
                        static_cast<float>(scaleTmp.y),
                        -1.f,
                        1.f);
                    _scaleXShader->setUniform("transform.mvp", m);
                    _mesh->setSize(scaleTmp, mirror);
                    _mesh->draw();

                    glFuncs->glViewport(vp[0], vp[1], vp[2], vp[3]);
                }

                // Vertical pass.
                _scaleYShader->bind();
                displayProfileInit(options, *_scaleYShader, *_lutDisplayProfile);
                glFuncs->glActiveTexture(GL_TEXTURE0);
                _scaleYShader->setUniform("inTexture", 0);
                glBindTexture(GL_TEXTURE_2D, buffer.texture());
                glFuncs->glActiveTexture(GL_TEXTURE1);
                _scaleYShader->setUniform("inScaleContrib", 1);
                _scaleYContrib->bind();
                OpenGLImageXform xform = options.xform;
                xform.scale = glm::vec2(1.f, 1.f);
                _scaleYShader->setUniform("transform.mvp", viewMatrix * OpenGLImageXform::xformMatrix(xform));
                _mesh->setSize(scale);
                _mesh->draw();
            }
            break;
            default: break;
            }
        }

    } // namespace Graphics
} // namespace djv
