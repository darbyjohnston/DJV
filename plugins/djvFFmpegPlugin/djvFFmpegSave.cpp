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

//! \file djvFFmpegSave.cpp

#include <djvFFmpegSave.h>

#include <djvOpenGlImage.h>

#include <stdio.h>

//------------------------------------------------------------------------------
// djvFFmpegSave
//------------------------------------------------------------------------------

djvFFmpegSave::djvFFmpegSave(const djvFFmpegPlugin::Options & options) :
    _options(options)
{}

djvFFmpegSave::~djvFFmpegSave()
{}

void djvFFmpegSave::open(const djvFileInfo & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvFFmpegSave::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;

    //DJV_DEBUG_PRINT("info = " << _info);

    _image.set(_info);
}

void djvFFmpegSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvFFmpegSave::write");
    //DJV_DEBUG_PRINT("in = " << in);
}

void djvFFmpegSave::_open(const QString & in, djvFileIo & io) throw (djvError)
{
    //DJV_DEBUG("djvFFmpegSave::_open");
    //DJV_DEBUG_PRINT("in = " << in);
}

