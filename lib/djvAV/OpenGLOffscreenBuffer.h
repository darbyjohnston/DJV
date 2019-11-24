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

#pragma once

#include <djvAV/OpenGL.h>
#include <djvAV/ImageData.h>

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            //! This enumeration provides OpenGL offscreen depth buffer types.
            enum class OffscreenDepthType
            {
                None,
                _24,
                _32,

                Count,
                First = None
            };
            GLenum getInternalFormat(OffscreenDepthType);
            GLenum getGLFormat(OffscreenDepthType);
            GLenum getGLType(OffscreenDepthType);

            //! This enumeration provides OpenGL offscreen buffer sampling options.
            enum class OffscreenSampling
            {
                None,
                MultiSample
            };

            //! This class provides an OpenGL offscreen buffer error.
            class OffscreenBufferError : public std::runtime_error
            {
            public:
                explicit OffscreenBufferError(const std::string&);
            };

            //! This class provides an OpenGL offscreen buffer.
            class OffscreenBuffer : public std::enable_shared_from_this<OffscreenBuffer>
            {
                DJV_NON_COPYABLE(OffscreenBuffer);
                void _init(
                    const Image::Size&,
                    Image::Type,
                    OffscreenDepthType,
                    OffscreenSampling);
                OffscreenBuffer();

            public:
                ~OffscreenBuffer();

                //! Create a new offscreen buffer.
                //! Throws:
                //! - OffscreenBufferError
                static std::shared_ptr<OffscreenBuffer> create(
                    const Image::Size&,
                    Image::Type,
                    OffscreenDepthType = OffscreenDepthType::None,
                    OffscreenSampling = OffscreenSampling::None);

                const Image::Size& getSize() const;
                Image::Type getColorType() const;
                OffscreenDepthType getDepthType() const;
                OffscreenSampling getSampling() const;
                GLuint getID() const;
                GLuint getColorID() const;
                GLuint getDepthID() const;

                void bind();

            private:
                Image::Size _size;
                Image::Type _colorType = Image::Type::None;
                OffscreenDepthType _depthType = OffscreenDepthType::None;
                OffscreenSampling _sampling = OffscreenSampling::None;
                GLuint _id = 0;
                GLuint _colorID = 0;
                GLuint _depthID = 0;
            };

            //! This class provides a wrapper for automatically binding and unbinding an
            //! OpenGL offscreen buffer.
            class OffscreenBufferBinding
            {
            public:
                explicit OffscreenBufferBinding(const std::shared_ptr<OffscreenBuffer> &);
                ~OffscreenBufferBinding();

            private:
                std::shared_ptr<OffscreenBuffer> _buffer;
            };

        } // namespace OpenGL
    } // namespace AV
} // namespace djv

#include <djvAV/OpenGLOffscreenBufferInline.h>

