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

#include <djvAV/LUTSave.h>

#include <djvAV/OpenGLImage.h>

#include <djvCore/CoreContext.h>
#include <djvCore/FileIO.h>
#include <djvCore/ListUtil.h>

namespace djv
{
    namespace AV
    {
        LUTSave::LUTSave(const Core::FileInfo & fileInfo, const ImageIOInfo & imageIOInfo, const LUT::Options & options, const QPointer<Core::CoreContext> & context) :
            ImageSave(fileInfo, imageIOInfo, context),
            _options(options)
        {
            //DJV_DEBUG("LUTSave::LUTSave");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            if (_imageIOInfo.sequence.frames.count() > 1)
            {
                _fileInfo.setType(Core::FileInfo::SEQUENCE);
            }
            _info = PixelDataInfo();
            _info.size = glm::ivec2(_imageIOInfo.size.x, 1);
            Pixel::TYPE type = Pixel::type(_imageIOInfo.pixel);
            switch (type)
            {
            case Pixel::F16:
            case Pixel::F32: type = Pixel::U16; break;
            default: break;
            }
            _info.pixel = Pixel::pixel(Pixel::format(_imageIOInfo.pixel), type);
            //DJV_DEBUG_PRINT("info = " << _info);
            _image.set(_info);
        }

        LUTSave::~LUTSave()
        {}

        void LUTSave::write(const Image & in, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("LUTSave::write");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("frame = " << frame);

            // Open the file.
            const QString fileName = _fileInfo.fileName(frame.frame);
            Core::FileIO io;
            io.open(fileName, Core::FileIO::WRITE);
            const int index = LUT::staticExtensions.indexOf(_fileInfo.extension());
            if (-1 == index)
            {
                throw Core::Error(
                    LUT::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNRECOGNIZED]);
            }
            _format = static_cast<LUT::FORMAT>(index);
            switch (_format)
            {
            case LUT::FORMAT_INFERNO:
                LUT::infernoOpen(io, _info);
                break;
            case LUT::FORMAT_KODAK:
                LUT::kodakOpen(io, _info);
                break;
            default: break;
            }

            // Convert the image.
            const PixelData * p = &in;
            if (in.info() != _info)
            {
                //DJV_DEBUG_PRINT("convert = " << _image);
                _image.zero();
                OpenGLImage().copy(in, _image);
                p = &_image;
            }

            // Write the file.
            switch (_format)
            {
            case LUT::FORMAT_INFERNO:
                LUT::infernoSave(io, p);
                break;
            case LUT::FORMAT_KODAK:
                LUT::kodakSave(io, p);
                break;
            default: break;
            }
        }

    } // namespace AV
} // namespace djv
