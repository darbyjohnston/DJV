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

#include <djvGraphics/LUTLoad.h>

#include <djvGraphics/Image.h>

#include <djvCore/Assert.h>
#include <djvCore/CoreContext.h>
#include <djvCore/FileIO.h>
#include <djvCore/ListUtil.h>

namespace djv
{
    namespace Graphics
    {
        LUTLoad::LUTLoad(const Core::FileInfo & fileInfo, const LUT::Options & options, const QPointer<Core::CoreContext> & context) :
            ImageLoad(fileInfo, context),
            _options(options)
        {
            Core::FileIO io;
            _open(_fileInfo.fileName(_fileInfo.sequence().start()), _imageIOInfo, io);
            if (Core::FileInfo::SEQUENCE == _fileInfo.type())
            {
                _imageIOInfo.sequence.frames = _fileInfo.sequence().frames;
            }
        }

        LUTLoad::~LUTLoad()
        {}

        void LUTLoad::read(Image & image, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("LUTLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);

            image.colorProfile = ColorProfile();
            image.tags = ImageTags();

            // Open the file.
            const QString fileName = _fileInfo.fileName(frame.frame != -1 ? frame.frame : _fileInfo.sequence().start());
            ImageIOInfo info;
            Core::FileIO io;
            _open(fileName, info, io);

            // Read the file.
            image.set(info);
            switch (_format)
            {
            case LUT::FORMAT_INFERNO:
                LUT::infernoLoad(io, image);
                break;
            case LUT::FORMAT_KODAK:
                LUT::kodakLoad(io, image);
                break;
            default: break;
            }
            //DJV_DEBUG_PRINT("image = " << image);
        }

        void LUTLoad::_open(const Core::FileInfo & in, ImageIOInfo & info, Core::FileIO & io)
        {
            //DJV_DEBUG("LUTLoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);
            io.open(in, Core::FileIO::READ);
            info.fileName = in;
            const int index = LUT::staticExtensions.indexOf(in.extension());
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
                LUT::infernoOpen(io, info, _options.type);
                break;
            case LUT::FORMAT_KODAK:
                LUT::kodakOpen(io, info, _options.type);
                break;
            default: break;
            }
        }

    } // namespace Graphics
} // namespace djv
