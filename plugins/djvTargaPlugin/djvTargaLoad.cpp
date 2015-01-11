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

//! \file djvTargaLoad.cpp

#include <djvTargaLoad.h>

#include <djvImage.h>
#include <djvPixelDataUtil.h>

//------------------------------------------------------------------------------
// djvTargaLoad
//------------------------------------------------------------------------------

djvTargaLoad::~djvTargaLoad()
{}

void djvTargaLoad::open(const djvFileInfo & in, djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvTargaLoad::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;
    
    djvFileIo io;
    
    _open(_file.fileName(_file.sequence().start()), info, io);

    if (djvFileInfo::SEQUENCE == _file.type())
    {
        info.sequence.frames = _file.sequence().frames;
    }
}

void djvTargaLoad::read(djvImage & image, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvTargaLoad::read");
    //DJV_DEBUG_PRINT("frame = " << frame);

    image.colorProfile = djvColorProfile();
    image.tags = djvImageTags();

    // Open the file.

    const QString fileName =
        _file.fileName(frame.frame != -1 ? frame.frame : _file.sequence().start());

    //DJV_DEBUG_PRINT("file name = " << fileName);

    djvImageIoInfo info;
    
    QScopedPointer<djvFileIo> io(new djvFileIo);
    
    _open(fileName, info, *io);

    // Read the file.

    io->readAhead();

    djvPixelData * data = frame.proxy ? &_tmp : &image;

    if (! _compression)
    {
        if ((io->size() - io->pos()) < djvPixelDataUtil::dataByteCount(info))
        {
            throw djvError(
                djvTargaPlugin::staticName,
                djvImageIo::errorLabels()[djvImageIo::ERROR_READ].
                arg(fileName));
        }

        data->set(info, io->mmapP(), io.data());

        io.take();
    }
    else
    {
        data->set(info);

        const quint8 * p = io->mmapP();
        
        const quint8 * const end = io->mmapEnd();
        
        const int channels = djvPixel::channels(info.pixel);

        for (int y = 0; y < info.size.y; ++y)
        {
            //DJV_DEBUG_PRINT("y = " << y);

            p = djvTargaPlugin::readRle(
                p,
                end,
                data->data(0, y),
                info.size.x,
                channels);

            if (! p)
            {
                throw djvError(
                    djvTargaPlugin::staticName,
                    djvImageIo::errorLabels()[djvImageIo::ERROR_READ].
                    arg(fileName));
            }
        }
    }

    // Proxy scale the image.
    
    if (frame.proxy)
    {
        info.size = djvPixelDataUtil::proxyScale(info.size, frame.proxy);
        info.proxy = frame.proxy;
        
        image.set(info);

        djvPixelDataUtil::proxyScale(_tmp, image, frame.proxy);
    }

    //DJV_DEBUG_PRINT("image = " << image);
}

void djvTargaLoad::_open(const QString & in, djvImageIoInfo & info, djvFileIo & io)
    throw (djvError)
{
    //DJV_DEBUG("djvTargaLoad::_open");
    //DJV_DEBUG_PRINT("in = " << in);

    io.setEndian(djvMemory::endian() != djvMemory::LSB);

    io.open(in, djvFileIo::READ);

    info.fileName = in;
    djvTargaPlugin::loadInfo(io, info, &_compression);
}
