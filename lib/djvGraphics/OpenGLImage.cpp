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

#include <djvGraphics/ColorUtil.h>
#include <djvGraphics/OpenGLImagePrivate.h>
#include <djvGraphics/OpenGLOffscreenBuffer.h>
#include <djvGraphics/OpenGLLUT.h>
#include <djvGraphics/OpenGLShader.h>
#include <djvGraphics/OpenGLTexture.h>

#include <djvCore/Debug.h>
#include <djvCore/Error.h>
#include <djvCore/Math.h>

#include <QCoreApplication>
#include <QPixmap>

#include <glm/gtc/matrix_transform.hpp>

namespace djv
{
    namespace Graphics
    {
        glm::mat4x4 OpenGLImageXform::xformMatrix(const OpenGLImageXform & in)
        {
            //DJV_DEBUG("OpenGLImageXform::xformMatrix");
            //DJV_DEBUG_PRINT("scale = " << in.scale);
            //DJV_DEBUG_PRINT("rotate = " << in.rotate);
            //DJV_DEBUG_PRINT("position = " << in.position);
            glm::mat4x4 out =
                glm::translate(glm::mat4x4(1.f), glm::vec3(in.position.x, in.position.y, 0.f)) *
                glm::rotate(glm::mat4x4(1.f), Core::Math::degreesToRadians(in.rotate), glm::vec3(0.f, 0.f, -1.f)) *
                glm::scale(glm::mat4x4(1.f), glm::vec3(in.scale.x, in.scale.y, 1.f));
            return out;
        }

        glm::mat4x4 OpenGLImageColor::brightnessMatrix(float r, float g, float b)
        {
            return glm::mat4x4(
                r, 0.f, 0.f, 0.f,
                0.f, g, 0.f, 0.f,
                0.f, 0.f, b, 0.f,
                0.f, 0.f, 0.f, 1.f);
        }

        glm::mat4x4 OpenGLImageColor::contrastMatrix(float r, float g, float b)
        {
            return
                glm::mat4x4(
                    1.f, 0.f, 0.f, -.5f,
                    0.f, 1.f, 0.f, -.5f,
                    0.f, 0.f, 1.f, -.5f,
                    0.f, 0.f, 0.f, 1.f) *
                glm::mat4x4(
                    r, 0.f, 0.f, 0.f,
                    0.f, g, 0.f, 0.f,
                    0.f, 0.f, b, 0.f,
                    0.f, 0.f, 0.f, 1.f) *
                glm::mat4x4(
                    1.f, 0.f, 0.f, .5f,
                    0.f, 1.f, 0.f, .5f,
                    0.f, 0.f, 1.f, .5f,
                    0.f, 0.f, 0.f, 1.f);
        }

        glm::mat4x4 OpenGLImageColor::saturationMatrix(float r, float g, float b)
        {
            const float s[] =
            {
                (1.f - r) * .3086f,
                (1.f - g) * .6094f,
                (1.f - b) * .0820f
            };
            return glm::mat4x4(
                s[0] + r, s[1], s[2], 0.f,
                s[0], s[1] + g, s[2], 0.f,
                s[0], s[1], s[2] + b, 0.f,
                0.f, 0.f, 0.f, 1.f);
        }

        glm::mat4x4 OpenGLImageColor::colorMatrix(const OpenGLImageColor & in)
        {
            return
                brightnessMatrix(in.brightness, in.brightness, in.brightness) *
                contrastMatrix(in.contrast, in.contrast, in.contrast) *
                saturationMatrix(in.saturation, in.saturation, in.saturation);
        }

        PixelData OpenGLImageLevels::colorLut(
            const OpenGLImageLevels & in,
            float                     softClip)
        {
            PixelData out(PixelDataInfo(1024, 1, Pixel::L_F32));
            const float inTmp = in.inHigh - in.inLow;
            const float gamma = 1.f / in.gamma;
            const float outTmp = in.outHigh - in.outLow;
            Pixel::F32_T * p = reinterpret_cast<Pixel::F32_T *>(out.data());
            const int size = out.size().x;
            for (int i = 0; i < size; ++i, ++p)
            {
                *p = static_cast<Pixel::F32_T>(
                    Core::Math::softClip(
                        Core::Math::pow(
                            Core::Math::max(
                            ((i / static_cast<float>(size - 1) - in.inLow) /
                                inTmp),
                                0.000001f),
                            gamma) * outTmp + in.outLow,
                        softClip));
            }
            return out;
        }

