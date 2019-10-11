//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
        DJV_TEXT("ParentChanged"),
        DJV_TEXT("ChildAdded"),
        DJV_TEXT("ChildRemoved"),
        DJV_TEXT("ChildOrder"),
        DJV_TEXT("TextUpdate"),
        DJV_TEXT("Update"),
        DJV_TEXT("Style"),
        DJV_TEXT("PreLayout"),
        DJV_TEXT("Layout"),
        DJV_TEXT("Clip"),
        DJV_TEXT("Paint"),
        DJV_TEXT("PaintOverlay"),
        DJV_TEXT("PointerEnter"),
        DJV_TEXT("PointerLeave"),
        DJV_TEXT("PointerMove"),
        DJV_TEXT("ButtonPress"),
        DJV_TEXT("ButtonRelease"),
        DJV_TEXT("Scroll"),
        DJV_TEXT("Drop"),
        DJV_TEXT("KeyPress"),
        DJV_TEXT("KeyRelease"),
        DJV_TEXT("TextFocus"),
        DJV_TEXT("TextFocusLost"),
        DJV_TEXT("TextInput"));

} // namespace djv
