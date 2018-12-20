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

#include <djvCore/PicoJSON.h>

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
        DJV_TEXT("Horizontal"),
        DJV_TEXT("Vertical"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        Side,
        DJV_TEXT("Left"),
        DJV_TEXT("Top"),
        DJV_TEXT("Right"),
        DJV_TEXT("Bottom"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        Expand,
        DJV_TEXT("None"),
        DJV_TEXT("Horizontal"),
        DJV_TEXT("Vertical"),
        DJV_TEXT("Both"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        HAlign,
        DJV_TEXT("Center"),
        DJV_TEXT("Left"),
        DJV_TEXT("Right"),
        DJV_TEXT("Fill"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        VAlign,
        DJV_TEXT("Center"),
        DJV_TEXT("Top"),
        DJV_TEXT("Bottom"),
        DJV_TEXT("Fill"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        TextHAlign,
        DJV_TEXT("Center"),
        DJV_TEXT("Left"),
        DJV_TEXT("Right"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        TextVAlign,
        DJV_TEXT("Center"),
        DJV_TEXT("Top"),
        DJV_TEXT("Bottom"),
        DJV_TEXT("Baseline"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        SelectionType,
        DJV_TEXT("None"),
        DJV_TEXT("Single"),
        DJV_TEXT("Radio"),
        DJV_TEXT("Multiple"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        SortOrder,
        DJV_TEXT("Forward"),
        DJV_TEXT("Reverse"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ButtonType,
        DJV_TEXT("Push"),
        DJV_TEXT("Toggle"),
        DJV_TEXT("Radio"));

} // namespace djv

