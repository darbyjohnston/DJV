//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

class djvOpenGlImageLut;
class djvOpenGlImageShader;
class djvOpenGlImageTexture;
class djvOpenGlOffscreenBuffer;

//! \addtogroup djvCoreOpenGL
//@{

//------------------------------------------------------------------------------
//! \struct djvOpenGlImageXform
//!
//! This struct provides OpenGL image transform options.
//------------------------------------------------------------------------------

struct DJV_CORE_EXPORT djvOpenGlImageXform
{
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
//! \struct djvOpenGlImageColor
//!
//! This struct provides OpenGL image color options.
//------------------------------------------------------------------------------

struct DJV_CORE_EXPORT djvOpenGlImageColor
{
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
//! \struct djvOpenGlImageLevels
//!
//! This struct provides OpenGL image color levels options.
//------------------------------------------------------------------------------

struct DJV_CORE_EXPORT djvOpenGlImageLevels
{
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
//! \struct djvOpenGlImageDisplayProfile
//!
//! This struct provides OpenGL image display profile options.
//------------------------------------------------------------------------------

struct DJV_CORE_EXPORT djvOpenGlImageDisplayProfile
{
    //! Constructor.

    djvOpenGlImageDisplayProfile();

    djvPixelData         lut;
    djvOpenGlImageColor  color;
    djvOpenGlImageLevels levels;
    double               softClip;
};

//------------------------------------------------------------------------------
//! \struct djvOpenGlImageFilter
//!
//! This struct provides OpenGL image filtering options.
//------------------------------------------------------------------------------

struct DJV_CORE_EXPORT djvOpenGlImageFilter
{
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

    //! Get the image filtering labels.

    static const QStringList & filterLabels();

    //! Constructor.

    djvOpenGlImageFilter();

    //! Constructor.

    djvOpenGlImageFilter(FILTER min, FILTER mag);

    FILTER min;
    FILTER mag;

    //! Convert a filter to OpenGL.

    static GLuint gl(FILTER);

    //! Get the default filter.

    static const djvOpenGlImageFilter & filterDefault();

    //! Get the high quality filter.

    static const djvOpenGlImageFilter & filterHighQuality();

    //! Get the global filter.

    static const djvOpenGlImageFilter & filterGlobal();

    //! Set the global filter.

    static void setFilterGlobal(const djvOpenGlImageFilter &);
};

//------------------------------------------------------------------------------
//! \struct djvOpenGlImageOptions
//!
//! This struct provides OpenGL image options.
//------------------------------------------------------------------------------

struct DJV_CORE_EXPORT djvOpenGlImageOptions
{
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
//! \struct djvOpenGlImageState
//!
//! This struct provides OpenGL image state.
//------------------------------------------------------------------------------

struct DJV_CORE_EXPORT djvOpenGlImageState
{
    //! Constructor.

    djvOpenGlImageState();

    //! Destructor.

    ~djvOpenGlImageState();

private:

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
    //! \todo GPU implementation.

    static void average(
        const djvPixelData &   input,
        djvColor &             output,
        const djvPixel::Mask & mask   = djvPixel::Mask()) throw (djvError);

    //! Calculate the histogram.
    //!
    //! \todo GPU implementation.

    static void histogram(
        const djvPixelData &   input,
        djvPixelData &         output,
        int                    size,
        djvColor &             min,
        djvColor &             max,
        const djvPixel::Mask & mask = djvPixel::Mask()) throw (djvError);
    
    //! Get a pixel.
    
    static djvColor pixel(const djvPixelData &, int x, int y);
};

//------------------------------------------------------------------------------

#if defined(DJV_WINDOWS)

DJV_CORE_EXPORT_TEMPLATE
template class DJV_CORE_EXPORT QVector<djvOpenGlImageFilter::FILTER>;
DJV_CORE_EXPORT_TEMPLATE
template class DJV_CORE_EXPORT QVector<djvOpenGlImageOptions::CHANNEL>;

#endif // DJV_WINDOWS

DJV_CORE_EXPORT bool operator == (
    const djvOpenGlImageXform &, const djvOpenGlImageXform &);
DJV_CORE_EXPORT bool operator == (
    const djvOpenGlImageColor &, const djvOpenGlImageColor &);
DJV_CORE_EXPORT bool operator == (
    const djvOpenGlImageLevels &, const djvOpenGlImageLevels &);
DJV_CORE_EXPORT bool operator == (
    const djvOpenGlImageDisplayProfile &, const djvOpenGlImageDisplayProfile &);
DJV_CORE_EXPORT bool operator == (
    const djvOpenGlImageFilter &, const djvOpenGlImageFilter &);
DJV_CORE_EXPORT bool operator == (
    const djvOpenGlImageOptions &, const djvOpenGlImageOptions &);

DJV_CORE_EXPORT bool operator != (
    const djvOpenGlImageXform &, const djvOpenGlImageXform &);
DJV_CORE_EXPORT bool operator != (
    const djvOpenGlImageColor &, const djvOpenGlImageColor &);
DJV_CORE_EXPORT bool operator != (
    const djvOpenGlImageLevels &, const djvOpenGlImageLevels &);
DJV_CORE_EXPORT bool operator != (
    const djvOpenGlImageDisplayProfile &, const djvOpenGlImageDisplayProfile &);
DJV_CORE_EXPORT bool operator != (
    const djvOpenGlImageFilter &, const djvOpenGlImageFilter &);
DJV_CORE_EXPORT bool operator != (
    const djvOpenGlImageOptions &, const djvOpenGlImageOptions &);

DJV_CORE_EXPORT QStringList & operator >> (QStringList &,
    djvOpenGlImageXform &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &,
    djvOpenGlImageColor &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &,
    djvOpenGlImageLevels &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &,
    djvOpenGlImageFilter &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &,
    djvOpenGlImageFilter::FILTER &) throw (QString);
DJV_CORE_EXPORT QStringList & operator >> (QStringList &,
    djvOpenGlImageOptions::CHANNEL &) throw (QString);

DJV_CORE_EXPORT QStringList & operator << (QStringList &,
    const djvOpenGlImageXform &);
DJV_CORE_EXPORT QStringList & operator << (QStringList &,
    const djvOpenGlImageColor &);
DJV_CORE_EXPORT QStringList & operator << (QStringList &,
    const djvOpenGlImageLevels &);
DJV_CORE_EXPORT QStringList & operator << (QStringList &,
    const djvOpenGlImageFilter &);
DJV_CORE_EXPORT QStringList & operator << (QStringList &,
    djvOpenGlImageFilter::FILTER);
DJV_CORE_EXPORT QStringList & operator << (QStringList &,
    djvOpenGlImageOptions::CHANNEL);

DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, const djvOpenGlImageXform &);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, const djvOpenGlImageColor &);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, const djvOpenGlImageLevels &);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, const djvOpenGlImageDisplayProfile &);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, const djvOpenGlImageFilter &);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, djvOpenGlImageFilter::FILTER);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, const djvOpenGlImageOptions &);
DJV_CORE_EXPORT djvDebug & operator << (djvDebug &, djvOpenGlImageOptions::CHANNEL);

//@} // djvCoreOpenGL

#endif // DJV_OPEN_GL_IMAGE_H

