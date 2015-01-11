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

//! \file djvColor.h

#ifndef DJV_COLOR_H
#define DJV_COLOR_H

#include <djvPixel.h>

#include <QMetaType>

//! \addtogroup djvCoreImage
//@{

//------------------------------------------------------------------------------
//! \class djvColor
//!
//! This class provides a color.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvColor
{
public:

    //! Constructor.

    djvColor();

    //! Constructor.

    djvColor(const djvColor &);

    //! Constructor.

    djvColor(djvPixel::PIXEL);

    //! Constructor.

    djvColor(djvPixel::F32_T l);

    //! Constructor.

    djvColor(djvPixel::F32_T l, djvPixel::F32_T a);

    //! Constructor.

    djvColor(djvPixel::F32_T r, djvPixel::F32_T g, djvPixel::F32_T b);

    //! Constructor.

    djvColor(djvPixel::F32_T r, djvPixel::F32_T g, djvPixel::F32_T b, djvPixel::F32_T a);

    //! Constructor.

    djvColor(const quint8 *, djvPixel::PIXEL);

    //! Get the pixel type.

    inline djvPixel::PIXEL pixel() const;

    //! Set the pixel type.

    void setPixel(djvPixel::PIXEL);

    //! Get the channel couunt.

    inline int channels() const;

    //! Get a channel.

    inline djvPixel::U8_T u8(int) const;

    //! Get a channel.

    inline djvPixel::U10_T u10(int) const;

    //! Get a channel.

    inline djvPixel::U16_T u16(int) const;

    //! Get a channel.

    inline djvPixel::F16_T f16(int) const;

    //! Get a channel.

    inline djvPixel::F32_T f32(int) const;

    //! Set a channel.

    inline void setU8(djvPixel::U8_T, int);

    //! Set a channel.

    inline void setU10(djvPixel::U10_T, int);

    //! Set a channel.

    inline void setU16(djvPixel::U16_T, int);

    //! Set a channel.

    inline void setF16(djvPixel::F16_T, int);

    //! Set a channel.

    inline void setF32(djvPixel::F32_T, int);

    //! Zero the channels.

    void zero();

    //! Get a pointer to the color data.

    inline quint8 * data();

    //! Get a const pointer to the color data.

    inline const quint8 * data() const;

    djvColor & operator = (const djvColor &);

private:

    void init();

    djvPixel::PIXEL _pixel;
    int             _channels;
    quint8          _data[djvPixel::channelsMax * djvPixel::bytesMax];
};

//------------------------------------------------------------------------------

Q_DECLARE_METATYPE(djvColor)

DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvColor);

DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvColor);

DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvColor);

//@} // djvCoreImage

#include <djvColorInline.h>

#endif // DJV_COLOR_H

