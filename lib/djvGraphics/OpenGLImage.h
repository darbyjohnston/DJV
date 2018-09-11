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

#include <djvGraphics/Color.h>
#include <djvGraphics/ColorProfile.h>
#include <djvGraphics/OpenGL.h>
#include <djvGraphics/PixelData.h>

#include <djvCore/Box.h>
#include <djvCore/Error.h>
#include <djvCore/Matrix.h>

#include <QMetaType>

class djvOpenGLLUT;
class djvOpenGLOffscreenBuffer;
class djvOpenGLTexture;
class djvOpenGLShader;

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

//! \addtogroup djvGraphicsOpenGL
//@{

//------------------------------------------------------------------------------
//! \class djvOpenGLImageXform
//!
//! This class provides OpenGL image transform options.
//------------------------------------------------------------------------------

class djvOpenGLImageXform
{
public:
    djvPixelDataInfo::Mirror mirror;
    glm::vec2                position = glm::vec2(0.f, 0.f);
    glm::vec2                scale    = glm::vec2(1.f, 1.f);
    float                    rotate   = 0.f;

    //! Create a transform matrix.
    static glm::mat4x4 xformMatrix(const djvOpenGLImageXform &);
};

//------------------------------------------------------------------------------
//! \class djvOpenGLImageColor
//!
//! This class provides OpenGL image color options.
//------------------------------------------------------------------------------

class djvOpenGLImageColor
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
    static glm::mat4x4 colorMatrix(const djvOpenGLImageColor &);
};

//------------------------------------------------------------------------------
//! \class djvOpenGLImageLevels
//!
//! This class provides OpenGL image color levels options.
//------------------------------------------------------------------------------

class djvOpenGLImageLevels
{
public:
    float inLow   = 0.f;
    float inHigh  = 1.f;
    float gamma   = 1.f;
    float outLow  = 0.f;
    float outHigh = 1.f;

    //! Create a lookup table from color levels.
    static djvPixelData colorLut(const djvOpenGLImageLevels &, float softClip);
};

//------------------------------------------------------------------------------
//! \class djvOpenGLImageDisplayProfile
//!
//! This class provides OpenGL image display profile options.
//------------------------------------------------------------------------------

class djvOpenGLImageDisplayProfile
{
public:
    djvPixelData         lut;
    djvOpenGLImageColor  color;
    djvOpenGLImageLevels levels;
    float                softClip = 0.f;
};

//------------------------------------------------------------------------------
//! \class djvOpenGLImageFilter
//!
//! This class provides OpenGL image filtering options.
//------------------------------------------------------------------------------

class djvOpenGLImageFilter
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

    djvOpenGLImageFilter();
    djvOpenGLImageFilter(FILTER min, FILTER mag);

    FILTER min;
    FILTER mag;

    //! Convert an image filter to OpenGL.
    static GLenum toGl(FILTER);

    //! Get the default image filter.
    static djvOpenGLImageFilter filterDefault();

    //! Get the high quality image filter.
    static const djvOpenGLImageFilter & filterHighQuality();

    //! Get the global image filter.
    static const djvOpenGLImageFilter & filter();

    //! Set the global image filter.
    static void setFilter(const djvOpenGLImageFilter &);
};

//------------------------------------------------------------------------------
//! \class djvOpenGLImageOptions
//!
//! This class provides OpenGL image options.
//------------------------------------------------------------------------------

class djvOpenGLImageOptions
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

    djvOpenGLImageXform          xform;
    djvColorProfile              colorProfile;
    djvOpenGLImageDisplayProfile displayProfile;
    CHANNEL                      channel        = CHANNEL_DEFAULT;
    djvOpenGLImageFilter         filter;
    djvColor                     background;
    bool                         proxyScale     = true;
};

//------------------------------------------------------------------------------
//! \class djvOpenGLImageMesh
//!
//! This class provides a mesh for drawing images.
//------------------------------------------------------------------------------

class djvOpenGLImageMesh
{
public:
    djvOpenGLImageMesh();

    ~djvOpenGLImageMesh();

