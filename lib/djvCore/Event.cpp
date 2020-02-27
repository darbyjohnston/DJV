//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
