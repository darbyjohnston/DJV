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

//! \file djvOpenGlImage.cpp

#include <djvOpenGlImage.h>

#include <djvColorUtil.h>
#include <djvDebug.h>
#include <djvError.h>
#include <djvMatrixUtil.h>
#include <djvOpenGlOffscreenBuffer.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvOpenGlImageXform
//------------------------------------------------------------------------------

djvOpenGlImageXform::djvOpenGlImageXform() :
    scale (1.0),
    rotate(0.0)
{}

djvMatrix3f djvOpenGlImageXform::xformMatrix(const djvOpenGlImageXform & in)
{
    return
        djvMatrixUtil::scale3f    (in.scale) *
        djvMatrixUtil::rotate3f   (in.rotate) *
        djvMatrixUtil::translate3f(in.position);
}

//------------------------------------------------------------------------------
// djvOpenGlImageColor
//------------------------------------------------------------------------------

djvOpenGlImageColor::djvOpenGlImageColor() :
    brightness(1.0),
    contrast  (1.0),
    saturation(1.0)
{}

djvMatrix4f djvOpenGlImageColor::brightnessMatrix(double r, double g, double b)
{
    return djvMatrix4f(
        r, 0, 0, 0,
        0, g, 0, 0,
        0, 0, b, 0,
        0, 0, 0, 1);
}

djvMatrix4f djvOpenGlImageColor::contrastMatrix(double r, double g, double b)
{
    return
        djvMatrix4f(
            1.0,  0.0,  0.0, 0.0,
            0.0,  1.0,  0.0, 0.0,
            0.0,  0.0,  1.0, 0.0,
           -0.5, -0.5, -0.5, 1.0) *
        djvMatrix4f(
            r,   0.0, 0.0, 0.0,
            0.0, g,   0.0, 0.0,
            0.0, 0.0, b,   0.0,
            0.0, 0.0, 0.0, 1.0) *
        djvMatrix4f(
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.5, 0.5, 0.5, 1.0);
}

djvMatrix4f djvOpenGlImageColor::saturationMatrix(double r, double g, double b)
{
    const double s [] =
    {
        (1.0 - r) * 0.3086,
        (1.0 - g) * 0.6094,
        (1.0 - b) * 0.0820
    };

    return djvMatrix4f(
        s[0] + r, s[0],     s[0],     0.0,
        s[1],     s[1] + g, s[1],     0.0,
        s[2],     s[2],     s[2] + b, 0.0,
         0.0,      0.0,      0.0,     1.0);
}

djvMatrix4f djvOpenGlImageColor::colorMatrix(const djvOpenGlImageColor & in)
{
    return
        brightnessMatrix(in.brightness, in.brightness, in.brightness) *
        contrastMatrix  (in.contrast,   in.contrast,   in.contrast  ) *
        saturationMatrix(in.saturation, in.saturation, in.saturation);
}

//------------------------------------------------------------------------------
// djvOpenGlImageLevels
//------------------------------------------------------------------------------

djvOpenGlImageLevels::djvOpenGlImageLevels() :
    inLow  (0.0),
    inHigh (1.0),
    gamma  (1.0),
    outLow (0.0),
    outHigh(1.0)
{}

djvPixelData djvOpenGlImageLevels::colorLut(
    const djvOpenGlImageLevels & in,
    double                       softClip)
{
    djvPixelData out(djvPixelDataInfo(1024, 1, djvPixel::L_F32));

    const double inTmp  = in.inHigh - in.inLow;
    const double gamma  = 1.0 / in.gamma;
    const double outTmp = in.outHigh - in.outLow;

    djvPixel::F32_T * p = reinterpret_cast<djvPixel::F32_T *>(out.data());
    const int size = out.size().x;

    for (int i = 0; i < size; ++i, ++p)
    {
        *p = static_cast<djvPixel::F32_T>(
            djvMath::softClip(
                djvMath::pow(
                    djvMath::max(
                        ((i / static_cast<double>(size - 1) - in.inLow) /
                            inTmp),
                        0.000001),
                    gamma) * outTmp + in.outLow,
                softClip));
    }

    return out;
}

