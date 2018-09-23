//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvGraphics/OpenEXRLoad.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/BoxUtil.h>
#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>

#include <ImfChannelList.h>
#include <ImfHeader.h>
#include <ImfInputFile.h>
#include <ImfRgbaYca.h>

#include <algorithm>

namespace djv
{
    namespace Graphics
    {
        OpenEXRLoad::OpenEXRLoad(const OpenEXR::Options & options, const QPointer<Core::CoreContext> & context) :
            ImageLoad(context),
            _options(options),
            _f(0),
            _fast(false)
        {}

        OpenEXRLoad::~OpenEXRLoad()
        {
            close();
        }

        void OpenEXRLoad::open(const Core::FileInfo & in, ImageIOInfo & info)
        {
            //DJV_DEBUG("OpenEXRLoad::open");
            //DJV_DEBUG_PRINT("in = " << in);
            _file = in;
            _open(_file.fileName(_file.sequence().start()), info);
            //DJV_DEBUG_PRINT("info = " << info);
            if (Core::FileInfo::SEQUENCE == _file.type())
            {
                info.sequence.frames = _file.sequence().frames;
            }
        }

        void OpenEXRLoad::read(Image & image, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("OpenEXRLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);
            try
            {
                // Open the file.
                const QString fileName = _file.fileName(
                    frame.frame != -1 ? frame.frame : _file.sequence().start());
                //DJV_DEBUG_PRINT("file name = " << fileName);
                ImageIOInfo info;
                _open(fileName, info);
                if (frame.layer < 0 || frame.layer >= _layers.count())
                {
                    throw Core::Error(
                        OpenEXR::staticName,
                        ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                }
                PixelDataInfo pixelDataInfo = info[frame.layer];
                const bool flip = Imf::DECREASING_Y == _f->header().lineOrder();
                //DJV_DEBUG_PRINT("flip = " << flip);
                pixelDataInfo.mirror.y = !flip;
                //DJV_DEBUG_PRINT("pixel data info = " << pixelDataInfo);

                //! Set the image tags.
                image.tags = info.tags;

                //! Set the color profile.
                if (_options.inputColorProfile != OpenEXR::COLOR_PROFILE_NONE)
                {
                    //DJV_DEBUG_PRINT("color profile");
                    switch (_options.inputColorProfile)
                    {
                    case OpenEXR::COLOR_PROFILE_GAMMA:
                        //DJV_DEBUG_PRINT("gamma = " << _options.inputGamma);
                        image.colorProfile.type = ColorProfile::GAMMA;
                        break;
                    case OpenEXR::COLOR_PROFILE_EXPOSURE:
                        image.colorProfile.type = ColorProfile::EXPOSURE;
                        break;
                    default: break;
                    }
                    image.colorProfile.gamma = _options.inputGamma;
                    image.colorProfile.exposure = _options.inputExposure;
                }
                else
                {
                    image.colorProfile = ColorProfile();
                }

                // Read the file.
                PixelData * data = frame.proxy ? &_tmp : &image;
                data->set(pixelDataInfo);
                const int channels = Pixel::channels(pixelDataInfo.pixel);
                const int byteCount = Pixel::channelByteCount(pixelDataInfo.pixel);
                //DJV_DEBUG_PRINT("channels = " << channels);
                //DJV_DEBUG_PRINT("byteCount = " << byteCount);
                const int cb = channels * byteCount;
                const int scb = pixelDataInfo.size.x * channels * byteCount;
                //DJV_DEBUG_PRINT("fast = " << _fast);
                if (_fast)
                {
                    Imf::FrameBuffer frameBuffer;
                    for (int c = 0; c < channels; ++c)
                    {
                        const QString & channel = _layers[frame.layer].channels[c].name;
                        //DJV_DEBUG_PRINT("channel = " << channel);
                        const glm::ivec2 sampling = _layers[frame.layer].channels[c].sampling;
                        //DJV_DEBUG_PRINT("sampling = " << sampling);
                        frameBuffer.insert(
                            channel.toLatin1().data(),
                            Imf::Slice(
                                OpenEXR::pixelTypeToImf(Pixel::type(data->pixel())),
                                (char *)data->data() + (c * byteCount),
                                cb,
                                scb,
                                sampling.x,
                                sampling.y,
                                0.f));
                    }
                    _f->setFrameBuffer(frameBuffer);
                    _f->readPixels(
                        _displayWindow.y,
                        _displayWindow.y + _displayWindow.size.y - 1);
                }
                else
                {
                    Imf::FrameBuffer frameBuffer;
                    std::vector<char> buf(_dataWindow.size.x * cb);
                    for (int c = 0; c < channels; ++c)
                    {
                        const QString & channel = _layers[frame.layer].channels[c].name;
                        //DJV_DEBUG_PRINT("channel = " << channel);
                        const glm::ivec2 sampling = _layers[frame.layer].channels[c].sampling;
                        //DJV_DEBUG_PRINT("sampling = " << sampling);
                        frameBuffer.insert(
                            channel.toLatin1().data(),
                            Imf::Slice(
                                OpenEXR::pixelTypeToImf(Pixel::type(data->pixel())),
                                buf.data() - (_dataWindow.x * cb) + (c * byteCount),
                                cb,
                                0,
                                sampling.x,
                                sampling.y,
                                0.f));
                    }
                    _f->setFrameBuffer(frameBuffer);
                    for (
                        int y = _displayWindow.y;
                        y < _displayWindow.y + _displayWindow.size.y;
                        ++y)
                    {
                        quint8 * p = data->data() + ((y - _displayWindow.y) * scb);
                        quint8 * end = p + scb;
                        if (y >= _intersectedWindow.y &&
                            y < _intersectedWindow.y + _intersectedWindow.size.y)
                        {
                            quint64 size = (_intersectedWindow.x - _displayWindow.x) * cb;
                            memset(p, 0, size);
                            p += size;
                            size = _intersectedWindow.size.x * cb;
                            _f->readPixels(y, y);
                            memcpy(
                                p,
                                buf.data() + Core::Math::max(_displayWindow.x - _dataWindow.x, 0) * cb,
                                size);
                            p += size;
                        }
                        memset(p, 0, end - p);
                    }
                }
                if (frame.proxy)
                {
                    //DJV_DEBUG_PRINT("proxy");
                    pixelDataInfo.size = PixelDataUtil::proxyScale(pixelDataInfo.size, frame.proxy);
                    pixelDataInfo.proxy = frame.proxy;
                    image.set(pixelDataInfo);
                    PixelDataUtil::proxyScale(_tmp, image, frame.proxy);
                }
            }
            catch (const std::exception & error)
            {
                throw Core::Error(
                    OpenEXR::staticName,
                    error.what());
            }
            //DJV_DEBUG_PRINT("image = " << image);

            close();
        }

        void OpenEXRLoad::close()
        {
            delete _f;

            _f = 0;
        }

        void OpenEXRLoad::_open(const QString & in, ImageIOInfo & info)
        {
            //DJV_DEBUG("OpenEXRLoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            close();

            try
            {
                // Open the file.
                _f = new Imf::InputFile(in.toLatin1().data());

                // Get the display and data windows.
                _displayWindow = OpenEXR::imfToBox(_f->header().displayWindow());
                _dataWindow = OpenEXR::imfToBox(_f->header().dataWindow());
                _intersectedWindow = Core::BoxUtil::intersect(_displayWindow, _dataWindow);
                //DJV_DEBUG_PRINT("display window = " << _displayWindow);
                //DJV_DEBUG_PRINT("data window = " << _dataWindow);
                //DJV_DEBUG_PRINT("intersected window = " << _intersectedWindow);
                _fast = _displayWindow == _dataWindow;

                // Get the layers.
                _layers = OpenEXR::layer(_f->header().channels(), _options.channels);
                info.setLayerCount(_layers.count());
                //DJV_DEBUG_PRINT("layers = " << _layers.count());
                for (int i = 0; i < _layers.count(); ++i)
                {
                    //DJV_DEBUG_PRINT("layer = " << _layers[i].name);
                    const glm::ivec2 sampling(
                        _layers[i].channels[0].sampling.x,
                        _layers[i].channels[0].sampling.y);
                    //DJV_DEBUG_PRINT("sampling = " << sampling);
                    if (sampling.x != 1 || sampling.y != 1)
                        _fast = false;
                    PixelDataInfo pixelDataInfo;
                    pixelDataInfo.fileName = in;
                    pixelDataInfo.layerName = _layers[i].name;
                    pixelDataInfo.size = _displayWindow.size;
                    Pixel::FORMAT format = static_cast<Pixel::FORMAT>(0);
                    if (!Pixel::format(_layers[i].channels.count(), format))
                    {
                        throw Core::Error(
                            OpenEXR::staticName,
                            ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
                    }
                    //DJV_DEBUG_PRINT("format = " << format);
                    if (!Pixel::pixel(
                        format,
                        _layers[i].channels[0].type,
                        pixelDataInfo.pixel))
                    {
                        throw Core::Error(
                            OpenEXR::staticName,
                            ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
                    }
                    //DJV_DEBUG_PRINT("pixel = " << pixelDataInfo.pixel);
                    info[i] = pixelDataInfo;
                }
                //DJV_DEBUG_PRINT("fast = " << _fast);

                // Get the image tags.
                OpenEXR::loadTags(_f->header(), info);
            }
            catch (const std::exception & error)
            {
                throw Core::Error(
                    OpenEXR::staticName,
                    error.what());
            }
        }

    } // namespace Graphics
} // namespace djv
