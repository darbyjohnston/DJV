// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <memory>

namespace djv
{
    namespace System
    {
        class TextSystem;
        class ResourceSystem;

    } // namespace System

    namespace Image
    {
        class Data;
        class Info;
    
    } // namespace Image

    namespace GL
    {
        //! Image data conversion.
        class ImageConvert
        {
            DJV_NON_COPYABLE(ImageConvert);

        protected:
            void _init(
                const std::shared_ptr<System::TextSystem>&,
                const std::shared_ptr<System::ResourceSystem>&);
            ImageConvert();

        public:
            ~ImageConvert();

            //! Note that this function requires an active OpenGL context.
            //! Throws:
            //! - ShaderError
            static std::shared_ptr<ImageConvert> create(
                const std::shared_ptr<System::TextSystem>&,
                const std::shared_ptr<System::ResourceSystem>&);

            //! Note that this function requires an active OpenGL context.
            //! Throws:
            //! - OffscreenBufferError
            void process(const Image::Data&, const Image::Info&, Image::Data&);

        private:
            DJV_PRIVATE();
        };

    } // namespace GL
} // namespace djv
