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

//! \file djvOpenGlImage.h

#ifndef DJV_OPEN_GL_IMAGE_H
#define DJV_OPEN_GL_IMAGE_H

#include <djvBox.h>
#include <djvColor.h>
#include <djvColorProfile.h>
#include <djvError.h>
#include <djvOpenGl.h>
#include <djvMatrix.h>
#include <djvPixelData.h>

#include <QMetaType>

class djvOpenGlImageLut;
class djvOpenGlImageShader;
class djvOpenGlImageTexture;
class djvOpenGlOffscreenBuffer;

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

class DJV_CORE_EXPORT djvOpenGlImageXform
{
public:

    //! Constructor.

    djvOpenGlImageXform();

    djvPixelDataInfo::Mirror mirror;
    djvVector2f              position;
    djvVector2f              scale;
    double                   rotate;

    //! Create a transform matrix.

    static djvMatrix3f xformMatrix(const djvOpenGlImageXform &);
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImageColor
//!
//! This class provides OpenGL image color options.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlImageColor
{
public:

    //! Constructor.

    djvOpenGlImageColor();

    double brightness;
    double contrast;
    double saturation;

    //! Create a brightness matrix.

    static djvMatrix4f brightnessMatrix(double r, double g, double b);

    //! Create a contrast matrix.

    static djvMatrix4f contrastMatrix(double r, double g, double b);

    //! Create a saturation matrix.

    static djvMatrix4f saturationMatrix(double r, double g, double b);

    //! Create a color matrix.

    static djvMatrix4f colorMatrix(const djvOpenGlImageColor &);
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImageLevels
//!
//! This class provides OpenGL image color levels options.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlImageLevels
{
public:

    //! Constructor.

    djvOpenGlImageLevels();

    double inLow;
    double inHigh;
    double gamma;
    double outLow;
    double outHigh;

    //! Create a lookup table from color levels.

    static djvPixelData colorLut(const djvOpenGlImageLevels &, double softClip);
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImageDisplayProfile
//!
//! This class provides OpenGL image display profile options.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlImageDisplayProfile
{
public:

    //! Constructor.

    djvOpenGlImageDisplayProfile();

    djvPixelData         lut;
    djvOpenGlImageColor  color;
    djvOpenGlImageLevels levels;
    double               softClip;
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImageFilter
//!
//! This class provides OpenGL image filtering options.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlImageFilter
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

    //! Constructor.

    djvOpenGlImageFilter();

    //! Constructor.

    djvOpenGlImageFilter(FILTER min, FILTER mag);

    FILTER min;
    FILTER mag;

    //! Convert an image filter to OpenGL.

    static GLuint toGl(FILTER);

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

class DJV_CORE_EXPORT djvOpenGlImageOptions
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

    //! Constructor.

    djvOpenGlImageOptions();

    djvOpenGlImageXform          xform;
    djvColorProfile              colorProfile;
    djvOpenGlImageDisplayProfile displayProfile;
    CHANNEL                      channel;
    djvOpenGlImageFilter         filter;
    djvColor                     background;
    bool                         proxyScale;
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImageState
//!
//! This class provides OpenGL image state.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlImageState
{
public:

    //! Constructor.

    djvOpenGlImageState();

    //! Destructor.

    ~djvOpenGlImageState();

private:

    DJV_PRIVATE_COPY(djvOpenGlImageState);

    bool                    _init;
    djvPixelDataInfo        _info;
    djvOpenGlImageOptions   _options;
    djvOpenGlImageTexture * _texture;
    djvOpenGlImageShader  * _shader;
    djvOpenGlImageTexture * _scaleXContrib;
    djvOpenGlImageTexture * _scaleYContrib;
    djvOpenGlImageShader  * _scaleXShader;
    djvOpenGlImageShader  * _scaleYShader;
    djvOpenGlImageLut *     _lutColorProfile;
    djvOpenGlImageLut *     _lutDisplayProfile;

    friend class djvOpenGlImage;
};

//------------------------------------------------------------------------------
//! \class djvOpenGlImage
//!
//! This class provides OpenGL image utilities.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvOpenGlImage
{
public:

    //! Destructor.
    
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
        const djvVector2i &      offset = djvVector2i());

    //! Setup OpenGL state for image reading.

    static void statePack(
        const djvPixelDataInfo & info,
        const djvVector2i &      offset = djvVector2i());

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
        ERROR_CREATE_SHADER,
        ERROR_COMPILE_SHADER,
        ERROR_CREATE_PROGRAM,
        ERROR_LINK_PROGRAM,
            
        ERROR_COUNT
    };
    
    //! Get the error code labels.
    
    static const QStringList & errorLabels();
};

//------------------------------------------------------------------------------

DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageXform);
DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageColor);
DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageLevels);
DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageDisplayProfile);
DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageFilter);
DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageOptions);

DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageXform);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageColor);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageLevels);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageFilter);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageFilter::FILTER);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageOptions::CHANNEL);

DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageXform);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageColor);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageLevels);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageDisplayProfile);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageFilter);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageFilter::FILTER);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageOptions);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvOpenGlImageOptions::CHANNEL);

//@} // djvCoreOpenGL

#endif // DJV_OPEN_GL_IMAGE_H