        const QStringList & OpenGLImageFilter::filterLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::OpenGLImageFilter", "Nearest") <<
                qApp->translate("djv::Graphics::OpenGLImageFilter", "Linear") <<
                qApp->translate("djv::Graphics::OpenGLImageFilter", "Box") <<
                qApp->translate("djv::Graphics::OpenGLImageFilter", "Triangle") <<
                qApp->translate("djv::Graphics::OpenGLImageFilter", "Bell") <<
                qApp->translate("djv::Graphics::OpenGLImageFilter", "B-Spline") <<
                qApp->translate("djv::Graphics::OpenGLImageFilter", "Lanczos3") <<
                qApp->translate("djv::Graphics::OpenGLImageFilter", "Cubic") <<
                qApp->translate("djv::Graphics::OpenGLImageFilter", "Mitchell");
            DJV_ASSERT(data.count() == FILTER_COUNT);
            return data;
        }

        OpenGLImageFilter::OpenGLImageFilter() :
            min(filter().min),
            mag(filter().mag)
        {}

        OpenGLImageFilter::OpenGLImageFilter(FILTER min, FILTER mag) :
            min(min),
            mag(mag)
        {}

        GLenum OpenGLImageFilter::toGl(FILTER in)
        {
            switch (in)
            {
            case NEAREST: return GL_NEAREST;
            case LINEAR:  return GL_LINEAR;
            default: break;
            }
            return GL_NONE;
        }

        const OpenGLImageFilter & OpenGLImageFilter::filterHighQuality()
        {
            static const OpenGLImageFilter data(
                OpenGLImageFilter::LANCZOS3,
                OpenGLImageFilter::MITCHELL);
            return data;
        }

        OpenGLImageFilter OpenGLImageFilter::filterDefault()
        {
            return OpenGLImageFilter(
                OpenGLImageFilter::LINEAR,
                OpenGLImageFilter::NEAREST);
        }

        namespace
        {
            OpenGLImageFilter _filter = OpenGLImageFilter::filterDefault();

        } // namespace

        const OpenGLImageFilter & OpenGLImageFilter::filter()
        {
            return _filter;
        }

        void OpenGLImageFilter::setFilter(const OpenGLImageFilter & filter)
        {
            _filter = filter;
        }

