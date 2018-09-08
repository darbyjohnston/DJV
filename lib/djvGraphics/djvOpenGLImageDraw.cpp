//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvOpenGLImage.h>

#include <djvOpenGLLUT.h>
#include <djvOpenGLOffscreenBuffer.h>
#include <djvOpenGLShader.h>
#include <djvOpenGLTexture.h>
#include <djvPixelDataUtil.h>

#include <djvDebug.h>
#include <djvError.h>
#include <djvVector.h>

#include <QCoreApplication>

using namespace gl;

//------------------------------------------------------------------------------
// djvOpenGLImageState
//------------------------------------------------------------------------------

djvOpenGLImageState::djvOpenGLImageState() :
    _init             (false),
    _texture          (new djvOpenGLTexture),
    _shader           (new djvOpenGLShader),
    _scaleXContrib    (new djvOpenGLTexture),
    _scaleYContrib    (new djvOpenGLTexture),
    _scaleXShader     (new djvOpenGLShader),
    _scaleYShader     (new djvOpenGLShader),
    _lutColorProfile  (new djvOpenGLLUT),
    _lutDisplayProfile(new djvOpenGLLUT)
{}

djvOpenGLImageState::~djvOpenGLImageState()
{
    delete _lutDisplayProfile;
    delete _lutColorProfile;
    delete _scaleXShader;
    delete _scaleYShader;
    delete _scaleXContrib;
    delete _scaleYContrib;
    delete _shader;
    delete _texture;
}

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

namespace
{

void activeTexture(GLenum in)
{
    //DJV_DEBUG("activeTexture");
    DJV_DEBUG_OPEN_GL(glActiveTexture(in));
}

void uniform1i(GLuint program, const QString & name, int value)
{
    glUniform1i(
        glGetUniformLocation(program, name.toLatin1().data()),
        static_cast<GLint>(value));
}

void uniform1f(GLuint program, const QString & name, float value)
{
    glUniform1f(
        glGetUniformLocation(program, name.toLatin1().data()),
        static_cast<GLfloat>(value));
}

void uniformMatrix4f(
    GLuint              program,
    const QString &     name,
    const glm::mat4x4 & value)
{
    glUniformMatrix4fv(
        glGetUniformLocation(program, name.toLatin1().data()),
        1,
        false,
        &value[0][0]);
}

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
    x *= djvMath::pi;
    if (x != 0.f)
    {
        return djvMath::sin(x) / x;
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

FilterFnc * filterFnc(djvOpenGLImageFilter::FILTER in)
{
    static FilterFnc * tmp [] =
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

static float filterSupport(djvOpenGLImageFilter::FILTER in)
{
    static const float tmp [] =
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
    return djvMath::clamp(in, 0, size - 1);
}

void scaleContrib(
    int                          input,
    int                          output,
    djvOpenGLImageFilter::FILTER filter,
    djvPixelData &               data)
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
    const int width = djvMath::ceil(radius * 2.f + 1.f);
    //DJV_DEBUG_PRINT("width = " << width);
    data.set(djvPixelDataInfo(output, width, djvPixel::LA_F32));

    // Work.
    for (int i = 0; i < output; ++i)
    {
        const float center = i / scale;
        const int    left   = djvMath::ceil (center - radius);
        const int    right  = djvMath::floor(center + radius);
        //DJV_DEBUG_PRINT(i << " = " << left << " " << center << " " << right);

        float sum   = 0.f;
        int   pixel = 0;
        int j = 0;
        for (int k = left; j < width && k <= right; ++j, ++k)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<djvPixel::F32_T *>(data.data(i, j));
            pixel = edge(k, input);
            const float x = (center - k) * (scale < 1.f ? scale : 1.f);
            const float w = (scale < 1.f) ? ((*fnc)(x) * scale) : (*fnc)(x);
            //DJV_DEBUG_PRINT("w = " << w);
            p[0] = static_cast<djvPixel::F32_T>(pixel / static_cast<float>(input));
            p[1] = static_cast<djvPixel::F32_T>(w);
            sum += w;
        }

        for (; j < width; ++j)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<djvPixel::F32_T *>(data.data(i, j));
            p[0] = static_cast<djvPixel::F32_T>(pixel / static_cast<float>(input));
            p[1] = 0.f;
        }

        /*for (j = 0; j < width; ++j)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<Pixel::F32_T *>(data.data(i, j));
            //DJV_DEBUG_PRINT(p[0] << " = " << p[1]);
        }
        //DJV_DEBUG_PRINT("sum = " << sum);*/

        //! \todo Why is it necessary to average the scale contributions?
        //! Without this the values don't always add up to zero causing image
        //! artifacts.
        for (j = 0; j < width; ++j)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<djvPixel::F32_T *>(data.data(i, j));
            p[1] /= static_cast<djvPixel::F32_T>(sum);
        }
    }
}

