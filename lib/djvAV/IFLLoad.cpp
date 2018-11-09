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

#include <djvAV/IFLLoad.h>

#include <djvAV/AVContext.h>
#include <djvAV/Image.h>
#include <djvAV/Pixel.h>

#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>
#include <djvCore/FileIOUtil.h>
#include <djvCore/FileInfoUtil.h>

namespace djv
{
    namespace AV
    {
        IFLLoad::IFLLoad(const Core::FileInfo & fileInfo, const QPointer<Core::CoreContext> & context) :
            ImageLoad(fileInfo, context)
        {
            //DJV_DEBUG("IFLLoad::IFLLoad");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            _list.clear();
            QStringList tmp;
            try
            {
                tmp = Core::FileIOUtil::lines(_fileInfo);
                //DJV_DEBUG_PRINT("list = " << tmp);
            }
            catch (const Core::Error &)
            {
                throw Core::Error(
                    IFL::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_READ]);
            }
            for (int i = 0; i < tmp.count(); ++i)
            {
                if (tmp[i].isEmpty())
                {
                    continue;
                }
                Core::FileInfo fileInfo(tmp[i]);
                if (fileInfo.path().isEmpty())
                {
                    fileInfo.setPath(_fileInfo.path());
                }
                if (fileInfo.isSequenceValid())
                {
                    fileInfo.setType(Core::FileInfo::SEQUENCE);

                    _list += Core::FileInfoUtil::expandSequence(fileInfo);
                }
                else
                {
                    _list += tmp[i];
                }
            }
            //DJV_DEBUG_PRINT("list = " << _list);
            dynamic_cast<AVContext*>(context.data())->imageIOFactory()->load(_list.count() ? _list[0] : QString(), _imageIOInfo);
            _imageIOInfo.sequence.frames.resize(_list.count());
            for (int i = 0; i < _list.count(); ++i)
            {
                _imageIOInfo.sequence.frames[i] = i;
            }
        }

        IFLLoad::~IFLLoad()
        {}

        void IFLLoad::read(Image & image, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("IFLLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);
            image.colorProfile = ColorProfile();
            image.tags = ImageTags();
            QString fileName;
            if (_list.count())
            {
                if (-1 == frame.frame)
                {
                    fileName = _list[0];
                }
                else if (
                    frame.frame >= 0 &&
                    frame.frame < _list.count())
                {
                    fileName = _list[frame.frame];
                }
            }
            //DJV_DEBUG_PRINT("file name = " << fileName);
            ImageIOInfo info;
            auto load = dynamic_cast<AVContext*>(context().data())->imageIOFactory()->load(fileName, info);
            load->read(image, ImageIOFrameInfo(-1, frame.layer, frame.proxy));
        }

    } // namespace AV
} // namespace djv
