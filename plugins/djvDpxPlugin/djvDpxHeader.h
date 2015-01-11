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

//! \file djvDpxHeader.h

#ifndef DJV_DPX_HEADER_H
#define DJV_DPX_HEADER_H

#include <djvDpxPlugin.h>

#include <djvFileIo.h>

//! \addtogroup djvDpxPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvDpxHeader
//!
//! This class proivdes DPX file header functionality.
//------------------------------------------------------------------------------

class djvDpxHeader
{
public:

    //! The DPX file magic numbers.
    
    static const char magic [][5];

    //! This enumeration provides the image orientation.

    enum ORIENT
    {
        ORIENT_LEFT_RIGHT_TOP_BOTTOM,
        ORIENT_RIGHT_LEFT_TOP_BOTTOM,
        ORIENT_LEFT_RIGHT_BOTTOM_TOP,
        ORIENT_RIGHT_LEFT_BOTTOM_TOP,
        ORIENT_TOP_BOTTOM_LEFT_RIGHT,
        ORIENT_TOP_BOTTOM_RIGHT_LEFT,
        ORIENT_BOTTOM_TOP_LEFT_RIGHT,
        ORIENT_BOTTOM_TOP_RIGHT_LEFT
    };

    //! This enumeration provides the descriptor.

    enum DESCRIPTOR
    {
        DESCRIPTOR_USER            = 0,
        DESCRIPTOR_R               = 1,
        DESCRIPTOR_G               = 2,
        DESCRIPTOR_B               = 3,
        DESCRIPTOR_A               = 4,
        DESCRIPTOR_L               = 6,
        DESCRIPTOR_CHROMA          = 7,
        DESCRIPTOR_Z               = 8,
        DESCRIPTOR_COMPOSITE_VIDEO = 9,
        DESCRIPTOR_RGB             = 50,
        DESCRIPTOR_RGBA            = 51,
        DESCRIPTOR_ABGR            = 52,
        DESCRIPTOR_CBYCRY          = 100,
        DESCRIPTOR_CBYACRYA        = 101,
        DESCRIPTOR_CBYCR           = 102,
        DESCRIPTOR_CBYCRA          = 103,
        DESCRIPTOR_USER_2          = 150,
        DESCRIPTOR_USER_3          = 151,
        DESCRIPTOR_USER_4          = 152,
        DESCRIPTOR_USER_5          = 153,
        DESCRIPTOR_USER_6          = 154,
        DESCRIPTOR_USER_7          = 155,
        DESCRIPTOR_USER_8          = 156
    };

    //! This enumeration provides the transfer information.

    enum TRANSFER
    {
        TRANSFER_USER,
        TRANSFER_FILM_PRINT,
        TRANSFER_LINEAR,
        TRANSFER_LOG,
        TRANSFER_VIDEO,
        TRANSFER_SMPTE_274M,
        TRANSFER_ITU_R_709_4,
        TRANSFER_ITU_R_601_5_B_OR_G,
        TRANSFER_ITU_R_601_5_M,
        TRANSFER_NTSC,
        TRANSFER_PAL,
        TRANSFER_Z,
        TRANSFER_Z_HOMOGENEOUS
    };

    //! This enumeration provides the colorimetric information for vesion
    //! 1.0.

    enum COLORIMETRIC_1_0
    {
        COLORIMETRIC_1_0_USER               = 0,
        COLORIMETRIC_1_0_FILM_PRINT         = 1,
        COLORIMETRIC_1_0_VIDEO              = 4,
        COLORIMETRIC_1_0_SMPTE_240M         = 5,
        COLORIMETRIC_1_0_ITU_R_709_1        = 6,
        COLORIMETRIC_1_0_ITU_R_601_2_B_OR_G = 7,
        COLORIMETRIC_1_0_ITU_R_601_2_M      = 8,
        COLORIMETRIC_1_0_NTSC               = 9,
        COLORIMETRIC_1_0_PAL                = 10
    };

    //! This enumeration provides the colorimetric information vesion 2.0.