void quad(
    const glm::ivec2 &               size,
    const djvPixelDataInfo::Mirror & mirror     = djvPixelDataInfo::Mirror(),
    int                              proxyScale = 1)
{
    //DJV_DEBUG("quad");
    float u [] = { 0.f, 0.f };
    float v [] = { 0.f, 0.f };
    u[! mirror.x] = 1.f;
    v[! mirror.y] = 1.f;
    //DJV_DEBUG_PRINT("u = " << u[0] << " " << u[1]);
    //DJV_DEBUG_PRINT("v = " << v[0] << " " << v[1]);
    const glm::vec2 uv[] =
    {
        glm::vec2(u[0], v[0]),
        glm::vec2(u[0], v[1]),
        glm::vec2(u[1], v[1]),
        glm::vec2(u[1], v[0])
    };
    glBegin(GL_QUADS);
    djvOpenGLUtil::drawBox(size * proxyScale, uv);
    glEnd();
}

} // namespace

//------------------------------------------------------------------------------
// Shader Source
//------------------------------------------------------------------------------

namespace
{
const QString sourceVertex =
    "void main(void)\n"
    "{\n"
    "    gl_FrontColor  = gl_Color;\n"
    "    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
    "    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
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
    "vec4 lut(vec4 value, sampler1D lut)\n"
    "{\n"
    "    value[0] = texture1D(lut, value[0])[0];\n"
    "    value[1] = texture1D(lut, value[1])[1];\n"
    "    value[2] = texture1D(lut, value[2])[2];\n"
    "\n"
    "    return value;\n"
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
    "\n"
    "    return value;\n"
    "}\n"
    "\n"
    "vec4 displayProfileColor(vec4 value, mat4 color)\n"
    "{\n"
    "    vec4 tmp;\n"
    "\n"
    "    tmp[0] = value[0];\n"
    "    tmp[1] = value[1];\n"
    "    tmp[2] = value[2];\n"
    "    tmp[3] = 1.0;\n"
    "\n"
    "    tmp *= color;\n"
    "\n"
    "    tmp[3] = value[3];\n"
    "\n"
    "    return tmp;\n"
    "}\n"
    "\n"
    "vec4 levels(vec4 value, Levels data)\n"
    "{\n"
    "    vec4 tmp;\n"
    "\n"
    "    tmp[0] = (value[0] - data.in0) / data.in1;\n"
    "    tmp[1] = (value[1] - data.in0) / data.in1;\n"
    "    tmp[2] = (value[2] - data.in0) / data.in1;\n"
    "\n"
    "%1"
    "\n"
    "    value[0] = tmp[0] * data.out1 + data.out0;\n"
    "    value[1] = tmp[1] * data.out1 + data.out0;\n"
    "    value[2] = tmp[2] * data.out1 + data.out0;\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n"
    "vec4 exposure(vec4 value, Exposure data)\n"
    "{\n"
    "    value[0] = max(0.0, value[0] - data.d) * data.v;\n"
    "    value[1] = max(0.0, value[1] - data.d) * data.v;\n"
    "    value[2] = max(0.0, value[2] - data.d) * data.v;\n"
    "\n"
    "    if (value[0] > data.k)\n"
    "        value[0] = data.k + knee(value[0] - data.k, data.f);\n"
    "    if (value[1] > data.k)\n"
    "        value[1] = data.k + knee(value[1] - data.k, data.f);\n"
    "    if (value[2] > data.k)\n"
    "        value[2] = data.k + knee(value[2] - data.k, data.f);\n"
    "\n"
    "    value[0] *= 0.332;\n"
    "    value[1] *= 0.332;\n"
    "    value[2] *= 0.332;\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n"
    "vec4 softClip(vec4 value, float softClip)\n"
    "{\n"
    "    float tmp = 1.0 - softClip;\n"
    "\n"
    "    if (value[0] > tmp)\n"
    "        value[0] = tmp + (1.0 - exp(-(value[0] - tmp) / softClip)) *\n"
    "            softClip;\n"
    "\n"
    "    if (value[1] > tmp)\n"
    "        value[1] = tmp + (1.0 - exp(-(value[1] - tmp) / softClip)) *\n"
    "            softClip;\n"
    "\n"
    "    if (value[2] > tmp)\n"
    "        value[2] = tmp + (1.0 - exp(-(value[2] - tmp) / softClip)) *\n"
    "            softClip;\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n";

const QString sourceFragmentScaleX =
    "vec4 scaleX(sampler2D texture, sampler2D contrib)\n"
    "{\n"
    "    vec4 value = vec4(0.0);\n"
    "\n"
    "    for (int i = 0; i < %1; ++i)\n"
    "    {\n"
    "        float t = float(i) / float(%2 - 1);\n"
    "\n"
    "        vec4 tmp = texture2D(contrib, vec2(gl_TexCoord[0].s, t));\n"
    "\n"
    "        vec2 position = vec2(tmp[0], gl_TexCoord[0].t);\n"
    "\n"
    "        value += tmp[3] * %3;\n"
    "  }\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n";

const QString sourceFragmentScaleY =
    "vec4 scaleY(sampler2D texture, sampler2D contrib)\n"
    "{\n"
    "    vec4 value = vec4(0.0);\n"
    "\n"
    "    for (int i = 0; i < %1; ++i)\n"
    "    {\n"
    "        float t = float(i) / float(%2 - 1);\n"
    "\n"
    "        vec4 tmp = texture2D(contrib, vec2(gl_TexCoord[0].t, t));\n"
    "\n"
    "        vec2 position = vec2(gl_TexCoord[0].s, tmp[0]);\n"
    "\n"
    "        value += tmp[3] * texture2D(texture, position);\n"
    "    }\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n";

const QString sourceFragmentMain =
    "void main(void)\n"
    "{\n"
    "    vec4 color;\n"
    "\n"
    "%1"
    "\n"
    "    gl_FragColor = color;\n"
    "}\n";

} // namespace

