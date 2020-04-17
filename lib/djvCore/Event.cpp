// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Event.h>

#include <algorithm>

namespace djv
{
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Event,
        Type,
        DJV_TEXT("event_parent_changed"),
        DJV_TEXT("event_child_added"),
        DJV_TEXT("event_child_removed"),
        DJV_TEXT("event_child_order"),
        DJV_TEXT("event_init"),
        DJV_TEXT("event_update"),
        DJV_TEXT("event_init_layout"),
        DJV_TEXT("event_pre_layout"),
        DJV_TEXT("event_layout"),
        DJV_TEXT("event_clip"),
        DJV_TEXT("event_paint"),
        DJV_TEXT("event_paint_overlay"),
        DJV_TEXT("event_pointer_enter"),
        DJV_TEXT("event_pointer_leave"),
        DJV_TEXT("event_pointer_move"),
        DJV_TEXT("event_button_press"),
        DJV_TEXT("event_button_release"),
        DJV_TEXT("event_scroll"),
        DJV_TEXT("event_drop"),
        DJV_TEXT("event_key_press"),
        DJV_TEXT("event_key_release"),
        DJV_TEXT("event_text_focus"),
        DJV_TEXT("event_text_focus_lost"),
        DJV_TEXT("event_text_input"));

} // namespace djv