//------------------------------------------------------------------------------
// djvOpenGlImageDisplayProfile
//------------------------------------------------------------------------------

djvOpenGlImageDisplayProfile::djvOpenGlImageDisplayProfile() :
    softClip(0.0)
{}

//------------------------------------------------------------------------------
// djvOpenGlImageFilter
//------------------------------------------------------------------------------

const QStringList & djvOpenGlImageFilter::filterLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvOpenGlImageFilter", "Nearest") <<
        qApp->translate("djvOpenGlImageFilter", "Linear") <<
        qApp->translate("djvOpenGlImageFilter", "Box") <<
        qApp->translate("djvOpenGlImageFilter", "Triangle") <<
        qApp->translate("djvOpenGlImageFilter", "Bell") <<
        qApp->translate("djvOpenGlImageFilter", "B-Spline") <<
        qApp->translate("djvOpenGlImageFilter", "Lanczos3") <<
        qApp->translate("djvOpenGlImageFilter", "Cubic") <<
        qApp->translate("djvOpenGlImageFilter", "Mitchell");

    DJV_ASSERT(data.count() == FILTER_COUNT);

    return data;
}

djvOpenGlImageFilter::djvOpenGlImageFilter() :
    min(filter().min),
    mag(filter().mag)
{}

djvOpenGlImageFilter::djvOpenGlImageFilter(FILTER min, FILTER mag) :
    min(min),
    mag(mag)
{}

GLuint djvOpenGlImageFilter::toGl(FILTER in)
{
    switch (in)
    {
        case NEAREST: return GL_NEAREST;
        case LINEAR:  return GL_LINEAR;

        default: break;
    }

    return 0;
}

const djvOpenGlImageFilter & djvOpenGlImageFilter::filterHighQuality()
{
    static const djvOpenGlImageFilter data(
        djvOpenGlImageFilter::LANCZOS3,
        djvOpenGlImageFilter::MITCHELL);

    return data;
}

djvOpenGlImageFilter djvOpenGlImageFilter::filterDefault()
{
    return djvOpenGlImageFilter(
        djvOpenGlImageFilter::LINEAR,
        djvOpenGlImageFilter::NEAREST);
}

namespace
{

djvOpenGlImageFilter _filter = djvOpenGlImageFilter::filterDefault();

} // namespace

const djvOpenGlImageFilter & djvOpenGlImageFilter::filter()
{
    return _filter;
}

void djvOpenGlImageFilter::setFilter(const djvOpenGlImageFilter & filter)
{
    _filter = filter;
}

//------------------------------------------------------------------------------
// djvOpenGlImageOptions
//------------------------------------------------------------------------------

djvOpenGlImageOptions::djvOpenGlImageOptions() :
    channel   (CHANNEL_DEFAULT),
    proxyScale(true)
{}

const QStringList & djvOpenGlImageOptions::channelLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvOpenGlImageOptions", "Default") <<
        qApp->translate("djvOpenGlImageOptions", "Red") <<
        qApp->translate("djvOpenGlImageOptions", "Green") <<
        qApp->translate("djvOpenGlImageOptions", "Blue") <<
        qApp->translate("djvOpenGlImageOptions", "Alpha");

    DJV_ASSERT(data.count() == CHANNEL_COUNT);

    return data;
}

//------------------------------------------------------------------------------
// djvOpenGlImage
//------------------------------------------------------------------------------

djvOpenGlImage::~djvOpenGlImage()
{}

void djvOpenGlImage::read(djvPixelData & output)
{
    read(output, djvBox2i(output.size()));
}

