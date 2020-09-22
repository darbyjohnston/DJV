// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/ScrollWidgetFunc.h>

#include <array>
#include <list>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        DJV_ENUM_HELPERS_IMPLEMENTATION(ScrollType);

    } // namespace UI    

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ScrollType,
        DJV_TEXT("ui_scroll_type_both"),
        DJV_TEXT("ui_scroll_type_horizontal"),
        DJV_TEXT("ui_scroll_type_vertical"));

} // namespace djv
