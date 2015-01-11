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

//! \file djvLutLoad.cpp

#include <djvLutLoad.h>

#include <djvAssert.h>
#include <djvImage.h>
#include <djvListUtil.h>

//------------------------------------------------------------------------------
// djvLutLoad
//------------------------------------------------------------------------------

djvLutLoad::djvLutLoad(const djvLutPlugin::Options & options) :
    _options(options)
{}

djvLutLoad::~djvLutLoad()
{}

void djvLutLoad::open(const djvFileInfo & in, djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvLutLoad::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;

    djvFileIo io;
    
    _open(_file.fileName(_file.sequence().start()), info, io);

    if (djvFileInfo::SEQUENCE == _file.type())
    {
        info.sequence.frames = _file.sequence().frames;
    }
}

void djvLutLoad::read(djvImage & image, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvLutLoad::read");
    //DJV_DEBUG_PRINT("frame = " << frame);

    image.colorProfile = djvColorProfile();
    image.tags = djvImageTags();

    // Open the file.

    const QString fileName =
        _file.fileName(frame.frame != -1 ? frame.frame : _file.sequence().start());

    djvImageIoInfo info;
    
    djvFileIo io;
    
    _open(fileName, info, io);

    // Read the file.

    image.set(info);

    switch (_format)
    {
        case djvLutPlugin::FORMAT_INFERNO:
            djvLutPlugin::infernoLoad(io, image);
            break;

        case djvLutPlugin::FORMAT_KODAK:
            djvLutPlugin::kodakLoad(io, image);
            break;

        default: break;
    }

    //DJV_DEBUG_PRINT("image = " << image);
}

void djvLutLoad::_open(const djvFileInfo & in, djvImageIoInfo & info, djvFileIo & io)
    throw (djvError)
{
    //DJV_DEBUG("djvLutLoad::_open");
    //DJV_DEBUG_PRINT("in = " << in);

    io.open(in, djvFileIo::READ);

    info.fileName = in;
    
    const int index = djvLutPlugin::staticExtensions.indexOf(in.extension());

    if (-1 == index)
    {
        throw djvError(
            djvLutPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNRECOGNIZED].arg(in));
    }

    _format = static_cast<djvLutPlugin::FORMAT>(index);

    switch (_format)
    {
        case djvLutPlugin::FORMAT_INFERNO:
            djvLutPlugin::infernoOpen(io, info, _options.type);
            break;

        case djvLutPlugin::FORMAT_KODAK:
            djvLutPlugin::kodakOpen(io, info, _options.type);
            break;

        default: break;
    }
}