void djvOpenGlImage::read(djvPixelData & output, const djvBox2i & area)
{
    //DJV_DEBUG("djvOpenGlImage::read");
    //DJV_DEBUG_PRINT("output = " << output);
    //DJV_DEBUG_PRINT("area = " << area);

    const djvPixelDataInfo & info = output.info();

    DJV_DEBUG_OPEN_GL(glPushAttrib(
        GL_CURRENT_BIT |
        GL_ENABLE_BIT |
        GL_PIXEL_MODE_BIT));
    DJV_DEBUG_OPEN_GL(glDisable(GL_DITHER));

    //! \todo What is the correct way to convert from RGB to luminance?

    switch (djvPixel::format(info.pixel))
    {
        case djvPixel::L:
        case djvPixel::LA:

            //DJV_DEBUG_OPEN_GL(glPixelTransferf(GL_GREEN_SCALE, 0.0));
            //DJV_DEBUG_OPEN_GL(glPixelTransferf(GL_BLUE_SCALE, 0.0));

            DJV_DEBUG_OPEN_GL(glPixelTransferf(GL_RED_SCALE,
                static_cast<GLfloat>(1.0 / 3.0)));
            DJV_DEBUG_OPEN_GL(glPixelTransferf(GL_GREEN_SCALE,
                static_cast<GLfloat>(1.0 / 3.0)));
            DJV_DEBUG_OPEN_GL(glPixelTransferf(GL_BLUE_SCALE,
                static_cast<GLfloat>(1.0 / 3.0)));

            break;

        default: break;
    }

    statePack(info, area.position);

    DJV_DEBUG_OPEN_GL(glReadPixels(
        0, 0, area.w, area.h,
        djvOpenGlUtil::format(info.pixel, info.bgr),
        djvOpenGlUtil::type(info.pixel),
        output.data()));

    //stateReset();

    DJV_DEBUG_OPEN_GL(glPopAttrib());
}

namespace
{

bool initAlpha(const djvPixel::PIXEL & input, const djvPixel::PIXEL & output)
{
    switch (djvPixel::format(input))
    {
        case djvPixel::L:
        case djvPixel::RGB:
        
            switch (djvPixel::format(output))
            {
                case djvPixel::LA:
                case djvPixel::RGBA:
                
                    return true;
                    
                default: break;
            }
            
            break;
        
        default: break;
    }
    
    return false;
}

}

void djvOpenGlImage::copy(
    const djvPixelData &          input,
    djvPixelData &                output,
    const djvOpenGlImageOptions & options,
    djvOpenGlImageState *         state,
    djvOpenGlOffscreenBuffer *    buffer) throw (djvError)
{
    //DJV_DEBUG("OpenGlImage::copy");
    //DJV_DEBUG_PRINT("input = " << input);
    //DJV_DEBUG_PRINT("output = " << output);
    //DJV_DEBUG_PRINT("scale = " << options.xform.scale);

    const djvVector2i & size = output.info().size;
    
    QScopedPointer<djvOpenGlOffscreenBuffer> _buffer;

    if (! buffer)
    {
        //DJV_DEBUG_PRINT("create buffer");

        _buffer.reset(
            new djvOpenGlOffscreenBuffer(djvPixelDataInfo(size, output.pixel())));

        buffer = _buffer.data();
    }

    try
    {
        djvOpenGlOffscreenBufferScope bufferScope(buffer);

        djvOpenGlUtil::ortho(size);

        DJV_DEBUG_OPEN_GL(glViewport(0, 0, size.x, size.y));

        //DJV_DEBUG_OPEN_GL(glClearColor(0, 1, 0, 0));

        djvColor background(djvPixel::RGB_F32);

        djvColorUtil::convert(options.background, background);

        DJV_DEBUG_OPEN_GL(glClearColor(
            background.f32(0),
            background.f32(1),
            background.f32(2),
            initAlpha(input.pixel(), output.pixel()) ? 1.0 : 0.0));

        DJV_DEBUG_OPEN_GL(glClear(GL_COLOR_BUFFER_BIT));

        djvOpenGlImageOptions _options = options;

        if (output.info().mirror.x)
        {
            _options.xform.mirror.x = ! _options.xform.mirror.x;
        }

        if (output.info().mirror.y)
        {
            _options.xform.mirror.y = ! _options.xform.mirror.y;
        }

        draw(input, _options, state);

        read(output);
    }
    catch (const djvError & error)
    {
        throw error;
    }
}

