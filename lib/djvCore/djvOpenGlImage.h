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

#pragma once

#include <djvBox.h>
#include <djvColor.h>
#include <djvColorProfile.h>
#include <djvError.h>
#include <djvMatrix.h>
#include <djvOpenGl.h>
#include <djvPixelData.h>

#include <QMetaType>

class djvOpenGlLut;
class djvOpenGlOffscreenBuffer;
class djvOpenGlTexture;
class djvOpenGlShader;

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

//! \addtogroup djvCoreOpenGL
//@{

//------------------------------------------------------------------------------
//! \class djvOpenGlImageXform
//!
//! This class provides OpenGL image transform options.
//------------------------------------------------------------------------------

class djvOpenGlImageXform
{
public:
    djvPixelDataInfo::Mirror mirror;
    glm::vec2                position = glm::vec2(0.f, 0.f);
    glm::vec2                scale    = glm::vec2(1.f, 1.f);
    float                    rotate   = 0.f;

    //! Create a transform matrix.
    static glm::mat4x4 xformMatrix(const djvOpenGlImageXform &);
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImageColor
//!
//! This class provides OpenGL image color options.
//------------------------------------------------------------------------------

class djvOpenGlImageColor
{
public:
    float brightness = 1.f;
    float contrast   = 1.f;
    float saturation = 1.f;

    //! Create a brightness matrix.
    static glm::mat4x4 brightnessMatrix(float r, float g, float b);

    //! Create a contrast matrix.
    static glm::mat4x4 contrastMatrix(float r, float g, float b);

    //! Create a saturation matrix.
    static glm::mat4x4 saturationMatrix(float r, float g, float b);

    //! Create a color matrix.
    static glm::mat4x4 colorMatrix(const djvOpenGlImageColor &);
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImageLevels
//!
//! This class provides OpenGL image color levels options.
//------------------------------------------------------------------------------

class djvOpenGlImageLevels
{
public:
    float inLow   = 0.f;
    float inHigh  = 1.f;
    float gamma   = 1.f;
    float outLow  = 0.f;
    float outHigh = 1.f;

    //! Create a lookup table from color levels.
    static djvPixelData colorLut(const djvOpenGlImageLevels &, float softClip);
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImageDisplayProfile
//!
//! This class provides OpenGL image display profile options.
//------------------------------------------------------------------------------

class djvOpenGlImageDisplayProfile
{
public:
    djvPixelData         lut;
    djvOpenGlImageColor  color;
    djvOpenGlImageLevels levels;
    float                softClip = 0.f;
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImageFilter
//!
//! This class provides OpenGL image filtering options.
//------------------------------------------------------------------------------

class djvOpenGlImageFilter
{
    Q_GADGET
    Q_ENUMS(FILTER)
    
public:
    //! This enumeration provides the image filters.
    enum FILTER
    {
        NEAREST,
        LINEAR,
        BOX,
        TRIANGLE,
        BELL,
        BSPLINE,
        LANCZOS3,
        CUBIC,
        MITCHELL,

        FILTER_COUNT
    };

    //! Get the image filter labels.
    static const QStringList & filterLabels();

    djvOpenGlImageFilter();
    djvOpenGlImageFilter(FILTER min, FILTER mag);

    FILTER min;
    FILTER mag;

    //! Convert an image filter to OpenGL.
    static gl::GLenum toGl(FILTER);

    //! Get the default image filter.
    static djvOpenGlImageFilter filterDefault();

    //! Get the high quality image filter.
    static const djvOpenGlImageFilter & filterHighQuality();

    //! Get the global image filter.
    static const djvOpenGlImageFilter & filter();

    //! Set the global image filter.
    static void setFilter(const djvOpenGlImageFilter &);
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImageOptions
//!
//! This class provides OpenGL image options.
//------------------------------------------------------------------------------

class djvOpenGlImageOptions
{
    Q_GADGET
    Q_ENUMS(CHANNEL)

public:
    //! This enumeration provides the channels to display.
    enum CHANNEL
    {
        CHANNEL_DEFAULT,
        CHANNEL_RED,
        CHANNEL_GREEN,
        CHANNEL_BLUE,
        CHANNEL_ALPHA,

        CHANNEL_COUNT
    };

    //! Get the channel labels.
    static const QStringList & channelLabels();

    djvOpenGlImageXform          xform;
    djvColorProfile              colorProfile;
    djvOpenGlImageDisplayProfile displayProfile;
    CHANNEL                      channel        = CHANNEL_DEFAULT;
    djvOpenGlImageFilter         filter;
    djvColor                     background;
    bool                         proxyScale     = true;
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImageState
//!
//! This class provides OpenGL image state.
//------------------------------------------------------------------------------

class djvOpenGlImageState
{
public:
    djvOpenGlImageState();
    ~djvOpenGlImageState();

private:
    DJV_PRIVATE_COPY(djvOpenGlImageState);

