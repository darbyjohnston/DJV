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

//! \file djvOpenExrSave.cpp

#include <djvOpenExrSave.h>

#include <djvError.h>
#include <djvOpenGlImage.h>

#include <ImfChannelList.h>
#include <ImfCompressionAttribute.h>
#include <ImfHeader.h>
#include <ImfOutputFile.h>
#include <ImfStandardAttributes.h>

//------------------------------------------------------------------------------
// djvOpenExrSave
//------------------------------------------------------------------------------

djvOpenExrSave::djvOpenExrSave(const djvOpenExrPlugin::Options & options) :
    _options(options),
    _f      (0)
{}

djvOpenExrSave::~djvOpenExrSave()
{
    close();
}

void djvOpenExrSave::open(const djvFileInfo & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvOpenExrSave::open");
    //DJV_DEBUG_PRINT("in = " << in);

    // File information.

    _file = in;

    if (info.sequence.frames.count() > 1)
    {
        _file.setType(djvFileInfo::SEQUENCE);
    }

    // Image information.

    _info = djvPixelDataInfo();
    _info.size = info.size;
    _info.mirror.y = true;

    djvPixel::FORMAT format = djvPixel::format(info.pixel);
    djvPixel::TYPE type = djvPixel::TYPE(0);

    switch (djvPixel::type(info.pixel))
    {
        case djvPixel::F32: type = djvPixel::F32; break;
        
        default: type = djvPixel::F16; break;
    }

    _info.pixel = djvPixel::pixel(format, type);

    //DJV_DEBUG_PRINT("info = " << _info);

    _speed = info.sequence.speed;

    // Initialize temporary image buffer.

    _tmp.set(_info);
}

void djvOpenExrSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvOpenExrSave::write");
    //DJV_DEBUG_PRINT("in = " << in);

    try
    {
        // Open the file.

        djvImageIoInfo info(_info);
        info.tags           = in.tags;
        info.sequence.speed = _speed;

        _open(_file.fileName(frame.frame), info);
        
        // Convert the image.

        const djvPixelData * p = &in;

        if (p->info() != _info)
        {
            //DJV_DEBUG_PRINT("convert = " << _tmp);

            _tmp.zero();

            djvOpenGlImage::copy(in, _tmp);

            p = &_tmp;
        }
        
        // Write the file.

        const int w         = p->w();
        const int h         = p->h();
        const int channels  = p->channels();
        const int byteCount = djvPixel::channelByteCount(p->pixel());

        Imf::FrameBuffer frameBuffer;

        for (int c = 0; c < channels; ++c)
        {
            const QString & channel = _channels[c];

            //DJV_DEBUG_PRINT("channel = " << channel);

            frameBuffer.insert(
                channel.toLatin1().data(),
                Imf::Slice(
                    djvOpenExrPlugin::pixelTypeToImf(djvPixel::type(p->pixel())),
                    (char *)p->data() + c * byteCount,
                    channels * byteCount,
                    w * channels * byteCount,
                    1,
                    1,
                    0.0));
        }

        _f->setFrameBuffer(frameBuffer);

        _f->writePixels(h);

    }
    catch (const std::exception & error)
    {
        throw djvError(
            djvOpenExrPlugin::staticName,
            error.what());
    }

    close();
}

void djvOpenExrSave::close() throw (djvError)
{
    delete _f;
    
    _f = 0;
}

void djvOpenExrSave::_open(const QString & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvOpenExrSave::_open");
    //DJV_DEBUG_PRINT("in = " << in);

    try
    {
        close();

        // Set the header.

        Imf::Header header(info.size.x, info.size.y);

        switch (djvPixel::channels(info.pixel))
        {
            case 1: _channels = QStringList() << "Y"; break;
            case 2: _channels = QStringList() << "Y" << "A"; break;
            case 3: _channels = QStringList() << "R" << "G" << "B"; break;
            case 4: _channels = QStringList() << "R" << "G" << "B" << "A"; break;
        }

        for (int i = 0; i < _channels.count(); ++i)
        {
            header.channels().insert(
                _channels[i].toLatin1().data(),
                djvOpenExrPlugin::pixelTypeToImf(djvPixel::type(info.pixel)));
        }

        Imf::CompressionAttribute compression;

        switch (_options.compression)
        {
            case djvOpenExrPlugin::COMPRESSION_NONE:
                compression = Imf::NO_COMPRESSION;
                break;

            case djvOpenExrPlugin::COMPRESSION_RLE:
                compression = Imf::RLE_COMPRESSION;
                break;

            case djvOpenExrPlugin::COMPRESSION_ZIPS:
                compression = Imf::ZIPS_COMPRESSION;
                break;

            case djvOpenExrPlugin::COMPRESSION_ZIP:
                compression = Imf::ZIP_COMPRESSION;
                break;

            case djvOpenExrPlugin::COMPRESSION_PIZ:
                compression = Imf::PIZ_COMPRESSION;
                break;

            case djvOpenExrPlugin::COMPRESSION_PXR24:
                compression = Imf::PXR24_COMPRESSION;
                break;

            case djvOpenExrPlugin::COMPRESSION_B44:
                compression = Imf::B44_COMPRESSION;
                break;

            case djvOpenExrPlugin::COMPRESSION_B44A:
                compression = Imf::B44A_COMPRESSION;
                break;

#if OPENEXR_VERSION_HEX >= 0x02020000
            case djvOpenExrPlugin::COMPRESSION_DWAA:
                compression = Imf::DWAA_COMPRESSION;
                break;

            case djvOpenExrPlugin::COMPRESSION_DWAB:
                compression = Imf::DWAB_COMPRESSION;
                break;
#endif // OPENEXR_VERSION_HEX

            default: break;
        }

        header.insert(Imf::CompressionAttribute::staticTypeName(), compression);
        
#if OPENEXR_VERSION_HEX >= 0x02020000
        addDwaCompressionLevel(header, _options.dwaCompressionLevel);
#endif // OPENEXR_VERSION_HEX

        // Set image tags.

        djvOpenExrPlugin::saveTags(info, header);

        // Open the file.

        _f = new Imf::OutputFile(in.toLatin1().data(), header);

    }
    catch (const std::exception & error)
    {
        throw djvError(
            djvOpenExrPlugin::staticName,
            error.what());
    }
}
