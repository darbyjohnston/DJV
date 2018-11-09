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

#pragma once

#include <djvViewLib/ViewLib.h>

#include <djvAV/ImageIO.h>
#include <djvAV/OpenGLImage.h>
#include <djvAV/Pixel.h>

#include <djvCore/FileInfo.h>

#include <QObject>

#include <memory>

namespace djv
{
    namespace ViewLib
    {
        class ViewContext;

        //! This struct provides file export information.
        struct FileExportInfo
        {
            FileExportInfo(
                const Core::FileInfo &               inputFile = Core::FileInfo(),
                const Core::FileInfo &               outputFile = Core::FileInfo(),
                const AV::PixelDataInfo &      info = AV::PixelDataInfo(),
                const Core::Sequence &               sequence = Core::Sequence(),
                int                                  layer = 0,
                const AV::PixelDataInfo::PROXY proxy = AV::PixelDataInfo::PROXY_NONE,
                bool                                 u8Conversion = false,
                bool                                 colorProfile = true,
                const AV::OpenGLImageOptions & options = AV::OpenGLImageOptions());

            Core::FileInfo                 inputFile;
            Core::FileInfo                 outputFile;
            AV::PixelDataInfo        info;
            Core::Sequence                 sequence;
            int                            layer = 0;
            AV::PixelDataInfo::PROXY proxy = AV::PixelDataInfo::PROXY_NONE;
            bool                           u8Conversion = false;
            bool                           colorProfile = true;
            AV::ImageIOFrameInfo     frameInfo;
            AV::OpenGLImageOptions   options;
        };

        //! This class provides file exporting.
        class FileExport : public QObject
        {
            Q_OBJECT

        public:
            explicit FileExport(const QPointer<ViewContext> &, QObject * parent = nullptr);
            ~FileExport() override;

        public Q_SLOTS:
            //! Start an export.
            void start(const djv::ViewLib::FileExportInfo &);

            //! Cancel an in progress export.
            void cancel();

        Q_SIGNALS:
            //! This signal is emitted when the export is finished.
            void finished();

        private Q_SLOTS:
            void callback(int);
            void finishedCallback();

        private:
            DJV_PRIVATE_COPY(FileExport);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
