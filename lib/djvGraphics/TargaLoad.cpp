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

#include <djvGraphics/TargaLoad.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/PixelDataUtil.h>

namespace djv
{
    namespace Graphics
    {
        TargaLoad::TargaLoad(Core::CoreContext * context) :
            ImageLoad(context)
        {}

        TargaLoad::~TargaLoad()
        {}

        void TargaLoad::open(const Core::FileInfo & in, ImageIOInfo & info)
            throw (Core::Error)
        {
            //DJV_DEBUG("TargaLoad::open");
            //DJV_DEBUG_PRINT("in = " << in);
            _file = in;
            Core::FileIO io;
            _open(_file.fileName(_file.sequence().start()), info, io);
            if (Core::FileInfo::SEQUENCE == _file.type())
            {
                info.sequence.frames = _file.sequence().frames;
            }
        }

        void TargaLoad::read(Image & image, const ImageIOFrameInfo & frame)
            throw (Core::Error)
        {
            //DJV_DEBUG("TargaLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);

            image.colorProfile = ColorProfile();
            image.tags = ImageTags();

            // Open the file.
            const QString fileName =
                _file.fileName(frame.frame != -1 ? frame.frame : _file.sequence().start());
            //DJV_DEBUG_PRINT("file name = " << fileName);
            ImageIOInfo info;
            QScopedPointer<Core::FileIO> io(new Core::FileIO);
            _open(fileName, info, *io);

            // Read the file.
            io->readAhead();
            PixelData * data = frame.proxy ? &_tmp : &image;
            if (!_compression)
            {
                if ((io->size() - io->pos()) < PixelDataUtil::dataByteCount(info))
                {
                    throw Core::Error(
                        Targa::staticName,
                        ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                }
                data->set(info, io->mmapP(), io.data());
                io.take();
            }
            else
            {
                data->set(info);
                const quint8 * p = io->mmapP();
                const quint8 * const end = io->mmapEnd();
                const int channels = Pixel::channels(info.pixel);
                for (int y = 0; y < info.size.y; ++y)
                {
                    //DJV_DEBUG_PRINT("y = " << y);
                    p = Targa::readRle(
                        p,
                        end,
                        data->data(0, y),
                        info.size.x,
                        channels);
                    if (!p)
                    {
                        throw Core::Error(
                            Targa::staticName,
                            ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                    }
                }
            }

            // Proxy scale the image.
            if (frame.proxy)
            {
                info.size = PixelDataUtil::proxyScale(info.size, frame.proxy);
                info.proxy = frame.proxy;
                image.set(info);
                PixelDataUtil::proxyScale(_tmp, image, frame.proxy);
            }

            //DJV_DEBUG_PRINT("image = " << image);
        }

        void TargaLoad::_open(const QString & in, ImageIOInfo & info, Core::FileIO & io)
            throw (Core::Error)
        {
            //DJV_DEBUG("djvTargaLoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);
            io.setEndian(Core::Memory::endian() != Core::Memory::LSB);
            io.open(in, Core::FileIO::READ);
            info.fileName = in;
            Targa::loadInfo(io, info, &_compression);
        }

    } // namespace Graphics
} // namespace djv
