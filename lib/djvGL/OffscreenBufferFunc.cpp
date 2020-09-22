// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvGL/OffscreenBufferFunc.h>

#include <array>

using namespace djv::Core;

namespace djv
{
    namespace GL
    {
        GLenum getInternalFormat(OffscreenDepthType value)
        {
            const std::array<GLenum, static_cast<size_t>(OffscreenDepthType::Count)> data =
            {
                GL_NONE,
#if !defined(DJV_GL_ES2)
                GL_DEPTH_COMPONENT24,
#else // DJV_GL_ES2
                GL_DEPTH_COMPONENT,
#endif // DJV_GL_ES2
#if !defined(DJV_GL_ES2)
                GL_DEPTH_COMPONENT32F
#else // DJV_GL_ES2
                GL_DEPTH_COMPONENT
#endif // DJV_GL_ES2
            };
            return data[static_cast<size_t>(value)];
        }

        GLenum getGLFormat(OffscreenDepthType value)
        {
            const std::array<GLenum, static_cast<size_t>(OffscreenDepthType::Count)> data =
            {
                GL_NONE,
                GL_DEPTH_COMPONENT,
                GL_DEPTH_COMPONENT
            };
            return data[static_cast<size_t>(value)];
        }

        GLenum getGLType(OffscreenDepthType value)
        {
            const std::array<GLenum, static_cast<size_t>(OffscreenDepthType::Count)> data =
            {
                GL_NONE,
                GL_UNSIGNED_INT,
                GL_FLOAT
            };
            return data[static_cast<size_t>(value)];
        }

        DJV_ENUM_HELPERS_IMPLEMENTATION(OffscreenDepthType);
        DJV_ENUM_HELPERS_IMPLEMENTATION(OffscreenSampling);

    } // namespace GL

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        GL,
        OffscreenDepthType,
        DJV_TEXT("offscreen_depth_type_none"),
        DJV_TEXT("offscreen_depth_type_24"),
        DJV_TEXT("offscreen_depth_type_32"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        GL,
        OffscreenSampling,
        DJV_TEXT("offscreen_sampling_none"),
        DJV_TEXT("offscreen_sampling_2"),
        DJV_TEXT("offscreen_sampling_4"),
        DJV_TEXT("offscreen_sampling_8"),
        DJV_TEXT("offscreen_sampling_16"));

} // namespace djv
