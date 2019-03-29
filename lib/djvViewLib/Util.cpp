// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewLib/Util.h>

#include <djvViewLib/Enum.h>
#include <djvViewLib/ViewContext.h>

#include <djvAV/Image.h>
#include <djvAV/IO.h>

#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>

#include <QDir>

namespace djv
{
    namespace ViewLib
    {
        Util::~Util()
        {}

        void Util::loadLut(
            const Core::FileInfo & fileInfo,
            AV::PixelData & lut,
            const QPointer<ViewContext> & context)
        {
            //DJV_DEBUG("Util::loadLut");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            lut = AV::PixelData();
            if (!fileInfo.fileName().isEmpty())
            {
                Core::FileInfo fileInfoTmp(fileInfo);
                if (fileInfoTmp.isSequenceValid())
                {
                    fileInfoTmp.setType(Core::FileInfo::SEQUENCE);
                }
                try
                {
                    AV::IOInfo info;
                    auto load = context->ioFactory()->load(fileInfoTmp, info);
                    AV::Image image;
                    load->read(image);
                    lut = image;
                    //DJV_DEBUG_PRINT("lut = " << lut);
                }
                catch (Core::Error error)
                {
                    error.add(
                        Enum::errorLabels()[Enum::ERROR_OPEN_LUT].
                        arg(QDir::toNativeSeparators(fileInfoTmp)));
                    throw error;
                }
            }
        }

    } // namespace ViewLib
} // namespace djv