        const QStringList & OpenGLImageOptions::channelLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::OpenGLImageOptions", "Default") <<
                qApp->translate("djv::Graphics::OpenGLImageOptions", "Red") <<
                qApp->translate("djv::Graphics::OpenGLImageOptions", "Green") <<
                qApp->translate("djv::Graphics::OpenGLImageOptions", "Blue") <<
                qApp->translate("djv::Graphics::OpenGLImageOptions", "Alpha");
            DJV_ASSERT(data.count() == CHANNEL_COUNT);
            return data;
        }

        OpenGLImage::OpenGLImage() :
            _p(new Private)
        {
            _p->texture.reset(new OpenGLTexture);
            _p->shader.reset(new OpenGLShader);
            _p->scaleXContrib.reset(new OpenGLTexture);
            _p->scaleYContrib.reset(new OpenGLTexture);
            _p->scaleXShader.reset(new OpenGLShader);
            _p->scaleYShader.reset(new OpenGLShader);
            _p->lutColorProfile.reset(new OpenGLLUT);
            _p->lutDisplayProfile.reset(new OpenGLLUT);
            _p->mesh.reset(new OpenGLImageMesh);
        }

        OpenGLImage::~OpenGLImage()
        {}

        namespace
        {
            bool initAlpha(const Pixel::PIXEL & input, const Pixel::PIXEL & output)
            {
                switch (Pixel::format(input))
                {
                case Pixel::L:
                case Pixel::RGB:
                    switch (Pixel::format(output))
                    {
                    case Pixel::LA:
                    case Pixel::RGBA: return true;
                    default: break;
                    }
                    break;
                default: break;
                }
                return false;
            }

        } // namespace

        void OpenGLImage::copy(
            const PixelData &          input,
            PixelData &                output,
            const OpenGLImageOptions & options)
        {
            //DJV_DEBUG("OpenGLImage::copy");
            //DJV_DEBUG_PRINT("input = " << input);
            //DJV_DEBUG_PRINT("output = " << output);
            //DJV_DEBUG_PRINT("scale = " << options.xform.scale);

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

            if (!_p->buffer || (_p->buffer && _p->buffer->info() != output.info()))
            {
                _p->buffer.reset(new OpenGLOffscreenBuffer(PixelDataInfo(output.info())));
            }

            try
            {
                OpenGLOffscreenBufferScope bufferScope(_p->buffer.get());

                glFuncs->glViewport(0, 0, output.w(), output.h());
                Color background(Pixel::RGB_F32);
                ColorUtil::convert(options.background, background);
                glFuncs->glClearColor(
                    background.f32(0),
                    background.f32(1),
                    background.f32(2),
                    initAlpha(input.pixel(), output.pixel()) ? 1.f : 0.f);
                //glClearColor(0, 1, 0, 0);
                glFuncs->glClear(GL_COLOR_BUFFER_BIT);

                OpenGLImageOptions _options = options;
                if (output.info().mirror.x)
                {
                    _options.xform.mirror.x = !_options.xform.mirror.x;
                }
                if (output.info().mirror.y)
                {
                    _options.xform.mirror.y = !_options.xform.mirror.y;
                }

                auto viewMatrix = glm::ortho(
                    0.f,
                    static_cast<float>(output.w()),
                    0.f,
                    static_cast<float>(output.h()),
                    -1.f,
                    1.f);
                draw(input, viewMatrix, _options, Pixel::format(output.pixel()));

                const PixelDataInfo & info = output.info();
                statePack(info);
                glFuncs->glReadPixels(
                    0, 0, output.w(), output.h(),
                    OpenGL::format(info.pixel, info.bgr),
                    OpenGL::type(info.pixel),
                    output.data());
            }
            catch (const Core::Error & error)
            {
                throw error;
            }
        }

        void OpenGLImage::stateUnpack(const PixelDataInfo & in, const glm::ivec2 & offset)
        {
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            glFuncs->glPixelStorei(GL_UNPACK_ALIGNMENT, in.align);
            glFuncs->glPixelStorei(GL_UNPACK_SWAP_BYTES, in.endian != Core::Memory::endian());
            //glFuncs->glPixelStorei(GL_UNPACK_ROW_LENGTH, in.data_window.w);
            glFuncs->glPixelStorei(GL_UNPACK_SKIP_ROWS, offset.y);
            glFuncs->glPixelStorei(GL_UNPACK_SKIP_PIXELS, offset.x);
        }

        void OpenGLImage::statePack(const PixelDataInfo & in, const glm::ivec2 & offset)
        {
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            glFuncs->glPixelStorei(GL_PACK_ALIGNMENT, in.align);
            glFuncs->glPixelStorei(GL_PACK_SWAP_BYTES, in.endian != Core::Memory::endian());
            glFuncs->glPixelStorei(GL_PACK_ROW_LENGTH, in.size.x);
            glFuncs->glPixelStorei(GL_PACK_SKIP_ROWS, offset.y);
            glFuncs->glPixelStorei(GL_PACK_SKIP_PIXELS, offset.x);
        }

        void OpenGLImage::average(
            const PixelData &   in,
            Color &             out,
            const Pixel::Mask & mask)
        {
            //DJV_DEBUG("average");
            //DJV_DEBUG_PRINT("in = " << in);

            out.setPixel(in.pixel());

            const int   w = in.w();
            const int   h = in.h();
            const float area = w * h;
            const int   channels = Pixel::channels(in.pixel());
            switch (Pixel::type(in.pixel()))
            {
#define _AVERAGE(TYPE) \
    const Pixel::TYPE * p = reinterpret_cast<const Pixel::TYPE *>(in.data()); \
    float accum [Pixel::channelsMax]; \
    for (int c = 0; c < channels; ++c) \
    { \
        accum[c] = 0.f; \
    } \
    for (int y = 0; y < h; ++y) \
    { \
        for (int x = 0; x < w; ++x, p += channels) \
        { \
            for (int c = 0; c < channels; ++c) \
            { \
                if (mask[c]) \
                { \
                    accum[c] += p[c] / area; \
                } \
            } \
        } \
    }
            case Pixel::U8:
            {
                _AVERAGE(U8_T)
                    for (int c = 0; c < channels; ++c)
                    {
                        out.setU8(int(accum[c]), c);
                    }
            }
            break;
            case Pixel::U16:
            {
                _AVERAGE(U16_T)
                    for (int c = 0; c < channels; ++c)
                    {
                        out.setU16(int(accum[c]), c);
                    }
            }
            break;
            case Pixel::F16:
            {
                _AVERAGE(F16_T)
                    for (int c = 0; c < channels; ++c)
                    {
                        out.setF16(static_cast<Pixel::F16_T>(accum[c]), c);
                    }
            }
            break;
            case Pixel::F32:
            {
                _AVERAGE(F32_T)
                    for (int c = 0; c < channels; ++c)
                    {
                        out.setF32(static_cast<Pixel::F32_T>(accum[c]), c);
                    }
            }
            break;
            case Pixel::U10:
            {
                const Pixel::U10_S * p = reinterpret_cast <const Pixel::U10_S *> (in.data());
                float accum[3] = { 0.f, 0.f, 0.f };
                for (int y = 0; y < h; ++y)
                {
                    for (int x = 0; x < w; ++x, ++p)
                    {
                        if (mask[0])
                            accum[0] += p->r / area;
                        if (mask[1])
                            accum[1] += p->g / area;
                        if (mask[2])
                            accum[2] += p->b / area;
                    }
                }
                for (int c = 0; c < 3; ++c)
                {
                    out.setU10(static_cast<int>(accum[c]), c);
                }
            }
            break;
            default: break;
            }
            //DJV_DEBUG_PRINT("out = " << out);
        }

        void OpenGLImage::histogram(
            const PixelData &   in,
            PixelData &         out,
            int                 size,
            Color &             min,
            Color &             max,
            const Pixel::Mask & mask)
        {
            //DJV_DEBUG("histogram");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("size = " << size);

            // Create the output data using a pixel type of U16.
            Pixel::PIXEL pixel = static_cast<Pixel::PIXEL>(0);
            switch (in.channels())
            {
            case 4:
            case 3: pixel = Pixel::RGB_U16; break;
            case 2:
            case 1: pixel = Pixel::L_U16;   break;
            default: break;
            }

            const int outChannels = Pixel::channels(pixel);
            out.set(PixelDataInfo(size, 1, pixel));
            out.zero();
            //DJV_DEBUG_PRINT("out = " << out);

            // We may need to convert the input data if it's in a weird format so
            // that we can work directly with the pixels.
            const PixelData * data = &in;
            PixelData tmp;
            PixelDataInfo info(in.size(), in.pixel());
            if (in.info() != info)
            {
                //DJV_DEBUG_PRINT("convert");
                copy(in, tmp);
                data = &tmp;
            }

            min = Color(info.pixel);
            max = Color(info.pixel);

            // Create a LUT for mapping input pixel values to the output data
            // pixel indexes.
            const int indexLutSize = Pixel::RGB_U10 == info.pixel ? 1024 : (Pixel::u16Max + 1);
            //DJV_DEBUG_PRINT("index lut size = " << indexLutSize);    
            int * indexLut = new int[indexLutSize];
            for (int i = 0; i < indexLutSize; ++i)
            {
                indexLut[i] = Core::Math::floor(
                    i / static_cast<float>(indexLutSize - 1) * (size - 1));
            }

            // Iterate over the input pixels counting their values.    
            const int w = info.size.x;
            const int h = info.size.y;
            const int channels = Pixel::channels(info.pixel);
            const int pixelMax = Pixel::u16Max;
            switch (info.pixel)
            {
            case Pixel::RGB_U10:
            {
                // Setup data pointers.
                const Pixel::U10_S * inP = reinterpret_cast<const Pixel::U10_S *>(data->data());
                const Pixel::U10_S * const end = inP + w * h;
                Pixel::U16_T * const outP = reinterpret_cast<Pixel::U16_T *>(out.data());
                Pixel::U10_S * const minP = reinterpret_cast<Pixel::U10_S *>(min.data());
                Pixel::U10_S * const maxP = reinterpret_cast<Pixel::U10_S *>(max.data());

                // Initialize the minimum and maximum values with the first pixel
                // in the input image.            
                if (w && h)
                {
                    if (mask[0])
                        minP->r = maxP->r = inP->r;
                    if (mask[1])
                        minP->g = maxP->g = inP->g;
                    if (mask[2])
                        minP->b = maxP->b = inP->b;
                }

                // Go...
                for (; inP < end; ++inP)
                {
                    int i = 0;
                    if (mask[0])
                    {
                        i = indexLut[inP->r] * outChannels + 0;
                        outP[i] = Core::Math::min(outP[i] + 1, pixelMax);
                        minP->r = Core::Math::min(inP->r, minP->r);
                        maxP->r = Core::Math::max(inP->r, maxP->r);
                    }
                    if (mask[1])
                    {
                        i = indexLut[inP->g] * outChannels + 1;
                        outP[i] = Core::Math::min(outP[i] + 1, pixelMax);
                        minP->g = Core::Math::min(inP->g, minP->g);
                        maxP->g = Core::Math::max(inP->g, maxP->g);
                    }
                    if (mask[2])
                    {
                        i = indexLut[inP->b] * outChannels + 2;
                        outP[i] = Core::Math::min(outP[i] + 1, pixelMax);
                        minP->b = Core::Math::min(inP->b, minP->b);
                        maxP->b = Core::Math::max(inP->b, maxP->b);
                    }
                }
            }
            break;
#define _HISTOGRAM(TYPE) \
    const Pixel::TYPE##_T * inP = reinterpret_cast<const Pixel::TYPE##_T *>(data->data()); \
    const Pixel::TYPE##_T * const end = inP + w * h * channels; \
    Pixel::U16_T * const outP = reinterpret_cast<Pixel::U16_T *>(out.data()); \
    Pixel::TYPE##_T * const minP = reinterpret_cast<Pixel::TYPE##_T *>(min.data()); \
    Pixel::TYPE##_T * const maxP = reinterpret_cast<Pixel::TYPE##_T *>(max.data()); \
    if (w && h) \
    { \
        for (int c = 0; c < channels; ++c) \
        { \
            if (mask[c]) \
            { \
                minP[c] = maxP[c] = inP[c]; \
            } \
        } \
    } \
    for (; inP < end; inP += channels) \
    { \
        int i = 0; \
        switch (outChannels) \
        { \
            case 4: \
                if (mask[3]) \
                { \
                    minP[3] = Core::Math::min(inP[3], minP[3]); \
                    maxP[3] = Core::Math::max(inP[3], maxP[3]); \
                } \
            case 3: \
                if (mask[2]) \
                { \
                    i = indexLut[PIXEL_##TYPE##_TO_U16(inP[2])] * outChannels + 2; \
                    outP[i] = Core::Math::min(outP[i] + 1, pixelMax); \
                    \
                    minP[2] = Core::Math::min(inP[2], minP[2]); \
                    maxP[2] = Core::Math::max(inP[2], maxP[2]); \
                } \
            \
            case 2: \
                if (mask[1]) \
                { \
                    i = indexLut[PIXEL_##TYPE##_TO_U16(inP[1])] * outChannels + 1; \
                    outP[i] = Core::Math::min(outP[i] + 1, pixelMax); \
                    minP[1] = Core::Math::min(inP[1], minP[1]); \
                    maxP[1] = Core::Math::max(inP[1], maxP[1]); \
                } \
            case 1: \
                if (mask[0]) \
                { \
                    i = indexLut[PIXEL_##TYPE##_TO_U16(inP[0])] * outChannels + 0; \
                    outP[i] = Core::Math::min(outP[i] + 1, pixelMax); \
                    minP[0] = Core::Math::min(inP[0], minP[0]); \
                    maxP[0] = Core::Math::max(inP[0], maxP[0]); \
                } \
                break; \
        } \
    }
            case Pixel::L_U8:
            case Pixel::LA_U8:
            case Pixel::RGB_U8:
            case Pixel::RGBA_U8:
            {
                _HISTOGRAM(U8);
            }
            break;
            case Pixel::L_U16:
            case Pixel::LA_U16:
            case Pixel::RGB_U16:
            case Pixel::RGBA_U16:
            {
                _HISTOGRAM(U16);
            }
            break;
            case Pixel::L_F16:
            case Pixel::LA_F16:
            case Pixel::RGB_F16:
            case Pixel::RGBA_F16:
            {
                _HISTOGRAM(F16);
            }
            break;
            case Pixel::L_F32:
            case Pixel::LA_F32:
            case Pixel::RGB_F32:
            case Pixel::RGBA_F32:
            {
                _HISTOGRAM(F32);
            }
            break;
            default: break;
            }
            //DJV_DEBUG_PRINT("min = " << min);
            //DJV_DEBUG_PRINT("max = " << max);

            delete[] indexLut;

            //Core::Pixel::U16_T * const outP = reinterpret_cast<Core::Pixel::U16_T *>(out.data());
            //for (int i = 0; i < size; ++i)
            //    DJV_DEBUG_PRINT(i << " = " << outP[i * 3]);
        }

        QPixmap OpenGLImage::toQt(
            const PixelData &          pixelData,
            const OpenGLImageOptions & options)
        {
            //DJV_DEBUG("PixmapUtil::toQt");
            //DJV_DEBUG_PRINT("pixelData = " << pixelData);

            // Convert the pixel data to an 8-bit format.
            const int w = pixelData.w();
            const int h = pixelData.h();
            const PixelDataInfo info(w, h, Pixel::RGBA_U8);
            const PixelData * pixelDataP = &pixelData;
            PixelData tmp;
            if (pixelDataP->pixel() != info.pixel)
            {
                tmp.set(info);
                copy(pixelData, tmp, options);
                pixelDataP = &tmp;
            }

            // Convert to a QImage.
            QImage qImage(w, h, QImage::Format_ARGB32);

            //! \todo It looks like when RGB_U10 is being converted to RGBA_U8
            //! the alpha channel is 0 instead of 255. This is a hack to work
            //! around the issue but how do we fix the underlying problem?
            const bool alpha = pixelData.pixel() != Pixel::RGB_U10;
            //DJV_DEBUG_PRINT("alpha = " << alpha);

            for (int y = 0; y < h; ++y)
            {
                QRgb * qRgb = (QRgb *)qImage.scanLine(y);
                const uchar * p = pixelDataP->data(0, h - 1 - y);
                for (int x = 0; x < w; ++x, p += 4)
                {
                    //const uchar * p = pixelDataP->data(x, h - 1 - y);
                    //DJV_DEBUG_PRINT("pixel (" << x << ", " << y << ") = " <<
                    //    p[0] << " " << p[1] << " " << p[2] << " " << p[3]);
                    qRgb[x] = qRgba(p[0], p[1], p[2], alpha ? p[3] : 255);
                }
            }

            return QPixmap::fromImage(qImage);
        }

    } // namespace Graphics

    bool operator == (const Graphics::OpenGLImageXform & a, const Graphics::OpenGLImageXform & b)
    {
        return
            a.mirror == b.mirror   &&
            a.position == b.position &&
            a.scale == b.scale    &&
            a.rotate == b.rotate;
    }

    bool operator != (const Graphics::OpenGLImageXform & a, const Graphics::OpenGLImageXform & b)
    {
        return !(a == b);
    }

    bool operator == (const Graphics::OpenGLImageColor & a, const Graphics::OpenGLImageColor & b)
    {
        return
            a.brightness == b.brightness &&
            a.contrast == b.contrast   &&
            a.saturation == b.saturation;
    }

    bool operator != (const Graphics::OpenGLImageColor & a, const Graphics::OpenGLImageColor & b)
    {
        return !(a == b);
    }

    bool operator == (const Graphics::OpenGLImageLevels & a, const Graphics::OpenGLImageLevels & b)
    {
        return
            a.inLow == b.inLow  &&
            a.inHigh == b.inHigh &&
            a.gamma == b.gamma  &&
            a.outLow == b.outLow &&
            a.outHigh == b.outHigh;
    }

    bool operator != (const Graphics::OpenGLImageLevels & a, const Graphics::OpenGLImageLevels & b)
    {
        return !(a == b);
    }

    bool operator == (
        const Graphics::OpenGLImageDisplayProfile & a,
        const Graphics::OpenGLImageDisplayProfile & b)
    {
        return
            a.lut == b.lut    &&
            a.color == b.color  &&
            a.levels == b.levels &&
            a.softClip == b.softClip;
    }

    bool operator != (
        const Graphics::OpenGLImageDisplayProfile & a,
        const Graphics::OpenGLImageDisplayProfile & b)
    {
        return !(a == b);
    }

    bool operator == (const Graphics::OpenGLImageFilter & a, const Graphics::OpenGLImageFilter & b)
    {
        return
            a.min == b.min &&
            a.mag == b.mag;
    }

    bool operator != (const Graphics::OpenGLImageFilter & a, const Graphics::OpenGLImageFilter & b)
    {
        return !(a == b);
    }

    bool operator == (const Graphics::OpenGLImageOptions & a, const Graphics::OpenGLImageOptions & b)
    {
        return
            a.xform == b.xform &&
            a.premultipliedAlpha == b.premultipliedAlpha &&
            a.colorProfile == b.colorProfile &&
            a.displayProfile == b.displayProfile &&
            a.channel == b.channel &&
            a.filter == b.filter &&
            a.background == b.background;
    }

    bool operator != (const Graphics::OpenGLImageOptions & a, const Graphics::OpenGLImageOptions & b)
    {
        return !(a == b);
    }

    QStringList & operator >> (QStringList & in, Graphics::OpenGLImageXform & out)
    {
        return in >>
            out.mirror >>
            out.position >>
            out.scale >>
            out.rotate;
    }

    QStringList & operator << (QStringList & out, const Graphics::OpenGLImageXform & in)
    {
        return out <<
            in.mirror <<
            in.position <<
            in.scale <<
            in.rotate;
    }

    QStringList & operator >> (QStringList & in, Graphics::OpenGLImageColor & out)
    {
        return in >>
            out.brightness >>
            out.contrast >>
            out.saturation;
    }

    QStringList & operator << (QStringList & out, const Graphics::OpenGLImageColor & in)
    {
        return out <<
            in.brightness <<
            in.contrast <<
            in.saturation;
    }

    QStringList & operator >> (QStringList & in, Graphics::OpenGLImageLevels & out)
    {
        return in >>
            out.inLow >>
            out.inHigh >>
            out.gamma >>
            out.outLow >>
            out.outHigh;
    }

    QStringList & operator << (QStringList & out, const Graphics::OpenGLImageLevels & in)
    {
        return out <<
            in.inLow <<
            in.inHigh <<
            in.gamma <<
            in.outLow <<
            in.outHigh;
    }

    QStringList & operator >> (QStringList & in, Graphics::OpenGLImageFilter & out)
    {
        return in >>
            out.min >>
            out.mag;
    }

    QStringList & operator << (QStringList & out, const Graphics::OpenGLImageFilter & in)
    {
        return out <<
            in.min <<
            in.mag;
    }

    _DJV_STRING_OPERATOR_LABEL(
        Graphics::OpenGLImageFilter::FILTER,
        Graphics::OpenGLImageFilter::filterLabels());
    _DJV_STRING_OPERATOR_LABEL(
        Graphics::OpenGLImageOptions::CHANNEL,
        Graphics::OpenGLImageOptions::channelLabels());

    Core::Debug & operator << (Core::Debug & debug, const Graphics::OpenGLImageXform & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::OpenGLImageColor & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::OpenGLImageLevels & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::OpenGLImageDisplayProfile & in)
    {
        return debug << "lut = [" << in.lut << "], color = " << in.color <<
            ", levels = " << in.levels << ", soft clip = " << in.softClip;
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::OpenGLImageFilter & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::OpenGLImageFilter::FILTER & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::OpenGLImageOptions & in)
    {
        return debug << "xform = [" << in.xform << "], premultiplied alpha = " << in.premultipliedAlpha <<
            ", color profile = [" << in.colorProfile << "], display profile = [" << in.displayProfile <<
            "], channel = " << in.channel << ", filter = " << in.filter <<
            ", background = " << in.background << ", proxy scale = " <<
            in.proxyScale;
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::OpenGLImageOptions::CHANNEL & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

} // namespace djv
