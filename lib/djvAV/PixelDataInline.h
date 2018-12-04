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
        namespace Pixel
        {
            inline Mirror::Mirror()
            {}

            inline Mirror::Mirror(bool x, bool y) :
                _x(x),
                _y(y)
            {}

            inline bool Mirror::getX() const
            {
                return _x;
            }

            inline bool Mirror::getY() const
            {
                return _y;
            }

            inline bool Mirror::operator == (const Mirror & other) const
            {
                return other._x == _x && other._y == _y;
            }

            inline bool Mirror::operator != (const Mirror & other) const
            {
                return !(other == *this);
            }

            inline Layout::Layout()
            {}

            inline Layout::Layout(const Mirror & mirror, GLint alignment, Core::Memory::Endian endian) :
                _mirror(mirror),
                _alignment(alignment),
                _endian(endian)
            {}

            inline const Mirror & Layout::getMirror() const
            {
                return _mirror;
            }

            inline GLint Layout::getAlignment() const
            {
                return _alignment;
            }

            inline Core::Memory::Endian Layout::getEndian() const
            {
                return _endian;
            }

            inline bool Layout::operator == (const Layout & other) const
            {
                return other._mirror == _mirror && other._alignment == _alignment && other._endian == _endian;
            }

            inline bool Layout::operator != (const Layout & other) const
            {
                return !(other == *this);
            }

            inline Info::Info()
            {}

            inline Info::Info(const glm::ivec2 & size, Type type, const Layout & layout) :
                _size(size),
                _type(type),
                _layout(layout)
            {}

            inline Info::Info(int width, int height, Type type, const Layout & layout) :
                _size(width, height),
                _type(type),
                _layout(layout)
            {}

            inline const std::string & Info::getName() const
            {
                return _name;
            }

            inline const glm::ivec2 & Info::getSize() const
            {
                return _size;
            }

            inline int Info::getWidth() const
            {
                return _size.x;
            }

            inline int Info::getHeight() const
            {
                return _size.y;
            }

            inline float Info::getAspectRatio() const
            {
                return _size.y > 0 ? (_size.x / static_cast<float>(_size.y)) : 1.f;
            }

            inline Type Info::getType() const
            {
                return _type;
            }

            inline GLenum Info::getGLFormat() const
            {
                return Pixel::getGLFormat(_type);
            }

            inline GLenum Info::getGLType() const
            {
                return Pixel::getGLType(_type);
            }

            inline const Layout & Info::getLayout() const
            {
                return _layout;
            }

            inline bool Info::isValid() const
            {
                return _size.x > 0 && _size.y > 0 && _type != Type::None;
            }

            inline size_t Info::getPixelByteCount() const
            {
                return Pixel::getByteCount(_type);
            }

            inline size_t Info::getScanlineByteCount() const
            {
                const size_t byteCount = _size.x * Pixel::getByteCount(_type);
                const size_t q = byteCount / _layout.getAlignment() * _layout.getAlignment();
                const size_t r = byteCount - q;
                return q + (r ? _layout.getAlignment() : 0);
            }

            inline size_t Info::getDataByteCount() const
            {
                return _size.y * getScanlineByteCount();
            }

            inline bool Info::operator == (const Info& other) const
            {
                return
                    other._size == _size &&
                    other._type == _type &&
                    other._layout == _layout;
            }

            inline bool Info::operator != (const Info& other) const
            {
                return !(other == *this);
            }

            inline bool Data::isValid() const
            {
                return _info.isValid();
            }

            inline const Info & Data::getInfo() const
            {
                return _info;
            }

            inline const glm::ivec2 & Data::getSize() const
            {
                return _info.getSize();
            }

            inline int Data::getWidth() const
            {
                return _info.getSize().x;
            }

            inline int Data::getHeight() const
            {
                return _info.getSize().y;
            }

            inline float Data::getAspectRatio() const
            {
                return _info.getAspectRatio();
            }

            inline Type Data::getType() const
            {
                return _info.getType();
            }

            inline GLenum Data::getGLFormat() const
            {
                return _info.getGLFormat();
            }

            inline GLenum Data::getGLType() const
            {
                return _info.getGLType();
            }

            inline const Layout & Data::getLayout() const
            {
                return _info.getLayout();
            }

            inline size_t Data::getPixelByteCount() const
            {
                return _info.getPixelByteCount();
            }

            inline size_t Data::getScanlineByteCount() const
            {
                return _info.getScanlineByteCount();
            }

            inline size_t Data::getDataByteCount() const
            {
                return _info.getDataByteCount();
            }

            inline const uint8_t * Data::getData() const
            {
                return _p;
            }

            inline const uint8_t * Data::getData(int y) const
            {
                return _p + y * _scanlineByteCount;
            }

            inline const uint8_t * Data::getData(int x, int y) const
            {
                return _p + y * _scanlineByteCount + x * _pixelByteCount;
            }

        } // namespace Pixel
    } // namespace AV
} // namespace djv
