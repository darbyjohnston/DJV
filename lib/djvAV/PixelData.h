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
        namespace Pixel
        {
            class Mirror
            {
                Q_GADGET

            public:
                inline Mirror();
                inline Mirror(bool x, bool y);

                inline bool getX() const;
                inline bool getY() const;

                void setX(bool);
                void setY(bool);

                inline bool operator == (const Mirror &) const;
                inline bool operator != (const Mirror &) const;

            private:
                bool _x = false;
                bool _y = false;
            };

            class Layout
            {
                Q_GADGET

            public:
                inline Layout();
                inline Layout(const Mirror &, GLint alignment = 1, Core::Memory::Endian = Core::Memory::getEndian());

                inline const Mirror & getMirror() const;
                inline GLint getAlignment() const;
                inline Core::Memory::Endian getEndian() const;

                void setMirror(const Mirror &);
                void setAlignment(GLint);
                void setEndian(Core::Memory::Endian);

                inline bool operator == (const Layout &) const;
                inline bool operator != (const Layout &) const;

            private:
                Mirror _mirror;
                GLint _alignment = 1;
                Core::Memory::Endian _endian = Core::Memory::getEndian();
            };

            class Info
            {
                Q_GADGET

            public:
                inline Info();
                inline Info(const glm::ivec2 &, Type, const Layout & = Layout());
                inline Info(int width, int height, Type, const Layout & = Layout());

                inline const std::string & getName() const;
                inline const glm::ivec2 & getSize() const;
                inline int getWidth() const;
                inline int getHeight() const;
                inline float getAspectRatio() const;
                inline Type getType() const;
                inline GLenum getGLFormat() const;
                inline GLenum getGLType() const;
                inline const Layout & getLayout() const;
                inline bool isValid() const;
                inline size_t getPixelByteCount() const;
                inline size_t getScanlineByteCount() const;
                inline size_t getDataByteCount() const;

                void setName(const std::string &);

                inline bool operator == (const Info &) const;
                inline bool operator != (const Info &) const;

            private:
                std::string _name;
                glm::ivec2 _size = glm::ivec2(0, 0);
                Type _type;
                Layout _layout;
            };

            class Data
            {
                Q_GADGET
                DJV_NON_COPYABLE(Data);

            protected:
                void _init(const Info &);
                inline Data();

            public:
                ~Data();

                static std::shared_ptr<Data> create(const Info &);

                inline const Info & getInfo() const;
                inline const glm::ivec2 & getSize() const;
                inline int getWidth() const;
                inline int getHeight() const;
                inline float getAspectRatio() const;
                inline Type getType() const;
                inline GLenum getGLFormat() const;
                inline GLenum getGLType() const;
                inline const Layout & getLayout() const;
                inline bool isValid() const;
                inline size_t getPixelByteCount() const;
                inline size_t getScanlineByteCount() const;
                inline size_t getDataByteCount() const;

                inline const uint8_t * getData() const;
                inline const uint8_t * getData(int y) const;
                inline const uint8_t * getData(int x, int y) const;
                inline uint8_t * getData();
                inline uint8_t * getData(int y);
                inline uint8_t * getData(int x, int y);

                void zero();

                bool operator == (const Data &) const;
                bool operator != (const Data &) const;

            private:
                Info _info;
                size_t _pixelByteCount = 0;
                size_t _scanlineByteCount = 0;
                size_t _dataByteCount = 0;
                uint8_t * _data = nullptr;
                const uint8_t * _p = nullptr;
            };

        } // namespace Pixel
    } // namespace AV
} // namespace djv

#include <djvAV/PixelDataInline.h>
