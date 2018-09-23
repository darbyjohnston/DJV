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

#include <djvGraphics/RLALoad.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/CoreContext.h>

namespace djv
{
    namespace Graphics
    {
        RLALoad::RLALoad(const QPointer<Core::CoreContext> & context) :
            ImageLoad(context)
        {}

        RLALoad::~RLALoad()
        {}

        void RLALoad::open(const Core::FileInfo & in, ImageIOInfo & info)
        {
            //DJV_DEBUG("RLALoad::open");
            //DJV_DEBUG_PRINT("in = " << in);
            _file = in;
            Core::FileIO io;
            _open(_file.fileName(_file.sequence().start()), info, io);
            if (Core::FileInfo::SEQUENCE == _file.type())
            {
                info.sequence.frames = _file.sequence().frames;
            }
        }

        void RLALoad::read(Image & image, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("RLALoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);
            image.colorProfile = ColorProfile();
            image.tags = ImageTags();

            // Open the file.
            const QString fileName =
                _file.fileName(frame.frame != -1 ? frame.frame : _file.sequence().start());
            //DJV_DEBUG_PRINT("file name = " << fileName);
            ImageIOInfo info;
            Core::FileIO io;
            _open(fileName, info, io);
            if (frame.layer < 0 || frame.layer >= info.layerCount())
            {
                throw Core::Error(
                    RLA::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_READ]);
            }
            PixelDataInfo _info = info[frame.layer];

            // Read the file.
            io.readAhead();
            PixelData * p = frame.proxy ? &_tmp : &image;
            p->set(_info);
            const int w = _info.size.x;
            const int h = _info.size.y;
            const int channels = Pixel::channels(_info.pixel);
            const int bytes = Pixel::channelByteCount(_info.pixel);
            //DJV_DEBUG_PRINT("channels = " << channels);
            //DJV_DEBUG_PRINT("bytes = " << bytes);
            quint8 * data_p = p->data();
            for (int y = 0; y < h; ++y, data_p += w * channels * bytes)
            {
                io.setPos(_rleOffset[y]);
                for (int c = 0; c < channels; ++c)
                {
                    if (Pixel::F32 == Pixel::type(_info.pixel))
                    {
                        RLA::floatLoad(io, data_p + c * bytes, w, channels);
                    }
                    else
                    {
                        RLA::readRle(io, data_p + c * bytes, w, channels, bytes);
                    }
                }
            }

            // Proxy scale the image.
            if (frame.proxy)
            {
                _info.size = PixelDataUtil::proxyScale(_info.size, frame.proxy);
                _info.proxy = frame.proxy;
                image.set(_info);
                PixelDataUtil::proxyScale(_tmp, image, frame.proxy);
            }

            //DJV_DEBUG_PRINT("image = " << image);
        }

        namespace
        {
            struct Header
            {
                qint16 dimensions[4]; // Left, right, bottom, top.
                qint16 active[4];
                qint16 frame;
                qint16 colorChannelType;
                qint16 colorChannels;
                qint16 matteChannels;
                qint16 auxChannels;
                qint16 version;
                char   gamma[16];
                char   chroma[3][24];
                char   whitepoint[24];
                qint32 job;
                char   fileName[128];
                char   description[128];
                char   progam[64];
                char   machine[32];
                char   user[32];
                char   date[20];
                char   aspect[24];
                char   aspectRatio[8];
                char   colorFormat[32];
                qint16 field;
                char   renderTime[12];
                char   filter[32];
                qint16 colorBitDepth;
                qint16 matteChannelType;
                qint16 matteBitDepth;
                qint16 auxChannelType;
                qint16 auxBitDepth;
                char   auxFormat[32];
                char   pad[36];
                qint32 offset;
            };

            void endian(Header * in)
            {
                Core::Memory::convertEndian(&in->dimensions, 4, 2);
                Core::Memory::convertEndian(&in->active, 4, 2);
                Core::Memory::convertEndian(&in->frame, 1, 2);
                Core::Memory::convertEndian(&in->colorChannelType, 1, 2);
                Core::Memory::convertEndian(&in->colorChannels, 1, 2);
                Core::Memory::convertEndian(&in->matteChannels, 1, 2);
                Core::Memory::convertEndian(&in->auxChannels, 1, 2);
                Core::Memory::convertEndian(&in->version, 1, 2);
                Core::Memory::convertEndian(&in->job, 1, 4);
                Core::Memory::convertEndian(&in->field, 1, 2);
                Core::Memory::convertEndian(&in->colorBitDepth, 1, 2);
                Core::Memory::convertEndian(&in->matteChannelType, 1, 2);
                Core::Memory::convertEndian(&in->matteBitDepth, 1, 2);
                Core::Memory::convertEndian(&in->auxChannelType, 1, 2);
                Core::Memory::convertEndian(&in->auxBitDepth, 1, 2);
                Core::Memory::convertEndian(&in->offset, 1, 4);
            }

