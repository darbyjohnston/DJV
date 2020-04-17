// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/ImageData.h>

namespace djv
{
    namespace Core
    {
        class ResourceSystem;

    } // namespace Core

    namespace AV
    {
        namespace Image
        {
            //! This class provides image data conversion.
            class Convert
            {
                DJV_NON_COPYABLE(Convert);

            protected:
                void _init(const std::shared_ptr<Core::ResourceSystem>&);
                Convert();

            public:
                ~Convert();

                //! Note that this function requires an OpenGL context.
                //! Throws:
                //! - OpenGL::ShaderError
                //! - Render::ShaderError
                static std::shared_ptr<Convert> create(const std::shared_ptr<Core::ResourceSystem>&);

                //! Note that this function requires an OpenGL context.
                //! Throws:
                //! - OpenGL::OffscreenBufferError
                void process(const Data&, const Info&, Data&);

            private:
                DJV_PRIVATE();
            };

        } // namespace Image
    } // namespace AV
} // namespace djv
