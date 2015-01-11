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

//! \file djvLutSave.cpp

#include <djvLutSave.h>

#include <djvListUtil.h>
#include <djvOpenGlImage.h>

//------------------------------------------------------------------------------
// djvLutSave
//------------------------------------------------------------------------------

djvLutSave::djvLutSave(const djvLutPlugin::Options & options) :
    _options(options)
{}

djvLutSave::~djvLutSave()
{}

void djvLutSave::open(const djvFileInfo & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvLutSave::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;

    if (info.sequence.frames.count() > 1)
    {
        _file.setType(djvFileInfo::SEQUENCE);
    }

    _info = djvPixelDataInfo();
    _info.size = djvVector2i(info.size.x, 1);
    djvPixel::TYPE type = djvPixel::type(info.pixel);

    switch (type)
    {
        case djvPixel::F16:
        case djvPixel::F32: type = djvPixel::U16; break;

        default: break;
    }

    _info.pixel = djvPixel::pixel(djvPixel::format(info.pixel), type);

    //DJV_DEBUG_PRINT("info = " << _info);

    _image.set(_info);
}

void djvLutSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvLutSave::write");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("frame = " << frame);

    // Open the file.
    
    const QString fileName = _file.fileName(frame.frame);
    
    djvFileIo io;

    io.open(fileName, djvFileIo::WRITE);
    
    const int index = djvLutPlugin::staticExtensions.indexOf(_file.extension());

    if (-1 == index)
    {
        throw djvError(
            djvLutPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNRECOGNIZED].
            arg(fileName));
    }

    _format = static_cast<djvLutPlugin::FORMAT>(index);

    switch (_format)
    {
        case djvLutPlugin::FORMAT_INFERNO:
            djvLutPlugin::infernoOpen(io, _info);
            break;

        case djvLutPlugin::FORMAT_KODAK:
            djvLutPlugin::kodakOpen(io, _info);
            break;

        default: break;
    }

    // Convert the image.

    const djvPixelData * p = &in;

    if (in.info() != _info)
    {
        //DJV_DEBUG_PRINT("convert = " << _image);

        _image.zero();

        djvOpenGlImage::copy(in, _image);

        p = &_image;
    }

    // Write the file.

    switch (_format)
    {
        case djvLutPlugin::FORMAT_INFERNO:
            djvLutPlugin::infernoSave(io, p);
            break;

        case djvLutPlugin::FORMAT_KODAK:
            djvLutPlugin::kodakSave(io, p);
            break;

        default: break;
    }
}