            void debug(const Header & in)
            {
                //DJV_DEBUG("debug(Header)");
                //DJV_DEBUG_PRINT("dimensions = " << in.dimensions[0] << " " <<
                //    in.dimensions[1] << " " << in.dimensions[2] << " " << "
                //    in.dimensions[3]);
                //DJV_DEBUG_PRINT("active = " << in.active[0] << " " << in.active[1] <<
                //    " " << in.active[2] << " " << in.active[3]);
                //DJV_DEBUG_PRINT("frame = " << in.frame);
                //DJV_DEBUG_PRINT("color channel type = " << in.colorChannelType);
                //DJV_DEBUG_PRINT("color channels = " << in.colorChannels);
                //DJV_DEBUG_PRINT("matte channels = " << in.matteChannels);
                //DJV_DEBUG_PRINT("aux channels = " << in.auxChannels);
                //DJV_DEBUG_PRINT("version = " << in.version);
                //DJV_DEBUG_PRINT("gamma = " << in.gamma);
                //DJV_DEBUG_PRINT("chroma = " << in.chroma[0] << " " << in.chroma[1] <<
                //    " " << in.chroma[2]);
                //DJV_DEBUG_PRINT("whitepoint = " << in.whitepoint);
                //DJV_DEBUG_PRINT("job = " << in.job);
                //DJV_DEBUG_PRINT("file name = " << in.fileName);
                //DJV_DEBUG_PRINT("description = " << in.description);
                //DJV_DEBUG_PRINT("progam = " << in.progam);
                //DJV_DEBUG_PRINT("machine = " << in.machine);
                //DJV_DEBUG_PRINT("user = " << in.user);
                //DJV_DEBUG_PRINT("date = " << in.date);
                //DJV_DEBUG_PRINT("aspect = " << in.aspect);
                //DJV_DEBUG_PRINT("aspect ratio = " << in.aspectRatio);
                //DJV_DEBUG_PRINT("color format = " << in.colorFormat);
                //DJV_DEBUG_PRINT("field = " << in.field);
                //DJV_DEBUG_PRINT("render time = " << in.renderTime);
                //DJV_DEBUG_PRINT("filter = " << in.filter);
                //DJV_DEBUG_PRINT("color bit depth = " << in.colorBitDepth);
                //DJV_DEBUG_PRINT("matte channel type = " << in.matteChannelType);
                //DJV_DEBUG_PRINT("matte bit depth = " << in.matteBitDepth);
                //DJV_DEBUG_PRINT("aux channel type = " << in.auxChannelType);
                //DJV_DEBUG_PRINT("aux bit depth = " << in.auxBitDepth);
                //DJV_DEBUG_PRINT("aux format = " << in.auxFormat);
                //DJV_DEBUG_PRINT("offset = " << in.offset);
            }

        } // namespace

        void RLALoad::_open(const QString & in, ImageIOInfo & info, Core::FileIO & io)
        {
            //DJV_DEBUG("djvRLALoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open the file.
            io.setEndian(Core::Memory::endian() != Core::Memory::MSB);
            io.open(in, Core::FileIO::READ);

            // Read the header.
            Header header;
            //DJV_DEBUG_PRINT("header size = " << static_cast<int>(sizeof(Header)));
            io.get(&header, sizeof(Header));
            if (io.endian())
            {
                endian(&header);
            }
            debug(header);
            const int w = header.active[1] - header.active[0] + 1;
            const int h = header.active[3] - header.active[2] + 1;

            // Read the scanline table.
            _rleOffset.resize(h);
            io.get32(_rleOffset.data(), h);

            // Get file information.
            const glm::ivec2 size(w, h);
            Pixel::PIXEL pixel = static_cast<Pixel::PIXEL>(0);
            if (header.matteChannels > 1)
            {
                throw Core::Error(
                    RLA::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
            if (header.matteChannelType != header.colorChannelType)
            {
                throw Core::Error(
                    RLA::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
            if (header.matteBitDepth != header.colorBitDepth)
            {
                throw Core::Error(
                    RLA::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
            if (!Pixel::pixel(
                header.colorChannels + header.matteChannels,
                header.colorBitDepth,
                3 == header.colorChannelType ? Pixel::FLOAT : Pixel::INTEGER,
                pixel))
            {
                throw Core::Error(
                    RLA::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
            if (header.field)
            {
                throw Core::Error(
                    RLA::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
            info = PixelDataInfo(in, size, pixel);
        }

    } // namespace Graphics
} // namespace djv
