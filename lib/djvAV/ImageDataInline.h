//------------------------------------------------------------------------------
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

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            inline Mirror::Mirror()
            {}

            inline Mirror::Mirror(bool x, bool y) :
                x(x),
                y(y)
            {}

            inline bool Mirror::operator == (const Mirror & other) const
            {
                return other.x == x && other.y == y;
            }

            inline bool Mirror::operator != (const Mirror & other) const
            {
                return !(other == *this);
            }

            inline Layout::Layout()
            {}

            inline Layout::Layout(const Mirror & mirror, gl::GLint alignment, Core::Memory::Endian endian) :
                mirror(mirror),
                alignment(alignment),
                endian(endian)
            {}

            inline bool Layout::operator == (const Layout & other) const
            {
                return other.mirror == mirror && other.alignment == alignment && other.endian == endian;
            }

            inline bool Layout::operator != (const Layout & other) const
            {
                return !(other == *this);
            }

            inline Info::Info()
            {}

            inline Info::Info(const glm::ivec2 & size, Type type, const Layout & layout) :
                size(size),
                type(type),
                layout(layout)
            {}

            inline Info::Info(int width, int height, Type type, const Layout & layout) :
                size(width, height),
                type(type),
                layout(layout)
            {}

            inline float Info::getAspectRatio() const
            {
                return size.y > 0 ? (size.x / static_cast<float>(size.y)) : 1.f;
            }

            inline gl::GLenum Info::getGLFormat() const
            {
                return AV::Image::getGLFormat(type);
            }

            inline gl::GLenum Info::getGLType() const
            {
                return AV::Image::getGLType(type);
            }

            inline bool Info::isValid() const
            {
                return size.x > 0 && size.y > 0 && type != Type::None;
            }

            inline size_t Info::getPixelByteCount() const
            {
                return AV::Image::getByteCount(type);
            }

            inline size_t Info::getScanlineByteCount() const
            {
                const size_t byteCount = size.x * AV::Image::getByteCount(type);
                const size_t q = byteCount / layout.alignment * layout.alignment;
                const size_t r = byteCount - q;
                return q + (r ? layout.alignment : 0);
            }

            inline size_t Info::getDataByteCount() const
            {
                return size.y * getScanlineByteCount();
            }

            inline bool Info::operator == (const Info& other) const
            {
                return
                    other.size == size &&
                    other.type == type &&
                    other.layout == layout;
            }

            inline bool Info::operator != (const Info& other) const
            {
                return !(other == *this);
            }

            inline Data::Data()
            {}

            inline Core::UID Data::getUID() const
            {
                return _uid;
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
                return _info.size;
            }

            inline int Data::getWidth() const
            {
                return _info.size.x;
            }

            inline int Data::getHeight() const
            {
                return _info.size.y;
            }

            inline float Data::getAspectRatio() const
            {
                return _info.getAspectRatio();
            }

            inline Type Data::getType() const
            {
                return _info.type;
            }

            inline gl::GLenum Data::getGLFormat() const
            {
                return _info.getGLFormat();
            }

            inline gl::GLenum Data::getGLType() const
            {
                return _info.getGLType();
            }

            inline const Layout & Data::getLayout() const
            {
                return _info.layout;
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

            inline uint8_t * Data::getData()
            {
                return _data;
            }

            inline uint8_t* Data::getData(int y)
            {
                return _data + y * _scanlineByteCount;
            }

            inline uint8_t * Data::getData(int x, int y)
            {
                return _data + y * _scanlineByteCount + x * _pixelByteCount;
            }

        } // namespace Image
    } // namespace AV
} // namespace djv