    void setSize(const glm::ivec2&, const djvPixelDataInfo::Mirror & mirror = djvPixelDataInfo::Mirror(), int proxyScale = 1);
    void draw();

private:
    size_t _vertexSize = 0;
    GLuint _vbo = 0;
    GLuint _vao = 0;
};

//------------------------------------------------------------------------------
//! \class djvOpenGLImageState
//!
//! This class provides OpenGL image state.
//------------------------------------------------------------------------------

class djvOpenGLImageState
{
public:
    djvOpenGLImageState();
    ~djvOpenGLImageState();

private:
    DJV_PRIVATE_COPY(djvOpenGLImageState);

    bool                  _init;
    djvPixelDataInfo      _info;
    djvOpenGLImageOptions _options;
    djvOpenGLTexture *    _texture = nullptr;
    djvOpenGLShader *     _shader = nullptr;
    djvOpenGLTexture *    _scaleXContrib = nullptr;
    djvOpenGLTexture *    _scaleYContrib = nullptr;
    djvOpenGLShader *     _scaleXShader = nullptr;
    djvOpenGLShader *     _scaleYShader = nullptr;
    djvOpenGLLUT *        _lutColorProfile = nullptr;
    djvOpenGLLUT *        _lutDisplayProfile = nullptr;
    djvOpenGLImageMesh *  _mesh = nullptr;

    friend class djvOpenGLImage;
};

//------------------------------------------------------------------------------
//! \class djvOpenGLImage
//!
//! This class provides OpenGL image utilities.
//------------------------------------------------------------------------------

class djvOpenGLImage
{
public:
    virtual ~djvOpenGLImage() = 0;

    //! Draw pixel data.
    static void draw(
        const djvPixelData &          data,
        const djvOpenGLImageOptions & options = djvOpenGLImageOptions(),
        djvOpenGLImageState *         state = 0) throw (djvError);

    //! Read pixel data.
    static void read(djvPixelData &);

    //! Read pixel data.
    static void read(djvPixelData &, const djvBox2i &);

    //! Copy pixel data.
    static void copy(
        const djvPixelData &          input,
        djvPixelData &                output,
        const djvOpenGLImageOptions & options = djvOpenGLImageOptions(),
        djvOpenGLImageState *         state   = 0,
        djvOpenGLOffscreenBuffer *    buffer  = 0) throw (djvError);

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

DJV_COMPARISON_OPERATOR(djvOpenGLImageXform);
DJV_COMPARISON_OPERATOR(djvOpenGLImageColor);
DJV_COMPARISON_OPERATOR(djvOpenGLImageLevels);
DJV_COMPARISON_OPERATOR(djvOpenGLImageDisplayProfile);
DJV_COMPARISON_OPERATOR(djvOpenGLImageFilter);
DJV_COMPARISON_OPERATOR(djvOpenGLImageOptions);

DJV_STRING_OPERATOR(djvOpenGLImageXform);
DJV_STRING_OPERATOR(djvOpenGLImageColor);
DJV_STRING_OPERATOR(djvOpenGLImageLevels);
DJV_STRING_OPERATOR(djvOpenGLImageFilter);
DJV_STRING_OPERATOR(djvOpenGLImageFilter::FILTER);
DJV_STRING_OPERATOR(djvOpenGLImageOptions::CHANNEL);

DJV_DEBUG_OPERATOR(djvOpenGLImageXform);
DJV_DEBUG_OPERATOR(djvOpenGLImageColor);
DJV_DEBUG_OPERATOR(djvOpenGLImageLevels);
DJV_DEBUG_OPERATOR(djvOpenGLImageDisplayProfile);
DJV_DEBUG_OPERATOR(djvOpenGLImageFilter);
DJV_DEBUG_OPERATOR(djvOpenGLImageFilter::FILTER);
DJV_DEBUG_OPERATOR(djvOpenGLImageOptions);
DJV_DEBUG_OPERATOR(djvOpenGLImageOptions::CHANNEL);

//@} // djvGraphicsOpenGL

