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
   
    picojson::value toJSON(UI::ViewType value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    void fromJSON(const picojson::value & value, UI::ViewType & out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

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
        DJV_TEXT("Radio"),
        DJV_TEXT("Exclusive"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ViewType,
        DJV_TEXT("Tiles"),
        DJV_TEXT("List"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ColorRole,
        DJV_TEXT("None"),
        DJV_TEXT("Background"),
        DJV_TEXT("BackgroundHeader"),
        DJV_TEXT("BackgroundBellows"),
        DJV_TEXT("BackgroundToolBar"),
        DJV_TEXT("Foreground"),
        DJV_TEXT("ForegroundDim"),
        DJV_TEXT("Border"),
        DJV_TEXT("BorderButton"),
        DJV_TEXT("Trough"),
        DJV_TEXT("Button"),
        DJV_TEXT("Hovered"),
        DJV_TEXT("Pressed"),
        DJV_TEXT("Checked"),
        DJV_TEXT("TextFocus"),
        DJV_TEXT("TooltipBackground"),
        DJV_TEXT("TooltipForeground"),
        DJV_TEXT("Overlay"),
        DJV_TEXT("OverlayLight"),
        DJV_TEXT("Shadow"),
        DJV_TEXT("Handle"),
        DJV_TEXT("Cached"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        MetricsRole,
        DJV_TEXT("None"),
        DJV_TEXT("Border"),
        DJV_TEXT("Margin"),
        DJV_TEXT("MarginSmall"),
        DJV_TEXT("MarginLarge"),
        DJV_TEXT("MarginDialog"),
        DJV_TEXT("Spacing"),
        DJV_TEXT("SpacingSmall"),
        DJV_TEXT("SpacingLarge"),
        DJV_TEXT("Drag"),
        DJV_TEXT("Icon"),
        DJV_TEXT("IconSmall"),
        DJV_TEXT("FontSmall"),
        DJV_TEXT("FontMedium"),
        DJV_TEXT("FontLarge"),
        DJV_TEXT("FontHeader"),
        DJV_TEXT("FontTitle"),
        DJV_TEXT("Swatch"),
        DJV_TEXT("Slider"),
        DJV_TEXT("ScrollArea"),
        DJV_TEXT("Menu"),
        DJV_TEXT("TextColumn"),
        DJV_TEXT("TextColumnLarge"),
        DJV_TEXT("SearchBox"),
        DJV_TEXT("Dialog"),
        DJV_TEXT("Shadow"),
        DJV_TEXT("ShadowSmall"),
        DJV_TEXT("TooltipOffset"),
        DJV_TEXT("Handle"),
        DJV_TEXT("Move"));

} // namespace djv

