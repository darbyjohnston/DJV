//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/Pixel.h>

#include <djvCore/Memory.h>

#include <glm/vec2.hpp>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class PixelDataMirror
            {
                Q_GADGET

            public:
                inline PixelDataMirror();
                inline PixelDataMirror(bool x, bool y);

                inline bool getX() const;
                inline bool getY() const;

                inline bool operator == (const PixelDataMirror &) const;
                inline bool operator != (const PixelDataMirror &) const;

            private:
                bool _x = false;
                bool _y = false;
            };

            class PixelDataLayout
            {
                Q_GADGET

            public:
                inline PixelDataLayout();
                inline PixelDataLayout(const PixelDataMirror &, GLint alignment = 1, Core::Memory::Endian = Core::Memory::getEndian());

                inline const PixelDataMirror & getMirror() const;
                inline GLint getAlignment() const;
                inline Core::Memory::Endian getEndian() const;

                inline bool operator == (const PixelDataLayout &) const;
                inline bool operator != (const PixelDataLayout &) const;

            private:
                PixelDataMirror _mirror;
                GLint _alignment = 1;
                Core::Memory::Endian _endian = Core::Memory::getEndian();
            };

            class PixelDataInfo
            {
                Q_GADGET

            public:
                inline PixelDataInfo();
                inline PixelDataInfo(const glm::ivec2 &, const Pixel &, const PixelDataLayout & = PixelDataLayout());
                inline PixelDataInfo(int width, int height, const Pixel &, const PixelDataLayout & = PixelDataLayout());

                inline const QString & getName() const;
                inline const glm::ivec2 & getSize() const;
                inline float getAspectRatio() const;
                inline const Pixel & getPixel() const;
                inline const PixelDataLayout & getLayout() const;
                inline bool isValid() const;
                inline size_t getPixelByteCount() const;
                inline size_t getScanlineByteCount() const;
                inline size_t getDataByteCount() const;

                void setName(const QString &);

                inline bool operator == (const PixelDataInfo &) const;
                inline bool operator != (const PixelDataInfo &) const;

            private:
                QString _name;
                glm::ivec2 _size = glm::ivec2(0, 0);
                Pixel _pixel;
                PixelDataLayout _layout;
            };

            class PixelData
            {
                Q_GADGET
                    DJV_NON_COPYABLE(PixelData);

            protected:
                void _init(const PixelDataInfo &);
                PixelData();

            public:
                ~PixelData();

                static std::shared_ptr<PixelData> create(const PixelDataInfo &);

                inline const PixelDataInfo & getInfo() const;
                inline const glm::ivec2 & getSize() const;
                inline int getWidth() const;
                inline int getHeight() const;
                inline float getAspectRatio() const;
                inline const Pixel & getPixel() const;
                inline const PixelDataLayout & getLayout() const;
                inline bool isValid() const;
                inline size_t getPixelByteCount() const;
                inline size_t getScanlineByteCount() const;
                inline size_t getDataByteCount() const;

                inline const uint8_t * getData() const;
                inline const uint8_t * getData(int y) const;
                inline const uint8_t * getData(int x, int y) const;
                uint8_t * getData();
                uint8_t * getData(int y);
                uint8_t * getData(int x, int y);

                void zero();

                bool operator == (const PixelData &) const;
                bool operator != (const PixelData &) const;

            private:
                PixelDataInfo _info;
                size_t _pixelByteCount = 0;
                size_t _scanlineByteCount = 0;
                size_t _dataByteCount = 0;
                uint8_t * _data = nullptr;
                const uint8_t * _p = nullptr;
            };

        } // namespace Image
    } // namespace AV
} // namespace djv

#include <djvAV/PixelDataInline.h>
