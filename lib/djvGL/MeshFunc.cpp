// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvGL/MeshFunc.h>

#include <array>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace GL
    {
        size_t getVertexByteCount(VBOType value) noexcept
        {
            const std::array<size_t, static_cast<size_t>(VBOType::Count)> data =
            {
                12, // 2 * sizeof(float) + 2 * sizeof(uint16_t)
                12, // 3 * sizeof(float)
                16, // 3 * sizeof(float) + 2 * sizeof(uint16_t)
                20, // 3 * sizeof(float) + 2 * sizeof(uint16_t) + sizeof(PackedNormal)
                24, // 3 * sizeof(float) + 2 * sizeof(uint16_t) + sizeof(PackedNormal) + sizeof(PackedColor)
                32, // 3 * sizeof(float) + 2 * sizeof(float) + 3 * sizeof(float)
                44, // 3 * sizeof(float) + 2 * sizeof(float) + 3 * sizeof(float) + 3 * sizeof(float)
                16  // 3 * sizeof(float) + sizeof(PackedColor)
            };
            return data[static_cast<size_t>(value)];
        }

        DJV_ENUM_HELPERS_IMPLEMENTATION(VBOType);

    } // namespace GL

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        GL,
        VBOType,
        DJV_TEXT("vbo_type_pos2_f32_uv_u16"),
        DJV_TEXT("vbo_type_pos3_f32"),
        DJV_TEXT("vbo_type_pos3_f32_uv_u16"),
        DJV_TEXT("vbo_type_pos3_f32_uv_u16_normal_u10"),
        DJV_TEXT("vbo_type_pos3_f32_uv_u16_normal_u10_color_u8"),
        DJV_TEXT("vbo_type_pos3_f32_uv_f32_normal_f32"),
        DJV_TEXT("vbo_type_pos3_f32_uv_f32_normal_f32_color_f32"),
        DJV_TEXT("vbo_type_pos3_f32_u8"));

} // namespace djv