    enum COLORIMETRIC_2_0
    {
        COLORIMETRIC_2_0_USER               = 0,
        COLORIMETRIC_2_0_FILM_PRINT         = 1,
        COLORIMETRIC_2_0_VIDEO              = 4,
        COLORIMETRIC_2_0_SMPTE_274M         = 5,
        COLORIMETRIC_2_0_ITU_R_709_4        = 6,
        COLORIMETRIC_2_0_ITU_R_601_5_B_OR_G = 7,
        COLORIMETRIC_2_0_ITU_R_601_5_M      = 8,
        COLORIMETRIC_2_0_NTSC               = 9,
        COLORIMETRIC_2_0_PAL                = 10
    };

    //! This enumeration provides how the data components are ordered.

    enum COMPONENTS
    {
        PACK,
        TYPE_A,
        TYPE_B
    };

    //! This struct provides file information.

    struct File
    {
        quint32 magic;
        quint32 imageOffset;
        char    version [8];
        quint32 size;
        quint32 dittoKey;
        quint32 headerSize;
        quint32 industryHeaderSize;
        quint32 userHeaderSize;
        char    name [100];
        char    time [24];
        char    creator [100];
        char    project [200];
        char    copyright [200];
        quint32 encryptionKey;
        quint8  pad [104];

    };

    //! This struct provides image information.

    struct Image
    {
        quint16 orient;
        quint16 elemSize;
        quint32 size [2];

        //! This struct provides image element information.

        struct Elem
        {
            quint32 dataSign;
            quint32 lowData;
            float   lowQuantity;
            quint32 highData;
            float   highQuantity;
            quint8  descriptor;
            quint8  transfer;
            quint8  colorimetric;
            quint8  bitDepth;
            quint16 packing;
            quint16 encoding;
            quint32 dataOffset;
            quint32 linePadding;
            quint32 elemPadding;
            char    description [32];

        };
        
        Elem   elem [8];
        quint8 pad [52];

    };

    //! This struct provides source information.

    struct Source
    {
        quint32 offset [2];
        float   center [2];
        quint32 size [2];
        char    file [100];
        char    time [24];
        char    inputDevice [32];
        char    inputSerial [32];
        quint16 border [4];
        quint32 pixelAspect [2];
        float   scanSize [2]; // V2.0
        quint8  pad [20];

    };

    //! This struct provides film information.

    struct Film
    {
        char    id [2];
        char    type [2];
        char    offset [2];
        char    prefix [6];
        char    count [4];
        char    format [32];
        quint32 frame;
        quint32 sequence;
        quint32 hold;
        float   frameRate;
        float   shutter;
        char    frameId [32];
        char    slate [100];
        quint8  pad [56];

    };

    //! This struct provides TV information.

    struct Tv
    {
        quint32 timecode;
        quint32 userBits;
        quint8  interlace;
        quint8  field;
        quint8  videoSignal;
        quint8  pad;
        float   sampleRate [2];
        float   frameRate;
        float   timeOffset;
        float   gamma;
        float   blackLevel;
        float   blackGain;
        float   breakpoint;
        float   whiteLevel;
        float   integrationTimes;
        quint8  pad2 [76];

    };

    //! Constructor.

    djvDpxHeader();

    File   file;
    Image  image;
    Source source;
    Film   film;
    Tv     tv;

    //! Load the header.

    void load(
        djvFileIo &,
        djvImageIoInfo &,
        bool & filmPrint) throw (djvError);

    //! Save the header.

    void save(
        djvFileIo &,
        const djvImageIoInfo &,
        djvDpxPlugin::ENDIAN,
        djvCineon::COLOR_PROFILE,
        djvDpxPlugin::VERSION) throw (djvError);

    //! Update the header when saving is finished.
    
    void saveEnd(djvFileIo &) throw (djvError);

    //! Zero memory.

    static void zero(char *, int size);

    //! Get whether the value is valid.

    static bool isValid(const quint8 *);

    //! Get whether the value is valid.

    static bool isValid(const quint16 *);

    //! Get whether the value is valid.

    static bool isValid(const quint32 *);

    //! Get whether the value is valid.

    static bool isValid(const float *);

    //! Get whether the value is valid.

    static bool isValid(const char *, int size);

    //! Get whether the value is valid.

    static QString toString(const char *, int size);

    QString debug() const;

    static QString debug(quint8);
    static QString debug(quint16);
    static QString debug(quint32);
    static QString debug(float);
    static QString debug(const char *, int size);

private:

    void endian();
};

//@} // djvDpxPlugin

#endif // DJV_DPX_HEADER_H

