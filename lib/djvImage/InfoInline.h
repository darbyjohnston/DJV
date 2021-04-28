// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/Type.h>

namespace djv
{
    namespace Image
    {
        constexpr Mirror::Mirror() noexcept
        {}

        constexpr Mirror::Mirror(bool x, bool y) noexcept :
            x(x),
            y(y)
        {}

        constexpr bool Mirror::operator == (const Mirror& other) const noexcept
        {
            return other.x == x && other.y == y;
        }

        constexpr bool Mirror::operator != (const Mirror& other) const noexcept
        {
            return !(other == *this);
        }

        constexpr Layout::Layout(const Mirror& mirror, GLint alignment, Core::Memory::Endian endian) noexcept :
            mirror(mirror),
            alignment(alignment),
            endian(endian)
        {}

        constexpr bool Layout::operator == (const Layout& other) const noexcept
        {
            return other.mirror == mirror && other.alignment == alignment && other.endian == endian;
        }

        constexpr bool Layout::operator != (const Layout& other) const noexcept
        {
            return !(other == *this);
        }

        inline bool Size::isValid() const noexcept
        {
            return w > 0 && h > 0;
        }

        inline float Size::getAspectRatio() const noexcept
        {
            return h > 0 ? (w / static_cast<float>(h)) : 1.F;
        }

        inline bool Size::operator == (const Size& other) const noexcept
        {
            return w == other.w && h == other.h;
        }

        inline bool Size::operator != (const Size& other) const noexcept
        {
            return !(*this == other);
        }

        inline float Info::getAspectRatio() const noexcept
        {
            return size.getAspectRatio();
        }

        inline GLenum Info::getGLFormat() const noexcept
        {
            return djv::Image::getGLFormat(type);
        }

        inline GLenum Info::getGLType() const noexcept
        {
            return djv::Image::getGLType(type);
        }

        inline bool Info::isValid() const noexcept
        {
            return size.w > 0 && size.h > 0 && type != Type::None;
        }

        inline size_t Info::getPixelByteCount() const noexcept
        {
            return djv::Image::getByteCount(type);
        }

        inline size_t Info::getScanlineByteCount() const noexcept
        {
            const size_t byteCount = static_cast<size_t>(size.w) * djv::Image::getByteCount(type);
            const size_t q = byteCount / layout.alignment * layout.alignment;
            const size_t r = byteCount - q;
            return q + (r ? layout.alignment : 0);
        }

        inline size_t Info::getDataByteCount() const noexcept
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

    } // namespace Image
} // namespace djv
