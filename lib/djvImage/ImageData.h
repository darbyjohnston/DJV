// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Pixel.h>

#include <djvCore/MemoryFunc.h>
#include <djvCore/UID.h>

#include <memory>

namespace djv
{
    namespace Image
    {
        //! This class provides information about mirroring the image.
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

        //! This class provides information about the data layout.
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

        //! This class provides the image size.
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

        //! This constant provides the default name.
        const std::string defaultName = "Video";
        
        //! This class provides image data information.
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

        //! This class provides image data.
        class Data
        {
            DJV_NON_COPYABLE(Data);

        protected:
            void _init(const Info&);
            Data();

        public:
            ~Data();

            static std::shared_ptr<Data> create(const Info&);

            Core::UID getUID() const;

            const Info& getInfo() const;
            const Size& getSize() const;
            uint16_t getWidth() const;
            uint16_t getHeight() const;
            float getAspectRatio() const;

            Type getType() const;
            GLenum getGLFormat() const;
            GLenum getGLType() const;
            const Layout& getLayout() const;

            bool isValid() const;
            uint8_t getPixelByteCount() const;
            size_t getScanlineByteCount() const;
            size_t getDataByteCount() const;

            const uint8_t* getData() const;
            const uint8_t* getData(uint16_t y) const;
            const uint8_t* getData(uint16_t x, uint16_t y) const;
            uint8_t* getData();
            uint8_t* getData(uint16_t y);
            uint8_t* getData(uint16_t x, uint16_t y);

            void zero();

            bool operator == (const Data&) const;
            bool operator != (const Data&) const;

        private:
            Core::UID _uid = 0;
            Info _info;
            uint8_t _pixelByteCount = 0;
            size_t _scanlineByteCount = 0;
            size_t _dataByteCount = 0;
            uint8_t* _data = nullptr;
            const uint8_t* _p = nullptr;
        };

    } // namespace Image
} // namespace djv

#include <djvImage/ImageDataInline.h>
