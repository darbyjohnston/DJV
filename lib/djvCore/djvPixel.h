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

//! \file djvPixel.h

#ifndef DJV_IMAGE_PIXEL_H
#define DJV_IMAGE_PIXEL_H

#include <djvDebug.h>
#include <djvStringUtil.h>

#include <QMetaType>

#include <half.h>

//! \addtogroup djvCoreImage
//@{

//------------------------------------------------------------------------------
//! \class djvPixel
//!
//! This class provides pixel functionality.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvPixel
{
    Q_GADGET
    Q_ENUMS(FORMAT)
    Q_ENUMS(TYPE)
    Q_ENUMS(DATA)
    Q_ENUMS(PIXEL)
    
public:

    //! Destructor.
    
    virtual ~djvPixel() = 0;

    //! This enumeration provides the pixel format.

    enum FORMAT
    {
        L,     //!< Luminance
        LA,    //!< Luminance, alpha
        RGB,   //!< Red, green, blue
        RGBA,  //!< Red, green, blue, alpha

        FORMAT_COUNT
    };

    //! Get the pixel format labels.

    static const QStringList & formatLabels();

    //! This enumeration provides the pixel type.

    enum TYPE
    {
        U8,   //!< 8-bit unsigned
        U10,  //!< 10-bit unsigned
        U16,  //!< 16-bit unsigned
        F16,  //!< 16-bit floating-point
        F32,  //!< 32-bit floating-point

        TYPE_COUNT
    };

    //! Get the pixel type labels.

    static const QStringList & typeLabels();
    
    //! This enumeration provides whether the pixel has integer or floating
    //! point data.
    
    enum DATA
    {
        INTEGER,
        FLOAT,
        
        DATA_COUNT
    };
    
    //! Get the data labels.
    
    static const QStringList & dataLabels();

    typedef quint8  U8_T;
    typedef quint16 U10_T;
    typedef quint16 U16_T;
    typedef half    F16_T;
    typedef float   F32_T;

    //! This struct provides 10-bit MSB data.

    struct U10_S_MSB
    {
        uint r: 10, g: 10, b: 10, pad: 2;
    };

    //! This struct provides 10-bit LSB data.

    struct U10_S_LSB
    {
        uint pad: 2, b: 10, g: 10, r: 10;
    };

#if defined(DJV_MSB)
    typedef U10_S_MSB U10_S;
#else
    typedef U10_S_LSB U10_S;
#endif

    static const int channelsMax = 4;
    static const int bytesMax    = 4;

    static const int u8Max  = 255;
    static const int u10Max = 1023;
    static const int u12Max = 4095;
    static const int u16Max = 65535;

    //! This enumeration provides the pixels.

    enum PIXEL
    {
        L_U8,
        L_U16,
        L_F16,
        L_F32,
        LA_U8,
        LA_U16,
        LA_F16,
        LA_F32,
        RGB_U8,
        RGB_U10,
        RGB_U16,
        RGB_F16,
        RGB_F32,
        RGBA_U8,
        RGBA_U16,
        RGBA_F16,
        RGBA_F32,

        PIXEL_COUNT
    };
    
    //! Get the pixel labels.

    static const QStringList & pixelLabels();

    //! Get the pixel format.

    static inline FORMAT format(PIXEL);

    //! Get the format for the given number of channels.

    static inline bool format(int channels, FORMAT &);

    //! Get the pixel type.

    static inline TYPE type(PIXEL);
    
    //! Get the pixel type for the given bit depth.
    
    static inline bool type(int bitDepth, DATA, TYPE &);

    //! Get the number of channels in a format.

    static inline int channels(FORMAT);

    //! Get the number of channels in a pixel.

    static inline int channels(PIXEL);

    //! Get the number of bytes in a channel.

    static inline int channelByteCount(PIXEL);

    //! Get the number of bytes in a pixel.

    static inline int byteCount(PIXEL);

    //! Get the bit depth of a type.

    static inline int bitDepth(TYPE);

    //! Get the bit depth of a pixel.

    static inline int bitDepth(PIXEL);

    //! Get the maximum value a pixel can hold (1 is returned for floating
    //! point formats.

    static inline int max(PIXEL);

    //! Create a pixel from the given format and type. If the combination is
    //! not compatible the next suitable pixel is used.

    static inline PIXEL pixel(FORMAT, TYPE);

    //! Create a pixel from the given format and type. If the combination is
    //! not compatible then false is returned.

    static inline bool pixel(FORMAT, TYPE, PIXEL &);

    //! Create a pixel from the given number of channels and bit depth. If the
    //! combination is not compatible then false is returned.

    static inline bool pixel(int channels, int bitDepth, DATA, PIXEL &);
    
    //! This struct provides a pixel mask.
    
    struct Mask
    {
        //! Constructor.

        inline Mask();
        
        //! Constructor.
        
        explicit inline Mask(bool);
        
        //! Constructor.
        
        inline Mask(bool, bool, bool, bool = true);
        
        //! Constructor.

        explicit inline Mask(const bool [channelsMax]);
        
        //! Constructor.

        inline Mask(const Mask &);
        
        //! Mask data.

        bool mask [channelsMax];
        
        inline Mask & operator = (const Mask &);
        
        inline bool operator [] (int) const;
        
        inline bool & operator [] (int);
    };

    //! Convert type data.

    static inline U10_T u8ToU10(U8_T);

    //! Convert type data.

    static inline U16_T u8ToU16(U8_T);

    //! Convert type data.

    static inline F16_T u8ToF16(U8_T);

    //! Convert type data.

    static inline F32_T u8ToF32(U8_T);

    //! Convert type data.

    static inline U8_T  u10ToU8 (U10_T);

    //! Convert type data.

    static inline U16_T u10ToU16(U10_T);

    //! Convert type data.

    static inline F16_T u10ToF16(U10_T);

    //! Convert type data.

    static inline F32_T u10ToF32(U10_T);

    //! Convert type data.

    static inline U8_T  u16ToU8 (U16_T);

    //! Convert type data.

    static inline U10_T u16ToU10(U16_T);

    //! Convert type data.

    static inline F16_T u16ToF16(U16_T);

    //! Convert type data.

    static inline F32_T u16ToF32(U16_T);

    //! Convert type data.

    static inline U8_T  f16ToU8 (F16_T);

    //! Convert type data.

    static inline U10_T f16ToU10(F16_T);

    //! Convert type data.

    static inline U16_T f16ToU16(F16_T);

    //! Convert type data.

    static inline F32_T f16ToF32(F16_T);

    //! Convert type data.

    static inline U8_T  f32ToU8 (F32_T);

    //! Convert type data.

    static inline U10_T f32ToU10(F32_T);

    //! Convert type data.

    static inline U16_T f32ToU16(F32_T);

    //! Convert type data.

    static inline F16_T f32ToF16(F32_T);

    //! Convert pixel data.

    static void convert(
        const void * in,
        PIXEL        inPixel,
        void *       out,
        PIXEL        outPixel,
        int          size     = 1,
        int          stride   = 1,
        bool         bgr      = false);
};

