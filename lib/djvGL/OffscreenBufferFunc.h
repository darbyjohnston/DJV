// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvGL/OffscreenBuffer.h>

#include <djvCore/Enum.h>

namespace djv
{
    namespace GL
    {
        GLenum getInternalFormat(OffscreenDepthType);
        GLenum getGLFormat(OffscreenDepthType);
        GLenum getGLType(OffscreenDepthType);

        DJV_ENUM_HELPERS(OffscreenDepthType);
        DJV_ENUM_HELPERS(OffscreenSampling);

    } // namespace GL

    DJV_ENUM_SERIALIZE_HELPERS(GL::OffscreenDepthType);
    DJV_ENUM_SERIALIZE_HELPERS(GL::OffscreenSampling);

} // namespace djv