//------------------------------------------------------------------------------
// sourceFragment
//
// Generate the fragment shader source code.
//------------------------------------------------------------------------------

namespace
{
/*QString sourceFragment()
{
    QString header;
    QString main;
    header = "uniform sampler2D inTexture;\n";
    main +=
        "vec2 position = gl_TexCoord[0].st;\n";
    main +=
        "color = texture2D(inTexture, position);\n";
    return
        header + "\n" +
        QString(sourceFragmentMain).arg(main);
}*/

QString sourceFragment(
    djvColorProfile::PROFILE             colorProfile,
    const djvOpenGLImageDisplayProfile & displayProfile,
    djvOpenGLImageOptions::CHANNEL       channel,
    bool                                 multipassFilter,
    int                                  scaleSize,
    bool                                 scaleX)
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
    header = sourceFragmentHeader.
        arg(! djvMath::fuzzyCompare(displayProfile.levels.gamma, 1.f) ?
            sourceGamma : "");
    header += "uniform sampler2D inTexture;\n";

    // Color profile.
    QString sample;
    switch (colorProfile)
    {
        case djvColorProfile::LUT:
            header += "uniform sampler1D inColorProfileLut;\n";
            sample =
                "lut(\n"
                "    texture2D(inTexture, position),\n"
                "    inColorProfileLut)";
            break;
        case djvColorProfile::GAMMA:
            header += "uniform float inColorProfileGamma;\n";
            sample =
                "gamma(\n"
                "    texture2D(inTexture, position),\n"
                "    inColorProfileGamma)";
            break;
        case djvColorProfile::EXPOSURE:
            header += "uniform Exposure inColorProfileExposure;\n";
            sample =
                "exposure(\n"
                "    texture2D(inTexture, position),\n"
                "    inColorProfileExposure)";
            break;
        default:
            sample = "texture2D(inTexture, position)";
            break;
    }