void djvOpenGlImage::stateUnpack(const djvPixelDataInfo & in, const djvVector2i & offset)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, in.align);
    glPixelStorei(GL_UNPACK_SWAP_BYTES, in.endian != djvMemory::endian());
    //glPixelStorei(GL_UNPACK_ROW_LENGTH, in.data_window.w);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, offset.y);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, offset.x);
}

void djvOpenGlImage::statePack(const djvPixelDataInfo & in, const djvVector2i & offset)
{
    glPixelStorei(GL_PACK_ALIGNMENT, in.align);
    glPixelStorei(GL_PACK_SWAP_BYTES, in.endian != djvMemory::endian());
    glPixelStorei(GL_PACK_ROW_LENGTH, in.size.x);
    glPixelStorei(GL_PACK_SKIP_ROWS, offset.y);
    glPixelStorei(GL_PACK_SKIP_PIXELS, offset.x);
}

void djvOpenGlImage::stateReset()
{
    statePack(djvPixelDataInfo());
    stateUnpack(djvPixelDataInfo());

    if (GLEW_ARB_shader_objects)
    {
        glUseProgramObjectARB(0);
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void djvOpenGlImage::average(
    const djvPixelData &   in,
    djvColor &             out,
    const djvPixel::Mask & mask) throw (djvError)
{
    //DJV_DEBUG("average");
    //DJV_DEBUG_PRINT("in = " << in);

    out.setPixel(in.pixel());

    const int    w        = in.w();
    const int    h        = in.h();
    const double area     = w * h;
    const int    channels = djvPixel::channels(in.pixel());

    switch (djvPixel::type(in.pixel()))
    {

#define _AVERAGE(TYPE) \
    \
    const djvPixel::TYPE * p = reinterpret_cast< \
        const djvPixel::TYPE *>(in.data()); \
    \
    double accum [djvPixel::channelsMax]; \
    \
    for (int c = 0; c < channels; ++c) \
    { \
        accum[c] = 0.0; \
    } \
    \
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

        case djvPixel::U8:
        {
            _AVERAGE(U8_T)

            for (int c = 0; c < channels; ++c)
            {
                out.setU8(int(accum[c]), c);
            }
        }
        break;

        case djvPixel::U16:
        {
            _AVERAGE(U16_T)

            for (int c = 0; c < channels; ++c)
            {
                out.setU16(int(accum[c]), c);
            }
        }
        break;

        case djvPixel::F16:
        {
            _AVERAGE(F16_T)

            for (int c = 0; c < channels; ++c)
            {
                out.setF16(static_cast<djvPixel::F16_T>(accum[c]), c);
            }
        }
        break;

        case djvPixel::F32:
        {
            _AVERAGE(F32_T)

            for (int c = 0; c < channels; ++c)
            {
                out.setF32(static_cast<djvPixel::F32_T>(accum[c]), c);
            }
        }
        break;

        case djvPixel::U10:
        {
            const djvPixel::U10_S * p =
                reinterpret_cast <const djvPixel::U10_S * > (in.data());
            
            double accum [3] = { 0.0, 0.0, 0.0 };

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

void djvOpenGlImage::histogram(
    const djvPixelData &   in,
    djvPixelData &         out,
    int                    size,
    djvColor &             min,
    djvColor &             max,
    const djvPixel::Mask & mask) throw (djvError)
{
    //DJV_DEBUG("histogram");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("size = " << size);

    // Create the output data using a pixel type of U16.
    
    djvPixel::PIXEL pixel = static_cast<djvPixel::PIXEL>(0);
    
    switch (in.channels())
    {
        case 4:
        case 3: pixel = djvPixel::RGB_U16; break;
        case 2:
        case 1: pixel = djvPixel::L_U16;   break;
        
        default: break;
    }
    
    const int outChannels = djvPixel::channels(pixel);
    
    out.set(djvPixelDataInfo(size, 1, pixel));
    out.zero();

    //DJV_DEBUG_PRINT("out = " << out);
    
    // We may need to convert the input data if it's in a weird format so
    // that we can work directly with the pixels.
    
    const djvPixelData * data = &in;
    djvPixelData tmp;
    djvPixelDataInfo info(in.size(), in.pixel());

    if (in.info() != info)
    {
        //DJV_DEBUG_PRINT("convert");
        
        copy(in, tmp);
        
        data = &tmp;
    }

    min = djvColor(info.pixel);
    max = djvColor(info.pixel);

    // Create a LUT for mapping input pixel values to the output data
    // pixel indexes.

    const int indexLutSize =
        djvPixel::RGB_U10 == info.pixel ? 1024 : (djvPixel::u16Max + 1);
    
    //DJV_DEBUG_PRINT("index lut size = " << indexLutSize);
    
    int * indexLut = new int [indexLutSize];
    
    for (int i = 0; i < indexLutSize; ++i)
    {
        indexLut[i] = djvMath::floor(
            i / static_cast<double>(indexLutSize - 1) * (size - 1));
    }
    
    // Iterate over the input pixels counting their values.
    
    const int w        = info.size.x;
    const int h        = info.size.y;
    const int channels = djvPixel::channels(info.pixel);
    const int pixelMax = djvPixel::u16Max;

    switch (info.pixel)
    {
        case djvPixel::RGB_U10:
        {
            // Setup data pointers.
            
            const djvPixel::U10_S * inP =
                reinterpret_cast<const djvPixel::U10_S *>(data->data());

            const djvPixel::U10_S * const end = inP + w * h;

            djvPixel::U16_T * const outP =
                reinterpret_cast<djvPixel::U16_T *>(out.data());

            djvPixel::U10_S * const minP =
                reinterpret_cast<djvPixel::U10_S *>(min.data());

            djvPixel::U10_S * const maxP =
                reinterpret_cast<djvPixel::U10_S *>(max.data());

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
                    outP[i] = djvMath::min(outP[i] + 1, pixelMax);

                    minP->r = djvMath::min(inP->r, minP->r);
                    maxP->r = djvMath::max(inP->r, maxP->r);
                }
                
                if (mask[1])
                {
                    i = indexLut[inP->g] * outChannels + 1;
                    outP[i] = djvMath::min(outP[i] + 1, pixelMax);

                    minP->g = djvMath::min(inP->g, minP->g);
                    maxP->g = djvMath::max(inP->g, maxP->g);
                }
                
                if (mask[2])
                {
                    i = indexLut[inP->b] * outChannels + 2;
                    outP[i] = djvMath::min(outP[i] + 1, pixelMax);

                    minP->b = djvMath::min(inP->b, minP->b);
                    maxP->b = djvMath::max(inP->b, maxP->b);
                }
            }

        }
        break;

#define _HISTOGRAM(TYPE) \
    \
    const djvPixel::TYPE##_T * inP = \
        reinterpret_cast<const djvPixel::TYPE##_T *>(data->data()); \
    \
    const djvPixel::TYPE##_T * const end = inP + w * h * channels; \
    \
    djvPixel::U16_T * const outP = \
        reinterpret_cast<djvPixel::U16_T *>(out.data()); \
    \
    djvPixel::TYPE##_T * const minP = \
        reinterpret_cast<djvPixel::TYPE##_T *>(min.data()); \
    \
    djvPixel::TYPE##_T * const maxP = \
        reinterpret_cast<djvPixel::TYPE##_T *>(max.data()); \
    \
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
    \
    for (; inP < end; inP += channels) \
    { \
        int i = 0; \
        \
        switch (outChannels) \
        { \
            case 4: \
                \
                if (mask[3]) \
                { \
                    minP[3] = djvMath::min(inP[3], minP[3]); \
                    maxP[3] = djvMath::max(inP[3], maxP[3]); \
                } \
            \
            case 3: \
                \
                if (mask[2]) \
                { \
                    i = indexLut[PIXEL_##TYPE##_TO_U16(inP[2])] * outChannels + 2; \
                    outP[i] = djvMath::min(outP[i] + 1, pixelMax); \
                    \
                    minP[2] = djvMath::min(inP[2], minP[2]); \
                    maxP[2] = djvMath::max(inP[2], maxP[2]); \
                } \
            \
            case 2: \
                \
                if (mask[1]) \
                { \
                    i = indexLut[PIXEL_##TYPE##_TO_U16(inP[1])] * outChannels + 1; \
                    outP[i] = djvMath::min(outP[i] + 1, pixelMax); \
                    \
                    minP[1] = djvMath::min(inP[1], minP[1]); \
                    maxP[1] = djvMath::max(inP[1], maxP[1]); \
                } \
            \
            case 1: \
                \
                if (mask[0]) \
                { \
                    i = indexLut[PIXEL_##TYPE##_TO_U16(inP[0])] * outChannels + 0; \
                    outP[i] = djvMath::min(outP[i] + 1, pixelMax); \
                    \
                    minP[0] = djvMath::min(inP[0], minP[0]); \
                    maxP[0] = djvMath::max(inP[0], maxP[0]); \
                } \
                \
                break; \
        } \
    }

        case djvPixel::L_U8:
        case djvPixel::LA_U8:
        case djvPixel::RGB_U8:
        case djvPixel::RGBA_U8:
        {
            _HISTOGRAM(U8);
        }
        break;

        case djvPixel::L_U16:
        case djvPixel::LA_U16:
        case djvPixel::RGB_U16:
        case djvPixel::RGBA_U16:
        {
            _HISTOGRAM(U16);
        }
        break;

        case djvPixel::L_F16:
        case djvPixel::LA_F16:
        case djvPixel::RGB_F16:
        case djvPixel::RGBA_F16:
        {
            _HISTOGRAM(F16);
        }
        break;

        case djvPixel::L_F32:
        case djvPixel::LA_F32:
        case djvPixel::RGB_F32:
        case djvPixel::RGBA_F32:
        {
            _HISTOGRAM(F32);
        }
        break;

        default: break;
    }
    
    //DJV_DEBUG_PRINT("min = " << min);
    //DJV_DEBUG_PRINT("max = " << max);
    
    delete [] indexLut;

    //djvPixel::U16_T * const outP =
    //    reinterpret_cast<djvPixel::U16_T *>(out.data());
    //for (int i = 0; i < size; ++i)
    //    DJV_DEBUG_PRINT(i << " = " << outP[i * 3]);
}

djvColor djvOpenGlImage::pixel(const djvPixelData & data, int x, int y)
{
    djvPixelData p(djvPixelDataInfo(1, 1, data.pixel()));
    
    djvOpenGlImageOptions options;
    options.xform.position = djvVector2f(-x, -y);

    djvOpenGlImage::copy(data, p, options);
    
    return djvColor(p.data(), p.pixel());
}

//------------------------------------------------------------------------------

bool operator == (const djvOpenGlImageXform & a, const djvOpenGlImageXform & b)
{
    return
        a.mirror   == b.mirror   &&
        a.position == b.position &&
        a.scale    == b.scale    &&
        a.rotate   == b.rotate;
}

bool operator != (const djvOpenGlImageXform & a, const djvOpenGlImageXform & b)
{
    return ! (a == b);
}

bool operator == (const djvOpenGlImageColor & a, const djvOpenGlImageColor & b)
{
    return
        a.brightness == b.brightness &&
        a.contrast   == b.contrast   &&
        a.saturation == b.saturation;
}

bool operator != (const djvOpenGlImageColor & a, const djvOpenGlImageColor & b)
{
    return ! (a == b);
}

bool operator == (const djvOpenGlImageLevels & a, const djvOpenGlImageLevels & b)
{
    return
        a.inLow   == b.inLow  &&
        a.inHigh  == b.inHigh &&
        a.gamma   == b.gamma  &&
        a.outLow  == b.outLow &&
        a.outHigh == b.outHigh;
}

bool operator != (const djvOpenGlImageLevels & a, const djvOpenGlImageLevels & b)
{
    return ! (a == b);
}

bool operator == (
    const djvOpenGlImageDisplayProfile & a,
    const djvOpenGlImageDisplayProfile & b)
{
    return
        a.lut      == b.lut    &&
        a.color    == b.color  &&
        a.levels   == b.levels &&
        a.softClip == b.softClip;
}

bool operator != (
    const djvOpenGlImageDisplayProfile & a,
    const djvOpenGlImageDisplayProfile & b)
{
    return ! (a == b);
}

bool operator == (const djvOpenGlImageFilter & a, const djvOpenGlImageFilter & b)
{
    return
        a.min == b.min &&
        a.mag == b.mag;
}

bool operator != (const djvOpenGlImageFilter & a, const djvOpenGlImageFilter & b)
{
    return ! (a == b);
}

bool operator == (const djvOpenGlImageOptions & a, const djvOpenGlImageOptions & b)
{
    return
        a.xform          == b.xform          &&
        a.colorProfile   == b.colorProfile   &&
        a.displayProfile == b.displayProfile &&
        a.channel        == b.channel        &&
        a.filter         == b.filter         &&
        a.background     == b.background;
}

bool operator != (const djvOpenGlImageOptions & a, const djvOpenGlImageOptions & b)
{
    return ! (a == b);
}

QStringList & operator >> (QStringList & in, djvOpenGlImageXform & out) throw (QString)
{
    return in >>
        out.mirror >>
        out.position >>
        out.scale >>
        out.rotate;
}

QStringList & operator << (QStringList & out, const djvOpenGlImageXform & in)
{
    return out <<
        in.mirror <<
        in.position <<
        in.scale <<
        in.rotate;
}

QStringList & operator >> (QStringList & in, djvOpenGlImageColor & out) throw (QString)
{
    return in >>
        out.brightness >>
        out.contrast >>
        out.saturation;
}

QStringList & operator << (QStringList & out, const djvOpenGlImageColor & in)
{
    return out <<
        in.brightness <<
        in.contrast <<
        in.saturation;
}

QStringList & operator >> (QStringList & in, djvOpenGlImageLevels & out) throw (QString)
{
    return in >>
        out.inLow >>
        out.inHigh >>
        out.gamma >>
        out.outLow >>
        out.outHigh;
}

QStringList & operator << (QStringList & out, const djvOpenGlImageLevels & in)
{
    return out <<
        in.inLow <<
        in.inHigh <<
        in.gamma <<
        in.outLow <<
        in.outHigh;
}

QStringList & operator >> (QStringList & in, djvOpenGlImageFilter & out) throw (QString)
{
    return in >>
        out.min >>
        out.mag;
}

QStringList & operator << (QStringList & out, const djvOpenGlImageFilter & in)
{
    return out <<
        in.min <<
        in.mag;
}

_DJV_STRING_OPERATOR_LABEL(
    djvOpenGlImageFilter::FILTER,
    djvOpenGlImageFilter::filterLabels())
_DJV_STRING_OPERATOR_LABEL(
    djvOpenGlImageOptions::CHANNEL,
    djvOpenGlImageOptions::channelLabels())

djvDebug & operator << (djvDebug & debug, const djvOpenGlImageXform & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const djvOpenGlImageColor & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const djvOpenGlImageLevels & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const djvOpenGlImageDisplayProfile & in)
{
    return debug << "lut = [" << in.lut << "], color = " << in.color <<
        ", levels = " << in.levels << ", soft clip = " << in.softClip;
}

djvDebug & operator << (djvDebug & debug, const djvOpenGlImageFilter & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const djvOpenGlImageFilter::FILTER & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const djvOpenGlImageOptions & in)
{
    return debug << "xform = [" << in.xform << "], color profile = [" <<
        in.colorProfile << "], display profile = [" << in.displayProfile <<
        "], channel = " << in.channel << ", filter = " << in.filter <<
        ", background = " << in.background << ", proxy scale = " <<
        in.proxyScale;
}

djvDebug & operator << (djvDebug & debug, const djvOpenGlImageOptions::CHANNEL & in)
{
    return debug << djvStringUtil::label(in);
}


