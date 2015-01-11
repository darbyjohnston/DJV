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

//! \file djvTiffSave.cpp

#include <djvTiffSave.h>

#include <djvOpenGlImage.h>

//------------------------------------------------------------------------------
// djvTiffSave
//------------------------------------------------------------------------------

djvTiffSave::djvTiffSave(const djvTiffPlugin::Options & options) :
    _options(options),
    _f      (0)
{}

djvTiffSave::~djvTiffSave()
{
    close();
}

void djvTiffSave::open(const djvFileInfo & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvTiffSave::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;

    _info          = djvPixelDataInfo();
    _info.size     = info.size;
    _info.mirror.y = true;

    djvPixel::TYPE type = djvPixel::type(info.pixel);

    switch (type)
    {
        case djvPixel::U10: type = djvPixel::U16; break;
        case djvPixel::F16: type = djvPixel::F32; break;

        default: break;
    }

    _info.pixel = djvPixel::pixel(djvPixel::format(info.pixel), type);

    //DJV_DEBUG_PRINT("info = " << _info);

    _image.set(_info);
}

void djvTiffSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvTiffSave::write");
    //DJV_DEBUG_PRINT("in = " << in);

    // Open the file.

    const QString fileName = _file.fileName(frame.frame);

    //DJV_DEBUG_PRINT("file name = " << fileName);

    djvImageIoInfo info(_info);
    info.tags = in.tags;

    _open(fileName, info);

    // Convert the image.

    const djvPixelData * p = &in;

    if (in.info() != _info)
    {
        //DJV_DEBUG_PRINT("convert = " << _image);

        _image.zero();

        djvOpenGlImage::copy(in, _image);

        p = &_image;
    }
    
    //! Write the file.

    const int h = p->h();

    for (int y = 0; y < h; ++y)
    {
        if (TIFFWriteScanline(_f, (tdata_t *)p->data(0, y), y) == -1)
        {
            throw djvError(
                djvTiffPlugin::staticName,
                djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].
                arg(fileName));
        }
    }

    close();
}

void djvTiffSave::close() throw (djvError)
{
    if (_f)
    {
        TIFFClose(_f);
        
        _f = 0;
    }
}

void djvTiffSave::_open(const QString & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvTiffSave::_open");
    //DJV_DEBUG_PRINT("in = " << in);

    close();

    // Open the file.

    _f = TIFFOpen(in.toLatin1().data(), "w");

    if (! _f)
    {
        throw djvError(
            djvTiffPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_OPEN].
            arg(in));
    }

    // Write the header.

    uint16 photometric      = 0;
    uint16 samples          = 0;
    uint16 sampleDepth      = 0;
    uint16 sampleFormat     = 0;
    uint16 extraSamples []  = { EXTRASAMPLE_ASSOCALPHA };
    uint16 extraSamplesSize = 0;
    uint16 compression      = 0;

    switch (djvPixel::format(_image.pixel()))
    {
        case djvPixel::L:
            photometric = PHOTOMETRIC_MINISBLACK;
            samples     = 1;
            break;

        case djvPixel::LA:
            photometric      = PHOTOMETRIC_MINISBLACK;
            samples          = 2;
            extraSamplesSize = 1;
            break;

        case djvPixel::RGB:
            photometric = PHOTOMETRIC_RGB;
            samples     = 3;
            break;

        case djvPixel::RGBA:
            photometric      = PHOTOMETRIC_RGB;
            samples          = 4;
            extraSamplesSize = 1;
            break;

        default: break;
    }

    switch (djvPixel::type(_image.pixel()))
    {
        case djvPixel::U8:
            sampleDepth  = 8;
            sampleFormat = SAMPLEFORMAT_UINT;
            break;

        case djvPixel::U16:
            sampleDepth  = 16;
            sampleFormat = SAMPLEFORMAT_UINT;
            break;

        case djvPixel::F16:
        case djvPixel::F32:
            sampleDepth  = 32;
            sampleFormat = SAMPLEFORMAT_IEEEFP;
            break;

        default: break;
    }

    switch (_options.compression)
    {
        case djvTiffPlugin::_COMPRESSION_NONE:
            compression = COMPRESSION_NONE;
            break;

        case djvTiffPlugin::_COMPRESSION_RLE:
            compression = COMPRESSION_PACKBITS;
            break;

        case djvTiffPlugin::_COMPRESSION_LZW:
            compression = COMPRESSION_LZW;
            break;

        default: break;
    }

    TIFFSetField(_f, TIFFTAG_IMAGEWIDTH, _image.w());
    TIFFSetField(_f, TIFFTAG_IMAGELENGTH, _image.h());
    TIFFSetField(_f, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(_f, TIFFTAG_SAMPLESPERPIXEL, samples);
    TIFFSetField(_f, TIFFTAG_BITSPERSAMPLE, sampleDepth);
    TIFFSetField(_f, TIFFTAG_SAMPLEFORMAT, sampleFormat);
    TIFFSetField(_f, TIFFTAG_EXTRASAMPLES, extraSamplesSize, extraSamples);
    TIFFSetField(_f, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(_f, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(_f, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

    // Set image tags.

    const QStringList & tags = djvImageTags::tagLabels();
    
    QString tmp = info.tags[tags[djvImageTags::CREATOR]];

    if (tmp.length())
    {
        TIFFSetField(_f, TIFFTAG_ARTIST, tmp.toLatin1().data());
    }

    tmp = info.tags[tags[djvImageTags::COPYRIGHT]];

    if (tmp.length())
    {
        TIFFSetField(_f, TIFFTAG_COPYRIGHT, tmp.toLatin1().data());
    }

    tmp = info.tags[tags[djvImageTags::TIME]];

    if (tmp.length())
    {
        TIFFSetField(_f, TIFFTAG_DATETIME, tmp.toLatin1().data());
    }

    tmp = info.tags[tags[djvImageTags::DESCRIPTION]];

    if (tmp.length())
    {
        TIFFSetField(_f, TIFFTAG_IMAGEDESCRIPTION, tmp.toLatin1().data());
    }
}