    // Image filter.
    if (! multipassFilter)
    {
        main += "vec2 position = gl_TexCoord[0].st;\n";
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
            main += "color = scaleX(inTexture, inScaleContrib);\n";
        }
        else
        {
            header += QString(sourceFragmentScaleY).
                arg(scaleSize).
                arg(scaleSize);
            main += "color = scaleY(inTexture, inScaleContrib);\n";
        }
    }

    // Display profile.
    if (displayProfile.lut.isValid())
    {
        header += "uniform sampler1D inDisplayProfileLut;\n";
        main += "color = lut(color, inDisplayProfileLut);\n";
    }
    if (displayProfile.color != djvOpenGLImageDisplayProfile().color)
    {
        header += "uniform mat4 inDisplayProfileColor;\n";
        main += "color = displayProfileColor(color, inDisplayProfileColor);\n";
    }
    if (displayProfile.levels != djvOpenGLImageDisplayProfile().levels)
    {
        header += "uniform Levels inDisplayProfileLevels;\n";
        main += "color = levels(color, inDisplayProfileLevels);\n";
    }
    if (displayProfile.softClip != djvOpenGLImageDisplayProfile().softClip)
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

    return
        header + "\n" +
        QString(sourceFragmentMain).arg(main);
}

} // namespace

//------------------------------------------------------------------------------
// colorProfileInit
//
// This function initializes the color profile.
//------------------------------------------------------------------------------

namespace
{

float knee(float x, float f)
{
    return djvMath::log(x * f + 1.f) / f;
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
    const djvOpenGLImageOptions & options,
    GLuint                        program,
    djvOpenGLLUT &                colorProfile)
{
    //DJV_DEBUG("colorProfileInit");
    //DJV_DEBUG_PRINT("type = " << options.colorProfile.type);
    
    switch (options.colorProfile.type)
    {
        case djvColorProfile::LUT:
        {
            activeTexture(GL_TEXTURE2);
            uniform1i(program, "inColorProfileLut", 2);
            colorProfile.init(options.colorProfile.lut);
        }
        break;
        case djvColorProfile::GAMMA:
        {
            uniform1f(
                program,
                "inColorProfileGamma",
                1.f / options.colorProfile.gamma);
        }
        break;
        case djvColorProfile::EXPOSURE:
        {
            struct Exposure
            {
                float v, d, k, f;
            };
            Exposure exposure;
            exposure.v = djvMath::pow(
                2.f,
                options.colorProfile.exposure.value + 2.47393);
            exposure.d = options.colorProfile.exposure.defog;
            exposure.k = djvMath::pow(
                2.f,
                options.colorProfile.exposure.kneeLow);
            exposure.f = knee2(
                djvMath::pow(2.f, options.colorProfile.exposure.kneeHigh) -
                exposure.k,
                djvMath::pow(2.f, 3.5f) - exposure.k);
            //DJV_DEBUG_PRINT("exposure");
            //DJV_DEBUG_PRINT("  v = " << exposure.v);
            //DJV_DEBUG_PRINT("  d = " << exposure.d);
            //DJV_DEBUG_PRINT("  k = " << exposure.k);
            //DJV_DEBUG_PRINT("  f = " << exposure.f);
            uniform1f(program, "inColorProfileExposure.v", exposure.v);
            uniform1f(program, "inColorProfileExposure.d", exposure.d);
            uniform1f(program, "inColorProfileExposure.k", exposure.k);
            uniform1f(program, "inColorProfileExposure.f", exposure.f);
        }
        break;
        default: break;
    }
}

} // namespace

//------------------------------------------------------------------------------
// displayProfileInit
//
// This function initializes the display profile.
//------------------------------------------------------------------------------

namespace
{
void displayProfileInit(
    const djvOpenGLImageOptions & options,
    GLuint                        program,
    djvOpenGLLUT &                displayProfile)
{
    //DJV_DEBUG("displayProfileInit");

    // Color matrix.
    uniformMatrix4f(
        program,
        "inDisplayProfileColor",
        djvOpenGLImageColor::colorMatrix(options.displayProfile.color));

    // Levels in.
    uniform1f(
        program,
        "inDisplayProfileLevels.in0",
        options.displayProfile.levels.inLow);
    uniform1f(
        program,
        "inDisplayProfileLevels.in1",
        options.displayProfile.levels.inHigh -
            options.displayProfile.levels.inLow);

    // Gamma.
    uniform1f(
        program,
        "inDisplayProfileLevels.gamma",
        1.f / options.displayProfile.levels.gamma);

    // Levels out.
    uniform1f(
        program,
        "inDisplayProfileLevels.out0",
        options.displayProfile.levels.outLow);
    uniform1f(
        program,
        "inDisplayProfileLevels.out1",
        options.displayProfile.levels.outHigh -
            options.displayProfile.levels.outLow);

    // Soft-clip.
    uniform1f(
        program,
        "inDisplayProfileSoftClip",
        options.displayProfile.softClip);

    // Lookup table.
    if (options.displayProfile.lut.isValid())
    {
        activeTexture(GL_TEXTURE3);
        uniform1i(program, "inDisplayProfileLut", 3);
        displayProfile.init(options.displayProfile.lut);
    }
}

} // namespace

