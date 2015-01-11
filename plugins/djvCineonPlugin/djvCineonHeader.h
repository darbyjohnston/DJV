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

//! \file djvCineonHeader.h

#ifndef DJV_CINEON_HEADER_H
#define DJV_CINEON_HEADER_H

#include <djvCineonPlugin.h>

//! \addtogroup djvCineonPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvCineonHeader
//!
//! This class provides Cineon file header functionality.
//------------------------------------------------------------------------------

class djvCineonHeader
{
public:

    //! The Cineon file magic numbers.
    
    static const quint32 magic[];
    
    // This constant is used to catch invalid values.

    static const float minSpeed;

    //! This enumeration provides the image orientation.

    enum ORIENT
    {
        ORIENT_LEFT_RIGHT_TOP_BOTTOM,
        ORIENT_LEFT_RIGHT_BOTTOM_TOP,
        ORIENT_RIGHT_LEFT_TOP_BOTTOM,
        ORIENT_RIGHT_LEFT_BOTTOM_TOP,
        ORIENT_TOP_BOTTOM_LEFT_RIGHT,
        ORIENT_TOP_BOTTOM_RIGHT_LEFT,
        ORIENT_BOTTOM_TOP_LEFT_RIGHT,
        ORIENT_BOTTOM_TOP_RIGHT_LEFT
    };

    //! This enumeration provides the descriptor.

    enum DESCRIPTOR
    {
        DESCRIPTOR_L = 0,
        DESCRIPTOR_R_FILM_PRINT = 1,
        DESCRIPTOR_G_FILM_PRINT = 2,
        DESCRIPTOR_B_FILM_PRINT = 3,
        DESCRIPTOR_R_CCIR_XA11 = 4,
        DESCRIPTOR_G_CCIR_XA11 = 5,
        DESCRIPTOR_B_CCIR_XA11 = 6
    };

    //! This struct provides file information.

    struct File
    {
        quint32 magic;
        quint32 imageOffset;
        quint32 headerSize;
        quint32 industryHeaderSize;
        quint32 userHeaderSize;
        quint32 size;
        char    version [8];
        char    name [100];
        char    time [24];
        quint8  pad[36];
    };

    //! This struct provides image information.

    struct Image
    {
        quint8 orient;
        quint8 channels;
        quint8 pad[2];

        //! This struct provides channel information.

        struct Channel
        {
            quint8  descriptor[2];
            quint8  bitDepth;
            quint8  pad;
            quint32 size[2];
            float   lowData;
            float   lowQuantity;
            float   highData;
            float   highQuantity;

        } channel [8];

        float   white [2];
        float   red [2];
        float   green [2];
        float   blue [2];
        char    label [200];
        quint8  pad2[28];
        quint8  interleave;
        quint8  packing;
        quint8  dataSign;
        quint8  dataSense;
        quint32 linePadding;
        quint32 channelPadding;
        quint8  pad3[20];
    };
    
    //! This struct provides source information.

    struct Source
    {
        qint32 offset [2];
        char   file [100];
        char   time [24];
        char   inputDevice [64];
        char   inputModel [32];
        char   inputSerial [32];
        float  inputPitch [2];
        float  gamma;
        char   pad [40];

    };

    //! This struct provides film information.

    struct Film
    {
        quint8  id;
        quint8  type;
        quint8  offset;
        quint8  pad;
        quint32 prefix;
        quint32 count;
        char    format [32];
        quint32 frame;
        float   frameRate;
        char    frameId [32];
        char    slate [200];
        char    pad2 [740];

    };

    //! Constructor.

    djvCineonHeader();

    File   file;
    Image  image;
    Source source;
    Film   film;

    //! Load the header.

    void load(
        djvFileIo &,
        djvImageIoInfo &,
        bool & filmPrint) throw (djvError);

    //! Save the header.

    void save(
        djvFileIo &,
        const djvImageIoInfo &,
        djvCineon::COLOR_PROFILE) throw (djvError);
    
    //! Update the header when saving is finished.
    
    void saveEnd(djvFileIo &) throw (djvError);

    //! Zero memory.

    static void zero(qint32 *);

    //! Zero memory.

    static void zero(float *);

    //! Zero memory.

    static void zero(char *, int size);

    //! Get whether the value is valid.

    static bool isValid(const quint8 *);

    //! Get whether the value is valid.

    static bool isValid(const quint16 *);

    //! Get whether the value is valid.

    static bool isValid(const quint32 *);

    //! Get whether the value is valid.

    static bool isValid(const qint32 *);

    //! Get whether the value is valid.

    static bool isValid(const float *);

    //! Get whether the value is valid.

    static bool isValid(const char *, int size);

    //! Convert to a string.

    static QString toString(const char *, int size);

    QString debug() const;

    static QString debug(quint8);
    static QString debug(quint16);
    static QString debug(quint32);
    static QString debug(qint32);
    static QString debug(float);
    static QString debug(const char *, int size);

private:

    void endian();
};

//@} // djvCineon

#endif // DJV_CINEON_HEADER_H

