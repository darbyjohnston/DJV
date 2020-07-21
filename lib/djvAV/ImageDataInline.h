// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            constexpr Mirror::Mirror()
            {}

            constexpr Mirror::Mirror(bool x, bool y) :
                x(x),
                y(y)
            {}

            constexpr bool Mirror::operator == (const Mirror& other) const
            {
                return other.x == x && other.y == y;
            }

            constexpr bool Mirror::operator != (const Mirror& other) const
            {
                return !(other == *this);
            }

            inline Layout::Layout()
            {}

            constexpr Layout::Layout(const Mirror& mirror, GLint alignment, Core::Memory::Endian endian) :
                mirror(mirror),
                alignment(alignment),
                endian(endian)
            {}

            constexpr bool Layout::operator == (const Layout& other) const
            {
                return other.mirror == mirror && other.alignment == alignment && other.endian == endian;
            }

            constexpr bool Layout::operator != (const Layout& other) const
            {
                return !(other == *this);
            }

            inline Size::Size(uint16_t w, uint16_t h) :
                w(w),
                h(h)
            {}

            inline bool Size::isValid() const
            {
                return w > 0 && h > 0;
            }

            inline float Size::getAspectRatio() const
            {
                return h > 0 ? (w / static_cast<float>(h)) : 1.F;
            }

            inline bool Size::operator == (const Size& other) const
            {
                return w == other.w && h == other.h;
            }

            inline bool Size::operator != (const Size& other) const
            {
                return !(*this == other);
            }

            inline Info::Info()
            {}

            inline Info::Info(const Size& size, Type type, const Layout& layout) :
                size(size),
                type(type),
                layout(layout)
            {}

            inline Info::Info(uint16_t width, uint16_t height, Type type, const Layout& layout) :
                size(width, height),
                type(type),
                layout(layout)
            {}

            inline float Info::getAspectRatio() const
            {
                return size.getAspectRatio();
            }

            inline GLenum Info::getGLFormat() const
            {
                return AV::Image::getGLFormat(type);
            }

            inline GLenum Info::getGLType() const
            {
                return AV::Image::getGLType(type);
            }

            inline bool Info::isValid() const
            {
                return size.w > 0 && size.h > 0 && type != Type::None;
            }

            inline size_t Info::getPixelByteCount() const
            {
                return AV::Image::getByteCount(type);
            }

            inline size_t Info::getScanlineByteCount() const
            {
                const size_t byteCount = static_cast<size_t>(size.w) * AV::Image::getByteCount(type);
                const size_t q = byteCount / layout.alignment * layout.alignment;
                const size_t r = byteCount - q;
                return q + (r ? layout.alignment : 0);
            }

            inline size_t Info::getDataByteCount() const
            {
                return size.h * getScanlineByteCount();
            }

            inline bool Info::operator == (const Info& other) const
            {
                return
                    name == other.name &&
                    size.w == other.size.w &&
                    size.h == other.size.h &&
                    pixelAspectRatio == other.pixelAspectRatio &&
                    type == other.type &&
                    layout == other.layout &&
                    codec == other.codec;
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

            inline const Info& Data::getInfo() const
            {
                return _info;
            }

            inline const Size& Data::getSize() const
            {
                return _info.size;
            }

            inline uint16_t Data::getWidth() const
            {
                return _info.size.w;
            }

            inline uint16_t Data::getHeight() const
            {
                return _info.size.h;
            }

            inline float Data::getAspectRatio() const
            {
                return _info.getAspectRatio();
            }

            inline Type Data::getType() const
            {
                return _info.type;
            }

            inline GLenum Data::getGLFormat() const
            {
                return _info.getGLFormat();
            }

            inline GLenum Data::getGLType() const
            {
                return _info.getGLType();
            }

            inline const Layout& Data::getLayout() const
            {
                return _info.layout;
            }

            inline uint8_t Data::getPixelByteCount() const
            {
                return _pixelByteCount;
            }

            inline size_t Data::getScanlineByteCount() const
            {
                return _scanlineByteCount;
            }

            inline const uint8_t* Data::getData() const
            {
                return _p;
            }

            inline const uint8_t* Data::getData(uint16_t y) const
            {
                return _p + y * _scanlineByteCount;
            }

            inline const uint8_t* Data::getData(uint16_t x, uint16_t y) const
            {
                return _p + y * _scanlineByteCount + x * static_cast<size_t>(_pixelByteCount);
            }

            inline uint8_t* Data::getData()
            {
#if defined(DJV_MMAP)
                detach();
#endif // DJV_MMAP
                return _data;
            }

            inline uint8_t* Data::getData(uint16_t y)
            {
#if defined(DJV_MMAP)
                detach();
#endif // DJV_MMAP
                return _data + y * _scanlineByteCount;
            }

            inline uint8_t* Data::getData(uint16_t x, uint16_t y)
            {
#if defined(DJV_MMAP)
                detach();
#endif // DJV_MMAP
                return _data + y * _scanlineByteCount + x * static_cast<size_t>(_pixelByteCount);
            }

        } // namespace Image
    } // namespace AV
} // namespace djv
