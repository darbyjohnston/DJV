// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/SceneWidgetFunc.h>

#include <array>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        DJV_ENUM_HELPERS_IMPLEMENTATION(SceneRotate);

    } // namespace UIComponents

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UIComponents,
        SceneRotate,
        DJV_TEXT("scene_rotate_none"),
        DJV_TEXT("scene_rotate_x_+90"),
        DJV_TEXT("scene_rotate_x_-90"),
        DJV_TEXT("scene_rotate_y_+90"),
        DJV_TEXT("scene_rotate_y_-90"),
        DJV_TEXT("scene_rotate_z_+90"),
        DJV_TEXT("scene_rotate_z_-90"));

} // namespace djv
