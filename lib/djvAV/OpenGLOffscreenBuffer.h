// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/OpenGL.h>
#include <djvAV/ImageData.h>

#include <djvCore/Enum.h>

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
            DJV_ENUM_HELPERS(OffscreenDepthType);
            GLenum getInternalFormat(OffscreenDepthType);
            GLenum getGLFormat(OffscreenDepthType);
            GLenum getGLType(OffscreenDepthType);

            //! This enumeration provides OpenGL offscreen buffer sampling options.
            enum class OffscreenSampling
            {
                None,
                _2,
                _4,
                _8,
                _16,

                Count,
                First = None
            };
            DJV_ENUM_HELPERS(OffscreenSampling);

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
                explicit OffscreenBufferBinding(const std::shared_ptr<OffscreenBuffer>&);
                ~OffscreenBufferBinding();

            private:
                std::shared_ptr<OffscreenBuffer> _buffer;
            };

        } // namespace OpenGL
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::OpenGL::OffscreenDepthType);
    DJV_ENUM_SERIALIZE_HELPERS(AV::OpenGL::OffscreenSampling);

} // namespace djv

#include <djvAV/OpenGLOffscreenBufferInline.h>