//------------------------------------------------------------------------------
// djvOpenGLImage::draw
//------------------------------------------------------------------------------

namespace
{
struct RestoreState
{
    ~RestoreState()
    {
        glUseProgram(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

} // namespace

void djvOpenGLImage::draw(
    const djvPixelData &          data,
    const djvOpenGLImageOptions & options,
    djvOpenGLImageState *         state) throw (djvError)
{
    //DJV_DEBUG("djvOpenGLImage::draw");
    //DJV_DEBUG_PRINT("data = " << data);
    //DJV_DEBUG_PRINT("color profile = " << options.colorProfile);
    
    RestoreState restoreState;
    djvOpenGLImageState defaultState;
    if (! state)
    {
        state = &defaultState;
    }
    const djvPixelDataInfo & info = data.info();
    const int proxyScale =
        options.proxyScale ?
        djvPixelDataUtil::proxyScale(info.proxy) :
        1;
    const glm::ivec2 scale(
        djvMath::ceil(options.xform.scale.x * info.size.x * proxyScale),
        djvMath::ceil(options.xform.scale.y * info.size.y * proxyScale));
    const glm::ivec2 scaleTmp(scale.x, data.h());
    //DJV_DEBUG_PRINT("scale = " << scale);
    //DJV_DEBUG_PRINT("scale tmp = " << scaleTmp);

    // Initialize.
    const djvOpenGLImageFilter::FILTER filter =
        info.size == scale ? djvOpenGLImageFilter::NEAREST :
        (scale.x * scale.y < info.size.x * info.size.y ?
         options.filter.min : options.filter.mag);
    //DJV_DEBUG_PRINT("filter min = " << options.filter.min);
    //DJV_DEBUG_PRINT("filter mag = " << options.filter.mag);
    //DJV_DEBUG_PRINT("filter = " << filter);

    if (! state->_init || state->_info != info || state->_options != options)
    {
        switch (filter)
        {
            case djvOpenGLImageFilter::NEAREST:
            case djvOpenGLImageFilter::LINEAR:
            {
                //DJV_DEBUG_PRINT("init single pass");
                state->_texture->init(
                    data.info(),
                    GL_TEXTURE_2D,
                    djvOpenGLImageFilter::toGl(filter),
                    djvOpenGLImageFilter::toGl(filter));
                state->_shader->init(
                    sourceVertex,
                    sourceFragment(
                        options.colorProfile.type,
                        options.displayProfile,
                        options.channel,
                        false,
                        0,
                        false));
            }
            break;
            case djvOpenGLImageFilter::BOX:
            case djvOpenGLImageFilter::TRIANGLE:
            case djvOpenGLImageFilter::BELL:
            case djvOpenGLImageFilter::BSPLINE:
            case djvOpenGLImageFilter::LANCZOS3:
            case djvOpenGLImageFilter::CUBIC:
            case djvOpenGLImageFilter::MITCHELL:
            {
                //DJV_DEBUG_PRINT("init two pass");
                state->_texture->init(
                    data.info(),
                    GL_TEXTURE_2D,
                    GL_NEAREST,
                    GL_NEAREST);

                // Initialize horizontal pass.
                djvPixelData contrib;
                scaleContrib(
                    data.w(),
                    scale.x,
                    filter,
                    contrib);
                state->_scaleXContrib->init(
                    contrib,
                    GL_TEXTURE_2D,
                    GL_NEAREST,
                    GL_NEAREST);
                state->_scaleXShader->init(
                    sourceVertex,
                    sourceFragment(
                        options.colorProfile.type,
                        djvOpenGLImageDisplayProfile(),
                        static_cast<djvOpenGLImageOptions::CHANNEL>(0),
                        true,
                        contrib.h(),
                        true));

                // Initialize vertical pass.
                scaleContrib(
                    data.h(),
                    scale.y,
                    filter,
                    contrib);
                state->_scaleYContrib->init(
                    contrib,
                    GL_TEXTURE_2D,
                    GL_NEAREST,
                    GL_NEAREST);
                state->_scaleYShader->init(
                    sourceVertex,
                    sourceFragment(
                        static_cast<djvColorProfile::PROFILE>(0),
                        options.displayProfile,
                        options.channel,
                        true,
                        contrib.h(),
                        false));
            }
            break;
            default: break;
        }
        state->_init    = true;
        state->_info    = info;
        state->_options = options;
    }

    // Render.
    const djvPixelDataInfo::Mirror mirror(
        info.mirror.x ? (! options.xform.mirror.x) : options.xform.mirror.x,
        info.mirror.y ? (! options.xform.mirror.y) : options.xform.mirror.y);
    //DJV_DEBUG_PRINT("mirror = " << mirror.x << " " << mirror.y);
    switch (filter)
    {
        case djvOpenGLImageFilter::NEAREST:
        case djvOpenGLImageFilter::LINEAR:
        {
            //DJV_DEBUG_PRINT("draw single pass");

            state->_shader->bind();

            // Initialize color and display profiles.
            colorProfileInit(
                options,
                state->_shader->program(),
                *state->_lutColorProfile);
            displayProfileInit(
                options,
                state->_shader->program(),
                *state->_lutDisplayProfile);

            // Draw.
            activeTexture(GL_TEXTURE0);
            uniform1i(state->_shader->program(), "inTexture", 0);
            state->_texture->copy(data);
            DJV_DEBUG_OPEN_GL(glPushMatrix());
            const glm::mat4x4 m = djvOpenGLImageXform::xformMatrix(options.xform);
            //DJV_DEBUG_PRINT("m = " << m);
            DJV_DEBUG_OPEN_GL(glLoadMatrixf(&m[0][0]));
            quad(info.size, mirror, proxyScale);
            DJV_DEBUG_OPEN_GL(glPopMatrix());
        }
        break;
        case djvOpenGLImageFilter::BOX:
        case djvOpenGLImageFilter::TRIANGLE:
        case djvOpenGLImageFilter::BELL:
        case djvOpenGLImageFilter::BSPLINE:
        case djvOpenGLImageFilter::LANCZOS3:
        case djvOpenGLImageFilter::CUBIC:
        case djvOpenGLImageFilter::MITCHELL:
        {
            //DJV_DEBUG_PRINT("draw two pass");

            // Horizontal pass.
            djvOpenGLOffscreenBuffer buffer(
                djvPixelDataInfo(scaleTmp, data.pixel()));

            {
                djvOpenGLOffscreenBufferScope bufferScope(&buffer);
                state->_scaleXShader->bind();
                colorProfileInit(
                    options,
                    state->_scaleXShader->program(),
                    *state->_lutColorProfile);
                activeTexture(GL_TEXTURE0);
                uniform1i(state->_scaleXShader->program(), "inTexture", 0);
                state->_texture->copy(data);
                state->_texture->bind();
                activeTexture(GL_TEXTURE1);
                uniform1i(
                    state->_scaleXShader->program(), "inScaleContrib", 1);
                state->_scaleXContrib->bind();
                glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                djvOpenGLUtil::ortho(scaleTmp);
                glViewport(0, 0, scaleTmp.x, scaleTmp.y);
                quad(scaleTmp, mirror);
                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
                glPopMatrix();
                glPopAttrib();
            }

            // Vertical pass.
            state->_scaleYShader->bind();
            displayProfileInit(
                options,
                state->_scaleYShader->program(),
                *state->_lutDisplayProfile);
            activeTexture(GL_TEXTURE0);
            uniform1i(state->_scaleYShader->program(), "inTexture", 0);
            DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_2D, buffer.texture()));
            activeTexture(GL_TEXTURE1);
            uniform1i(state->_scaleYShader->program(), "inScaleContrib", 1);
            state->_scaleYContrib->bind();
            djvOpenGLImageXform xform = options.xform;
            xform.scale = glm::vec2(1.f, 1.f);
            const glm::mat4x4 m = djvOpenGLImageXform::xformMatrix(xform);
            DJV_DEBUG_OPEN_GL(glPushMatrix());
            DJV_DEBUG_OPEN_GL(glLoadMatrixf(&m[0][0]));
            quad(scale);
            DJV_DEBUG_OPEN_GL(glPopMatrix());
        }
        break;
        default: break;
    }
}