//------------------------------------------------------------------------------

#define PIXEL_U8_ZERO  0
#define PIXEL_U10_ZERO 0
#define PIXEL_U16_ZERO 0
#define PIXEL_F16_ZERO 0.0
#define PIXEL_F32_ZERO 0.0

#define PIXEL_U8_ONE   djvPixel::u8Max
#define PIXEL_U10_ONE  djvPixel::u10Max
#define PIXEL_U16_ONE  djvPixel::u16Max
#define PIXEL_F16_ONE  1.0
#define PIXEL_F32_ONE  1.0

#define PIXEL_U8_TO_U8(IN)   (IN)
#define PIXEL_U8_TO_U10(IN)  djvPixel::u8ToU10(IN)
#define PIXEL_U8_TO_U16(IN)  djvPixel::u8ToU16(IN)
#define PIXEL_U8_TO_F16(IN)  djvPixel::u8ToF16(IN)
#define PIXEL_U8_TO_F32(IN)  djvPixel::u8ToF32(IN)

#define PIXEL_U10_TO_U8(IN)  djvPixel::u10ToU8(IN)
#define PIXEL_U10_TO_U10(IN) (IN)
#define PIXEL_U10_TO_U16(IN) djvPixel::u10ToU16(IN)
#define PIXEL_U10_TO_F16(IN) djvPixel::u10ToF16(IN)
#define PIXEL_U10_TO_F32(IN) djvPixel::u10ToF32(IN)

#define PIXEL_U16_TO_U8(IN)  djvPixel::u16ToU8(IN)
#define PIXEL_U16_TO_U10(IN) djvPixel::u16ToU10(IN)
#define PIXEL_U16_TO_U16(IN) (IN)
#define PIXEL_U16_TO_F16(IN) djvPixel::u16ToF16(IN)
#define PIXEL_U16_TO_F32(IN) djvPixel::u16ToF32(IN)

#define PIXEL_F16_TO_U8(IN)  djvPixel::f16ToU8(IN)
#define PIXEL_F16_TO_U10(IN) djvPixel::f16ToU10(IN)
#define PIXEL_F16_TO_U16(IN) djvPixel::f16ToU16(IN)
#define PIXEL_F16_TO_F16(IN) (IN)
#define PIXEL_F16_TO_F32(IN) djvPixel::f16ToF32(IN)

#define PIXEL_F32_TO_U8(IN)  djvPixel::f32ToU8(IN)
#define PIXEL_F32_TO_U10(IN) djvPixel::f32ToU10(IN)
#define PIXEL_F32_TO_U16(IN) djvPixel::f32ToU16(IN)
#define PIXEL_F32_TO_F16(IN) djvPixel::f32ToF16(IN)
#define PIXEL_F32_TO_F32(IN) (IN)

inline bool operator == (const djvPixel::Mask &, const djvPixel::Mask &);

inline bool operator != (const djvPixel::Mask &, const djvPixel::Mask &);

DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvPixel::FORMAT);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvPixel::TYPE);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvPixel::DATA);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvPixel::PIXEL);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvPixel::Mask);

DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvPixel::FORMAT);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvPixel::TYPE);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvPixel::DATA);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvPixel::PIXEL);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvPixel::FORMAT);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvPixel::Mask);

//@} // djvCoreImage

#include <djvPixelInline.h>

#endif // DJV_PIXEL_H

