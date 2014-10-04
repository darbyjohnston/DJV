//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvOpenExrLoad.cpp

#include <djvOpenExrLoad.h>

#include <djvError.h>
#include <djvImage.h>
#include <djvOpenGlImage.h>
#include <djvPixelDataUtil.h>

#include <ImfChannelList.h>
#include <ImfHeader.h>
#include <ImfInputFile.h>
#include <ImfRgbaYca.h>

#include <algorithm>

//------------------------------------------------------------------------------
// djvOpenExrLoad
//------------------------------------------------------------------------------

djvOpenExrLoad::djvOpenExrLoad(const djvOpenExrPlugin::Options & options) :
    _options(options),
    _f      (0)
{}

djvOpenExrLoad::~djvOpenExrLoad()
{
    close();
}

void djvOpenExrLoad::open(const djvFileInfo & in, djvImageIoInfo & info) throw (djvError)
{
    //DJV_DEBUG("djvOpenExrLoad::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _file = in;

    _open(_file.fileName(_file.sequence().start()), info);

    //DJV_DEBUG_PRINT("info = " << info);

    if (djvFileInfo::SEQUENCE == _file.type())
    {
        info.sequence.frames = _file.sequence().frames;
    }
}

void djvOpenExrLoad::read(djvImage & image, const djvImageIoFrameInfo & frame) throw (djvError)
{
    //DJV_DEBUG("djvOpenExrLoad::read");
    //DJV_DEBUG_PRINT("frame = " << frame);

    try
    {
        // Open the file.

        const QString fileName = _file.fileName(
            frame.frame != -1 ? frame.frame : _file.sequence().start());

        //DJV_DEBUG_PRINT("file name = " << fileName);

        djvImageIoInfo info;
        _open(fileName, info);

        if (frame.layer < 0 || frame.layer >= _layers.count())
        {
            djvImageIo::throwErrorRead(djvOpenExrPlugin::staticName, fileName);
        }

        djvPixelDataInfo _info = info[frame.layer];
        _info.size = _dataWindow.size;

        //! Set the image tags.
        
        image.tags = info.tags;

        //! Set the color profile.

        if (_options.inputColorProfile != djvOpenExrPlugin::COLOR_PROFILE_NONE)
        {
            //DJV_DEBUG_PRINT("color profile");

            switch (_options.inputColorProfile)
            {
                case djvOpenExrPlugin::COLOR_PROFILE_GAMMA:

                    //DJV_DEBUG_PRINT("gamma = " << _options.inputGamma);

                    image.colorProfile.type = djvColorProfile::GAMMA;

                    break;

                case djvOpenExrPlugin::COLOR_PROFILE_EXPOSURE:
                    image.colorProfile.type = djvColorProfile::EXPOSURE;
                    break;

                default: break;
            }

            image.colorProfile.gamma    = _options.inputGamma;
            image.colorProfile.exposure = _options.inputExposure;
        }
        else
        {
            image.colorProfile = djvColorProfile();
        }
        
        // Read the file.

        djvPixelData * data = frame.proxy ? &_tmp : &image;
        data->set(_info);

        const bool flip = Imf::DECREASING_Y == _f->header().lineOrder();

        //DJV_DEBUG_PRINT("flip = " << flip);

        const int         channels  = djvPixel::channels(_info.pixel);
        const int         byteCount = djvPixel::channelByteCount(_info.pixel);
        const djvVector2i sampling  = _layers[frame.layer].channels[0].sampling;
        
        Imf::FrameBuffer frameBuffer;

        for (int c = 0; c < channels; ++c)
        {
            const QString & channel = _layers[frame.layer].channels[c].name;

            //DJV_DEBUG_PRINT("channel = " << channel);

            frameBuffer.insert(
                channel.toLatin1().data(),
                Imf::Slice(
                    djvOpenExrPlugin::pixelTypeToImf(djvPixel::type(data->pixel())),
                    (char *)data->data() -
                    (_dataWindow.y * _info.size.x * channels * byteCount) -
                    (_dataWindow.x * channels * byteCount) +
                    c * byteCount,
                    channels * byteCount,
                    _info.size.x * channels * byteCount,
                    sampling.x,
                    sampling.y,
                    0.0));
        }

        _f->setFrameBuffer(frameBuffer);

        if (flip)
        {
            for (
                int y = 0;
                y < _info.size.y * sampling.y;
                y += sampling.y)
            {
                _f->readPixels(
                    _dataWindow.y + (_info.size.y * sampling.y - 1 - y));
            }
        }
        else
        {
            _f->readPixels(
                _dataWindow.y,
                _dataWindow.y + _info.size.y * sampling.y - 1);
        }

        if (_displayWindow != _dataWindow)
        {
            //DJV_DEBUG_PRINT("display window");

            //! \todo Is there a more efficient way of reading the file's
            //! display/data window so we don't need this extra copy?

            djvPixelData tmp = *data;

            _info.size = _displayWindow.size;
            data->set(_info);

            djvOpenGlImageOptions options;
            options.xform.position =
                _dataWindow.position - _displayWindow.position;
            
            djvOpenGlImage::copy(tmp, *data, options);
        }

        if (frame.proxy)
        {
            //DJV_DEBUG_PRINT("proxy");

            _info.size = djvPixelDataUtil::proxyScale(_info.size, frame.proxy);
            _info.proxy = frame.proxy;
            image.set(_info);

            djvPixelDataUtil::proxyScale(_tmp, image, frame.proxy);
        }
    }
    catch (const std::exception & error)
    {
        throw djvError(djvOpenExrPlugin::staticName, error.what());
    }

    //DJV_DEBUG_PRINT("image = " << image);
    
    close();
}

