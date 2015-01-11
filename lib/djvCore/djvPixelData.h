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

//! \file djvPixelData.h

#ifndef DJV_PIXEL_DATA_H
#define DJV_PIXEL_DATA_H

#include <djvPixel.h>

#include <djvBox.h>
#include <djvColor.h>
#include <djvMemory.h>
#include <djvMemoryBuffer.h>
#include <djvVector.h>

#include <QMetaType>
#include <QString>

class djvFileIo;

//! \addtogroup djvCoreImage
//@{

//------------------------------------------------------------------------------
//! \class djvPixelDataInfo
//!
//! This class provides pixel data information.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvPixelDataInfo
{
    Q_GADGET
    Q_ENUMS(PROXY)
    
public:

    //! Constructor.

    djvPixelDataInfo();

    //! Constructor.

    djvPixelDataInfo(
        const djvVector2i & size,
        djvPixel::PIXEL     pixel);

    //! Constructor.

    djvPixelDataInfo(
        int             width,
        int             height,
        djvPixel::PIXEL pixel);

    //! Constructor.

    djvPixelDataInfo(
        const QString &     fileName,
        const djvVector2i & size,
        djvPixel::PIXEL     pixel);

    //! Constructor.

    djvPixelDataInfo(
        const QString & fileName,
        int             width,
        int             height,
        djvPixel::PIXEL pixel);

    //! This enumeration provides the proxy scale.

    enum PROXY
    {
        PROXY_NONE,
        PROXY_1_2,
        PROXY_1_4,
        PROXY_1_8,

        PROXY_COUNT
    };

    //! Get the proxy scale labels.

    static const QStringList & proxyLabels();
    
    //! This struct provides mirroring.
    
    struct DJV_CORE_EXPORT Mirror
    {
        Mirror(bool x = false, bool y = false);
        
        bool x;
        bool y;
    };

    QString           fileName;
    QString           layerName;
    djvVector2i       size;
    PROXY             proxy;
    djvPixel::PIXEL   pixel;
    bool              bgr;
    Mirror            mirror;
    int               align;
    djvMemory::ENDIAN endian;

private:

    void init();
};

//------------------------------------------------------------------------------
//! \class djvPixelData
//!
//! This class provides pixel data.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvPixelData
{
public:

    //! Constructor.

    djvPixelData();

    //! Constructor.

    djvPixelData(const djvPixelData &);

    //! Constructor.

    djvPixelData(const djvPixelDataInfo &, const quint8 * = 0, djvFileIo * = 0);

    //! Destructor.

    virtual ~djvPixelData();

    //! Set the pixel data.

    void set(const djvPixelDataInfo &, const quint8 * = 0, djvFileIo * = 0);

    //! Zero the pixel data.

    void zero();

    //! Get the pixel data information.

    inline const djvPixelDataInfo & info() const;

    //! Get the dimensions.

    inline const djvVector2i & size() const;

    //! Get the proxy scale.

    inline djvPixelDataInfo::PROXY proxy() const;

    //! Get the width.

    inline int w() const;

    //! Get the height.

    inline int h() const;

    //! Get the pixel type.

    inline djvPixel::PIXEL pixel() const;

    //! Get the number of channels.

    inline int channels() const;

    //! Get whether the pixel data is valid.

    inline bool isValid() const;

    //! Get a pointer to the data.

    inline quint8 * data();

    //! Get a pointer to the data.

    inline const quint8 * data() const;

    //! Get a pointer to the data.

    inline quint8 * data(int x, int y);

    //! Get a pointer to the data.

    inline const quint8 * data(int x, int y) const;

    //! Get the number of bytes in a pixel.

    inline quint64 pixelByteCount() const;

    //! Get the number of bytes in a scanline.

    inline quint64 scanlineByteCount() const;

    //! Get the number of bytes in the data.

    inline quint64 dataByteCount() const;
    
    //! Close the file I/O associated with the pixel data. This will initialize
    //! the image.
    
    void close();

    djvPixelData & operator = (const djvPixelData &);

private:

    void init();
    void detach();
    void copy(const djvPixelData &);

    djvPixelDataInfo        _info;
    int                     _channels;
    djvMemoryBuffer<quint8> _data;
    const quint8 *          _p;
    quint64                 _pixelByteCount;
    quint64                 _scanlineByteCount;
    quint64                 _dataByteCount;
    djvFileIo *             _fileIo;
};

//------------------------------------------------------------------------------

Q_DECLARE_METATYPE(djvPixelDataInfo)
Q_DECLARE_METATYPE(djvPixelData)

DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvPixelDataInfo::Mirror);
DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvPixelDataInfo);
DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvPixelData);

DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvPixelDataInfo::PROXY);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvPixelDataInfo::Mirror);

DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvPixelDataInfo::PROXY);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvPixelDataInfo::Mirror);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvPixelDataInfo);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvPixelData);

//@} // djvCoreImage

#include <djvPixelDataInline.h>

#endif // DJV_PIXEL_DATA_H

