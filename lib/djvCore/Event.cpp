//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
    namespace Core
    {
        namespace Event
        {
        } // namespace Event
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Event,
        Type,
        DJV_TEXT("djv::Core::Event", "ParentChanged"),
        DJV_TEXT("djv::Core::Event", "Locale"),
        DJV_TEXT("djv::Core::Event", "Update"),
        DJV_TEXT("djv::Core::Event", "Style"),
        DJV_TEXT("djv::Core::Event", "PreLayout"),
        DJV_TEXT("djv::Core::Event", "Layout"),
        DJV_TEXT("djv::Core::Event", "Clip"),
        DJV_TEXT("djv::Core::Event", "Paint"),
        DJV_TEXT("djv::Core::Event", "PointerEnter"),
        DJV_TEXT("djv::Core::Event", "PointerLeave"),
        DJV_TEXT("djv::Core::Event", "PointerMove"),
        DJV_TEXT("djv::Core::Event", "ButtonPress"),
        DJV_TEXT("djv::Core::Event", "ButtonRelease"),
        DJV_TEXT("djv::Core::Event", "Scroll"),
        DJV_TEXT("djv::Core::Event", "Drop"),
        DJV_TEXT("djv::Core::Event", "KeyboardFocus"),
        DJV_TEXT("djv::Core::Event", "KeyboardFocusLost"),
        DJV_TEXT("djv::Core::Event", "Key"),
        DJV_TEXT("djv::Core::Event", "Text"));

} // namespace djv