void djvOpenExrLoad::close() throw (djvError)
{
    delete _f;
    
    _f = 0;
}

void djvOpenExrLoad::_open(const QString & in, djvImageIoInfo & info) throw (djvError)
{
    //DJV_DEBUG("djvOpenExrLoad::_open");
    //DJV_DEBUG_PRINT("in = " << in);

    close();

    try
    {
        _f = new Imf::InputFile(in.toLatin1().data());

        // Get file information.

        _displayWindow = djvOpenExrPlugin::imfToBox(_f->header().displayWindow());
        _dataWindow = djvOpenExrPlugin::imfToBox(_f->header().dataWindow());

        //DJV_DEBUG_PRINT("display window = " << _displayWindow);
        //DJV_DEBUG_PRINT("data window = " << _dataWindow);

        djvPixelDataInfo _info;
        _info.size = _displayWindow.size;
        _info.mirror.y = true;

        _layers = djvOpenExrPlugin::layer(
            _f->header().channels(),
            _options.channels);
        info.setLayerCount(_layers.count());
        
        //DJV_DEBUG_PRINT("layers = " << _layers.count());

        for (int i = 0; i < _layers.count(); ++i)
        {
            const djvVector2i sampling(
                _layers[i].channels[0].sampling.x,
                _layers[i].channels[0].sampling.y);

            djvPixelDataInfo tmp = _info;
            tmp.fileName = in;
            tmp.layerName = _layers[i].name;
            tmp.size /= sampling;
            
            djvPixel::FORMAT format = static_cast<djvPixel::FORMAT>(0);
            
            if (! djvPixel::format(_layers[i].channels.count(), format))
            {
                djvImageIo::throwUnsupported(djvOpenExrPlugin::staticName, in);
            }
            
            //DJV_DEBUG_PRINT("format = " << format);

            if (! djvPixel::pixel(
                format,
                _layers[i].channels[0].type,
                tmp.pixel))
            {
                djvImageIo::throwUnsupported(djvOpenExrPlugin::staticName, in);
            }

            //DJV_DEBUG_PRINT("pixel = " << tmp.pixel);

            info[i] = tmp;
        }

        // Get image tags.

        djvOpenExrPlugin::loadTags(_f->header(), info);
    }
    catch (const std::exception & error)
    {
        throw djvError(djvOpenExrPlugin::staticName, error.what());
    }
}