    bool                  _init;
    djvPixelDataInfo      _info;
    djvOpenGlImageOptions _options;
    djvOpenGlTexture *    _texture;
    djvOpenGlShader *     _shader;
    djvOpenGlTexture *    _scaleXContrib;
    djvOpenGlTexture *    _scaleYContrib;
    djvOpenGlShader *     _scaleXShader;
    djvOpenGlShader *     _scaleYShader;
    djvOpenGlLut *        _lutColorProfile;
    djvOpenGlLut *        _lutDisplayProfile;

    friend class djvOpenGlImage;
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImage
//!
//! This class provides OpenGL image utilities.
//------------------------------------------------------------------------------

class djvOpenGlImage
{
public:
    virtual ~djvOpenGlImage() = 0;

    //! Draw pixel data.
    static void draw(
        const djvPixelData &          data,
        const djvOpenGlImageOptions & options = djvOpenGlImageOptions(),
        djvOpenGlImageState *         state = 0) throw (djvError);

    //! Read pixel data.
    static void read(djvPixelData &);

    //! Read pixel data.
    static void read(djvPixelData &, const djvBox2i &);

    //! Copy pixel data.
    static void copy(
        const djvPixelData &          input,
        djvPixelData &                output,
        const djvOpenGlImageOptions & options = djvOpenGlImageOptions(),
        djvOpenGlImageState *         state   = 0,
        djvOpenGlOffscreenBuffer *    buffer  = 0) throw (djvError);

    //! Setup OpenGL state for image drawing.
    static void stateUnpack(
        const djvPixelDataInfo & info,
        const glm::ivec2 &       offset = glm::ivec2(0, 0));

    //! Setup OpenGL state for image reading.
    static void statePack(
        const djvPixelDataInfo & info,
        const glm::ivec2 &       offset = glm::ivec2(0, 0));

    //! Reset OpenGL state.
    static void stateReset();

    //! Calculate the average color.
    //!
    //! \todo Use a GPU implementation.
    static void average(
        const djvPixelData &   input,
        djvColor &             output,
        const djvPixel::Mask & mask   = djvPixel::Mask()) throw (djvError);

    //! Calculate the histogram.
    //!
    //! \todo Use a GPU implementation.
    static void histogram(
        const djvPixelData &   input,
        djvPixelData &         output,
        int                    size,
        djvColor &             min,
        djvColor &             max,
        const djvPixel::Mask & mask = djvPixel::Mask()) throw (djvError);
    
    //! Get a pixel.
    static djvColor pixel(const djvPixelData &, int x, int y);
    
    //! This enumeration provides error codes.
    enum ERROR
    {
        ERROR_CREATE_TEXTURE,
        ERROR_CREATE_TEXTURE2,
        
        ERROR_COUNT
    };
    
    //! Get the error code labels.
    static const QStringList & errorLabels();
};

DJV_COMPARISON_OPERATOR(djvOpenGlImageXform);
DJV_COMPARISON_OPERATOR(djvOpenGlImageColor);
DJV_COMPARISON_OPERATOR(djvOpenGlImageLevels);
DJV_COMPARISON_OPERATOR(djvOpenGlImageDisplayProfile);
DJV_COMPARISON_OPERATOR(djvOpenGlImageFilter);
DJV_COMPARISON_OPERATOR(djvOpenGlImageOptions);

DJV_STRING_OPERATOR(djvOpenGlImageXform);
DJV_STRING_OPERATOR(djvOpenGlImageColor);
DJV_STRING_OPERATOR(djvOpenGlImageLevels);
DJV_STRING_OPERATOR(djvOpenGlImageFilter);
DJV_STRING_OPERATOR(djvOpenGlImageFilter::FILTER);
DJV_STRING_OPERATOR(djvOpenGlImageOptions::CHANNEL);

DJV_DEBUG_OPERATOR(djvOpenGlImageXform);
DJV_DEBUG_OPERATOR(djvOpenGlImageColor);
DJV_DEBUG_OPERATOR(djvOpenGlImageLevels);
DJV_DEBUG_OPERATOR(djvOpenGlImageDisplayProfile);
DJV_DEBUG_OPERATOR(djvOpenGlImageFilter);
DJV_DEBUG_OPERATOR(djvOpenGlImageFilter::FILTER);
DJV_DEBUG_OPERATOR(djvOpenGlImageOptions);
DJV_DEBUG_OPERATOR(djvOpenGlImageOptions::CHANNEL);

//@} // djvCoreOpenGL

