// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvGL/GL.h>

#include <djvImage/Type.h>

namespace djv
{
    namespace GL
    {
        //! \name Information
        ///@{
        /// 
        GLenum getInternalFormat2D(Image::Type);
        //GLenum getInternalFormat1D(Image::Type);

        ///@}

    } // namespace GL
} // namespace djv
