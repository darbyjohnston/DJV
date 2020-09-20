// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/RowLayoutFunc.h>

#include <array>
#include <list>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            DJV_ENUM_HELPERS_IMPLEMENTATION(RowStretch);

        } // namespace Layout
    } // namespace UI    

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::Layout,
        RowStretch,
        DJV_TEXT("layout_row_stretch_none"),
        DJV_TEXT("layout_row_stretch_expand"));

} // namespace ddjv
