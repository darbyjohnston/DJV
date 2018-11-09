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

#include <djvAV/Color.h>
#include <djvAV/Pixel.h>

#include <djvCore/Box.h>
#include <djvCore/Memory.h>
#include <djvCore/Vector.h>

#include <QMetaType>
#include <QString>

#include <vector>

namespace djv
{
    namespace Core
    {
        class FileIO;

    } // namespace Core

    namespace AV
    {
        //! This class provides pixel data information.
        class PixelDataInfo
        {
            Q_GADGET

        public:
            PixelDataInfo();
            PixelDataInfo(
                const glm::ivec2 & size,
                Pixel::PIXEL       pixel);
            PixelDataInfo(
                int          width,
                int          height,
                Pixel::PIXEL pixel);
            PixelDataInfo(
                const QString &    fileName,
                const glm::ivec2 & size,
                Pixel::PIXEL       pixel);
            PixelDataInfo(
                const QString & fileName,
                int             width,
                int             height,
                Pixel::PIXEL    pixel);

            //! This enumeration provides the proxy scale.
            enum PROXY
            {
                PROXY_NONE,
                PROXY_1_2,
                PROXY_1_4,
                PROXY_1_8,

                PROXY_COUNT
            };
            Q_ENUM(PROXY);

            //! Get the proxy scale labels.
            static const QStringList & proxyLabels();

            //! This struct provides mirroring.
            struct Mirror
            {
                Mirror();
                Mirror(bool x, bool y);

                bool x = false;
                bool y = false;
            };

            QString              fileName;
            QString              layerName;
            glm::ivec2           size = glm::ivec2(0, 0);
            PROXY                proxy = PROXY_NONE;
            Pixel::PIXEL         pixel = static_cast<Pixel::PIXEL>(0);
            bool                 bgr = false;
            Mirror               mirror;
            int                  align = 1;
            Core::Memory::ENDIAN endian = Core::Memory::endian();

        private:
            void init();
        };

        //! This class provides pixel data.
        class PixelData
        {
        public:
            PixelData();
            PixelData(const PixelData &);
            PixelData(const PixelDataInfo &, const quint8 * = 0, Core::FileIO * = 0);
            virtual ~PixelData();

            //! Set the pixel data.
            void set(const PixelDataInfo &, const quint8 * = 0, Core::FileIO * = 0);

            //! Zero the pixel data.
            void zero();

            //! Get the pixel data information.
            inline const PixelDataInfo & info() const;

            //! Get the dimensions.
            inline const glm::ivec2 & size() const;

            //! Get the proxy scale.
            inline PixelDataInfo::PROXY proxy() const;

            //! Get the width.
            inline int w() const;

            //! Get the height.
            inline int h() const;

            //! Get the pixel type.
            inline Pixel::PIXEL pixel() const;

            //! Get the number of channels.
            inline int channels() const;

            //! Get whether the pixel data is valid.
            inline bool isValid() const;

            //! Get a pointer to the data.
            inline quint8 * data();

            //! Get a pointer to the data.
            inline const quint8 * data() const;

            //! Get a pointer to the data.
            inline quint8 * data(int x, int y);

            //! Get a pointer to the data.
            inline const quint8 * data(int x, int y) const;

            //! Get the number of bytes in a pixel.
            inline quint64 pixelByteCount() const;

            //! Get the number of bytes in a scanline.
            inline quint64 scanlineByteCount() const;

            //! Get the number of bytes in the data.
            inline quint64 dataByteCount() const;

            //! Close the file I/O associated with the pixel data. This will initialize
            //! the image.
            void close();

            PixelData & operator = (const PixelData &);

        private:
            void detach();
            void copy(const PixelData &);

            PixelDataInfo       _info;
            int                 _channels = 0;
            std::vector<quint8> _data;
            const quint8 *      _p = nullptr;
            quint64             _pixelByteCount = 0;
            quint64             _scanlineByteCount = 0;
            quint64             _dataByteCount = 0;
            Core::FileIO *      _fileIo = nullptr;
        };

    } // namespace AV

    DJV_COMPARISON_OPERATOR(AV::PixelDataInfo::Mirror);
    DJV_COMPARISON_OPERATOR(AV::PixelDataInfo);
    DJV_COMPARISON_OPERATOR(AV::PixelData);

    DJV_STRING_OPERATOR(AV::PixelDataInfo::PROXY);
    DJV_STRING_OPERATOR(AV::PixelDataInfo::Mirror);

    DJV_DEBUG_OPERATOR(AV::PixelDataInfo::PROXY);
    DJV_DEBUG_OPERATOR(AV::PixelDataInfo::Mirror);
    DJV_DEBUG_OPERATOR(AV::PixelDataInfo);
    DJV_DEBUG_OPERATOR(AV::PixelData);

} // namespace djv

Q_DECLARE_METATYPE(djv::AV::PixelDataInfo)
Q_DECLARE_METATYPE(djv::AV::PixelData)

#include <djvAV/PixelDataInline.h>
