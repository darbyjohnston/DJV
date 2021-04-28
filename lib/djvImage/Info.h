// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Type.h>

#include <djvCore/Memory.h>

#include <glad.h>

#if defined(GetObject)
#undef GetObject
#endif // GetObject

namespace djv
{
    namespace Image
    {
        //! Image mirroring.
        class Mirror
        {
        public:
            constexpr Mirror() noexcept;
            constexpr Mirror(bool x, bool y) noexcept;

            bool x = false;
            bool y = false;

            constexpr bool operator == (const Mirror&) const noexcept;
            constexpr bool operator != (const Mirror&) const noexcept;
        };

        //! Image data layout.
        class Layout
        {
        public:
            Layout() noexcept;
            constexpr Layout(const Mirror&, GLint alignment = 1, Core::Memory::Endian = Core::Memory::getEndian()) noexcept;

            Mirror                  mirror;
            GLint                   alignment   = 1;
            Core::Memory::Endian    endian      = Core::Memory::getEndian();

            constexpr bool operator == (const Layout&) const noexcept;
            constexpr bool operator != (const Layout&) const noexcept;
        };

        //! Image size.
        class Size
        {
        public:
            Size(uint16_t w = 0, uint16_t h = 0) noexcept;

            uint16_t w = 0;
            uint16_t h = 0;

            bool isValid() const noexcept;
            float getAspectRatio() const noexcept;

            bool operator == (const Size&) const noexcept;
            bool operator != (const Size&) const noexcept;
        };

        //! The default name.
        const std::string defaultName = "Video";
        
        //! Image information.
        class Info
        {
        public:
            Info();
            Info(const Size&, Type, const Layout& = Layout());
            Info(uint16_t width, uint16_t height, Type, const Layout& = Layout());

            std::string name                = defaultName;
            Size        size;
            float       pixelAspectRatio    = 1.F;
            Type        type                = Type::None;
            Layout      layout;
            std::string codec;

            float getAspectRatio() const noexcept;
            GLenum getGLFormat() const noexcept;
            GLenum getGLType() const noexcept;
            bool isValid() const noexcept;
            size_t getPixelByteCount() const noexcept;
            size_t getScanlineByteCount() const noexcept;
            size_t getDataByteCount() const noexcept;

            bool operator == (const Info&) const;
            bool operator != (const Info&) const;
        };

    } // namespace Image

    std::ostream& operator << (std::ostream&, const Image::Size&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Image::Size&);

    rapidjson::Value toJSON(const Image::Size&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Image::Mirror&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Image::Size&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Image::Mirror&);

} // namespace djv

#include <djvImage/InfoInline.h>
