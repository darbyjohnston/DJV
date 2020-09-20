// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/GridLayoutFunc.h>

#include <array>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            DJV_ENUM_HELPERS_IMPLEMENTATION(GridStretch);

        } // namespace Layout
    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::Layout,
        GridStretch,
        DJV_TEXT("layout_grid_stretch_none"),
        DJV_TEXT("layout_grid_stretch_horizontal"),
        DJV_TEXT("layout_grid_stretch_vertical"),
        DJV_TEXT("layout_grid_stretch_both"));

} // namespace djv

