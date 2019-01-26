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

#include <djvUI/Enum.h>

#include <algorithm>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        Orientation getOpposite(Orientation value)
        {
            Orientation out = value;
            switch (value)
            {
            case Orientation::Horizontal: out = Orientation::Vertical;   break;
            case Orientation::Vertical:   out = Orientation::Horizontal; break;
            default: break;
            }
            return out;
        }

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        Orientation,
        DJV_TEXT("djv::UI", "Horizontal"),
        DJV_TEXT("djv::UI", "Vertical"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        Side,
        DJV_TEXT("djv::UI", "Left"),
        DJV_TEXT("djv::UI", "Top"),
        DJV_TEXT("djv::UI", "Right"),
        DJV_TEXT("djv::UI", "Bottom"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        Expand,
        DJV_TEXT("djv::UI", "None"),
        DJV_TEXT("djv::UI", "Horizontal"),
        DJV_TEXT("djv::UI", "Vertical"),
        DJV_TEXT("djv::UI", "Both"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        HAlign,
        DJV_TEXT("djv::UI", "Center"),
        DJV_TEXT("djv::UI", "Left"),
        DJV_TEXT("djv::UI", "Right"),
        DJV_TEXT("djv::UI", "Fill"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        VAlign,
        DJV_TEXT("djv::UI", "Center"),
        DJV_TEXT("djv::UI", "Top"),
        DJV_TEXT("djv::UI", "Bottom"),
        DJV_TEXT("djv::UI", "Fill"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        TextHAlign,
        DJV_TEXT("djv::UI", "Center"),
        DJV_TEXT("djv::UI", "Left"),
        DJV_TEXT("djv::UI", "Right"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        TextVAlign,
        DJV_TEXT("djv::UI", "Center"),
        DJV_TEXT("djv::UI", "Top"),
        DJV_TEXT("djv::UI", "Bottom"),
        DJV_TEXT("djv::UI", "Baseline"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        SelectionType,
        DJV_TEXT("djv::UI", "None"),
        DJV_TEXT("djv::UI", "Single"),
        DJV_TEXT("djv::UI", "Radio"),
        DJV_TEXT("djv::UI", "Multiple"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        SortOrder,
        DJV_TEXT("djv::UI", "Forward"),
        DJV_TEXT("djv::UI", "Reverse"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ButtonType,
        DJV_TEXT("djv::UI", "Push"),
        DJV_TEXT("djv::UI", "Toggle"),
        DJV_TEXT("djv::UI", "Radio"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ViewType,
        DJV_TEXT("djv::UI::FileBrowser", "ThumbnailsLarge"),
        DJV_TEXT("djv::UI::FileBrowser", "ThumbnailsSmall"),
        DJV_TEXT("djv::UI::FileBrowser", "ListView"));

} // namespace djv

