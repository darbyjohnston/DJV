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

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            inline PixelDataMirror::PixelDataMirror()
            {}

            inline PixelDataMirror::PixelDataMirror(bool x, bool y) :
                _x(x),
                _y(y)
            {}

            inline bool PixelDataMirror::getX() const
            {
                return _x;
            }

            inline bool PixelDataMirror::getY() const
            {
                return _y;
            }

            inline bool PixelDataMirror::operator == (const PixelDataMirror & other) const
            {
                return other._x == _x && other._y == _y;
            }

            inline bool PixelDataMirror::operator != (const PixelDataMirror & other) const
            {
                return !(other == *this);
            }

            inline PixelDataLayout::PixelDataLayout()
            {}

            inline PixelDataLayout::PixelDataLayout(const PixelDataMirror & mirror, GLint alignment, Core::Memory::Endian endian) :
                _mirror(mirror),
                _alignment(alignment),
                _endian(endian)
            {}

            inline const PixelDataMirror & PixelDataLayout::getMirror() const
            {
                return _mirror;
            }

            inline GLint PixelDataLayout::getAlignment() const
            {
                return _alignment;
            }

            inline Core::Memory::Endian PixelDataLayout::getEndian() const
            {
                return _endian;
            }

            inline bool PixelDataLayout::operator == (const PixelDataLayout & other) const
            {
                return other._mirror == _mirror && other._alignment == _alignment && other._endian == _endian;
            }

            inline bool PixelDataLayout::operator != (const PixelDataLayout & other) const
            {
                return !(other == *this);
            }

            inline PixelDataInfo::PixelDataInfo()
            {}

            inline PixelDataInfo::PixelDataInfo(const glm::ivec2 & size, const Pixel & pixel, const PixelDataLayout & layout) :
                _size(size),
                _pixel(pixel),
                _layout(layout)
            {}

            inline PixelDataInfo::PixelDataInfo(int width, int height, const Pixel& pixel, const PixelDataLayout & layout) :
                _size(width, height),
                _pixel(pixel),
                _layout(layout)
            {}

            inline const QString & PixelDataInfo::getName() const
            {
                return _name;
            }

            inline const glm::ivec2 & PixelDataInfo::getSize() const
            {
                return _size;
            }

            inline float PixelDataInfo::getAspectRatio() const
            {
                return _size.y > 0 ? (_size.x / static_cast<float>(_size.y)) : 1.f;
            }

            inline const Pixel & PixelDataInfo::getPixel() const
            {
                return _pixel;
            }

            inline const PixelDataLayout & PixelDataInfo::getLayout() const
            {
                return _layout;
            }

            inline bool PixelDataInfo::isValid() const
            {
                return _size.x > 0 && _size.y > 0 && _pixel.isValid();
            }

            inline size_t PixelDataInfo::getPixelByteCount() const
            {
                return _pixel.getByteCount();
            }

            inline size_t PixelDataInfo::getScanlineByteCount() const
            {
                const size_t byteCount = _size.x * _pixel.getByteCount();
                const size_t q = byteCount / _layout.getAlignment() * _layout.getAlignment();
                const size_t r = byteCount - q;
                return q + (r ? _layout.getAlignment() : 0);
            }

            inline size_t PixelDataInfo::getDataByteCount() const
            {
                return _size.y * getScanlineByteCount();
            }

            inline bool PixelDataInfo::operator == (const PixelDataInfo& other) const
            {
                return
                    other._size == _size &&
                    other._pixel == _pixel &&
                    other._layout == _layout;
            }

            inline bool PixelDataInfo::operator != (const PixelDataInfo& other) const
            {
                return !(other == *this);
            }

            inline bool PixelData::isValid() const
            {
                return _info.isValid();
            }

            inline const PixelDataInfo & PixelData::getInfo() const
            {
                return _info;
            }

            inline const glm::ivec2 & PixelData::getSize() const
            {
                return _info.getSize();
            }

            inline int PixelData::getWidth() const
            {
                return _info.getSize().x;
            }

            inline int PixelData::getHeight() const
            {
                return _info.getSize().y;
            }

            inline float PixelData::getAspectRatio() const
            {
                return _info.getAspectRatio();
            }

            inline const Pixel& PixelData::getPixel() const
            {
                return _info.getPixel();
            }

            inline const PixelDataLayout & PixelData::getLayout() const
            {
                return _info.getLayout();
            }

            inline size_t PixelData::getPixelByteCount() const
            {
                return _info.getPixelByteCount();
            }

            inline size_t PixelData::getScanlineByteCount() const
            {
                return _info.getScanlineByteCount();
            }

            inline size_t PixelData::getDataByteCount() const
            {
                return _info.getDataByteCount();
            }

            inline const uint8_t * PixelData::getData() const
            {
                return _p;
            }

            inline const uint8_t * PixelData::getData(int y) const
            {
                return _p + y * _scanlineByteCount;
            }

            inline const uint8_t * PixelData::getData(int x, int y) const
            {
                return _p + y * _scanlineByteCount + x * _pixelByteCount;
            }

        } // namespace Image
    } // namespace AV
} // namespace djv
