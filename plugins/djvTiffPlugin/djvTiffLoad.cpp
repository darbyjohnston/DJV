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

//! \file djvTiffLoad.cpp

#include <djvTiffLoad.h>

#include <djvImage.h>
#include <djvPixelDataUtil.h>

//------------------------------------------------------------------------------
// djvTiffLoad
//------------------------------------------------------------------------------

djvTiffLoad::djvTiffLoad() :
    _f(0)
{}

djvTiffLoad::~djvTiffLoad()
{
    close();
}

void djvTiffLoad::open(const djvFileInfo & in, djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvTiffLoad::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;
    
    _open(_file.fileName(_file.sequence().start()), info);

    if (djvFileInfo::SEQUENCE == _file.type())
    {
        info.sequence.frames = _file.sequence().frames;
    }
}

void djvTiffLoad::read(djvImage & image, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvTiffLoad::read");
    //DJV_DEBUG_PRINT("frame = " << frame);

    image.colorProfile = djvColorProfile();
    image.tags = djvImageTags();

    // Open the file.

    const QString fileName = _file.fileName(
        frame.frame != -1 ? frame.frame : _file.sequence().start());

    //DJV_DEBUG_PRINT("file name = " << fileName);

    djvImageIoInfo info;
    
    _open(fileName, info);
    
    image.tags = info.tags;

    // Read the file.

    djvPixelData * data = frame.proxy ? &_tmp : &image;
    data->set(info);

    for (int y = 0; y < info.size.y; ++y)
    {
        if (TIFFReadScanline(_f, (tdata_t *)data->data(0, y), y) == -1)
        {
            throw djvError(
                djvTiffPlugin::staticName,
                djvImageIo::errorLabels()[djvImageIo::ERROR_READ].
                arg(fileName));
        }

        if (_palette)
        {
            djvTiffPlugin::paletteLoad(
                data->data(0, y),
                info.size.x,
                djvPixel::channelByteCount(info.pixel),
                _colormap[0], _colormap[1], _colormap[2]);
        }
    }
    
    // Proxy scaling.

    if (frame.proxy)
    {
        info.size = djvPixelDataUtil::proxyScale(info.size, frame.proxy);
        info.proxy = frame.proxy;
        
        image.set(info);

        djvPixelDataUtil::proxyScale(_tmp, image, frame.proxy);
    }

    // Close the file.
    
    //DJV_DEBUG_PRINT("image = " << image);
    
    close();
}

void djvTiffLoad::close() throw (djvError)
{
    if (_f)
    {
        TIFFClose(_f);
        
        _f = 0;
    }
}

void djvTiffLoad::_open(const QString & in, djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvTiffLoad::_open");
    //DJV_DEBUG_PRINT("in = " << in);

    close();

    // Open the file.

    _f = TIFFOpen(in.toLatin1().data(), "r");

    if (! _f)
    {
        throw djvError(
            djvTiffPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_OPEN].arg(in));
    }

    // Read the Header.

    uint32   width            = 0;
    uint32   height           = 0;
    uint16   photometric      = 0;
    uint16   samples          = 0;
    uint16   sampleDepth      = 0;
    uint16   sampleFormat     = 0;
    uint16 * extraSamples     = 0;
    uint16   extraSamplesSize = 0;
    uint16   orient           = 0;
    uint16   compression      = 0;
    uint16   channels         = 0;

    TIFFGetFieldDefaulted(_f, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetFieldDefaulted(_f, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetFieldDefaulted(_f, TIFFTAG_PHOTOMETRIC, &photometric);
    TIFFGetFieldDefaulted(_f, TIFFTAG_SAMPLESPERPIXEL, &samples);
    TIFFGetFieldDefaulted(_f, TIFFTAG_BITSPERSAMPLE, &sampleDepth);
    TIFFGetFieldDefaulted(_f, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
    TIFFGetFieldDefaulted(_f, TIFFTAG_EXTRASAMPLES, &extraSamplesSize,
        &extraSamples);
    TIFFGetFieldDefaulted(_f, TIFFTAG_ORIENTATION, &orient);
    TIFFGetFieldDefaulted(_f, TIFFTAG_COMPRESSION, &compression);
    TIFFGetFieldDefaulted(_f, TIFFTAG_PLANARCONFIG, &channels);
    TIFFGetFieldDefaulted(_f, TIFFTAG_COLORMAP,
        &_colormap[0], &_colormap[1], &_colormap[2]);

    //DJV_DEBUG_PRINT("tiff size = " << width << " " << height);
    //DJV_DEBUG_PRINT("tiff photometric = " << photometric);
    //DJV_DEBUG_PRINT("tiff samples = " << samples);
    //DJV_DEBUG_PRINT("tiff sample depth = " << sampleDepth);
    //DJV_DEBUG_PRINT("tiff channels = " << channels);
    

    // Get file information.

    info.fileName = in;

    info.size = djvVector2i(width, height);

    if (samples > 1 && PLANARCONFIG_SEPARATE == channels)
    {
        throw djvError(
            djvTiffPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].
            arg(in));
    }

    djvPixel::PIXEL pixel = static_cast<djvPixel::PIXEL>(0);
    
    bool found = false;
    
    switch (photometric)
    {
        case PHOTOMETRIC_PALETTE:
        
            pixel = djvPixel::RGB_U8;
            
            found = true;
            
            break;

        case PHOTOMETRIC_MINISWHITE:
        case PHOTOMETRIC_MINISBLACK:
        case PHOTOMETRIC_RGB:

            if (32 == sampleDepth && sampleFormat != SAMPLEFORMAT_IEEEFP)
                break;
        
            found = djvPixel::pixel(
                samples,
                sampleDepth,
                SAMPLEFORMAT_IEEEFP == sampleFormat ?
                    djvPixel::FLOAT :
                    djvPixel::INTEGER,
                pixel);
            
            break;
    }
    
    //DJV_DEBUG_PRINT("found = " << found);
    //DJV_DEBUG_PRINT("pixel = " << pixel);

    if (! found)
    {
        throw djvError(
            djvTiffPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].
            arg(in));
    }

    info.pixel = pixel;

    _compression = compression != COMPRESSION_NONE;
    _palette = PHOTOMETRIC_PALETTE == photometric;

    switch (orient)
    {
        case ORIENTATION_TOPLEFT:  info.mirror.y = true;                 break;
        case ORIENTATION_TOPRIGHT: info.mirror.x = info.mirror.y = true; break;
        case ORIENTATION_BOTRIGHT: info.mirror.x = true;                 break;
        case ORIENTATION_BOTLEFT:                                        break;
    }

    // Get image tags.

    const QStringList & tags = djvImageTags::tagLabels();
    char * tag = 0;

    if (TIFFGetField(_f, TIFFTAG_ARTIST, &tag))
    {
        info.tags[tags[djvImageTags::CREATOR]] = tag;
    }

    if (TIFFGetField(_f, TIFFTAG_IMAGEDESCRIPTION, &tag))
    {
        info.tags[tags[djvImageTags::DESCRIPTION]] = tag;
    }

    if (TIFFGetField(_f, TIFFTAG_COPYRIGHT, &tag))
    {
        info.tags[tags[djvImageTags::COPYRIGHT]] = tag;
    }

    if (TIFFGetField(_f, TIFFTAG_DATETIME, &tag))
    {
        info.tags[tags[djvImageTags::TIME]] = tag;
    }
}
