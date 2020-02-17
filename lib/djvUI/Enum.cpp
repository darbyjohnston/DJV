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

        float getImageRotate(ImageRotate value)
        {
            const float values[] =
            {
                0.F,
                90.F,
                180.F,
                270.F
            };
            return values[static_cast<size_t>(value)];
        }

        float getImageAspectRatio(ImageAspectRatio value)
        {
            const float values[] =
            {
                1.F,
                1.F,
                1.78F,
                1.85F,
                2.35F,
                2.39F
            };
            return values[static_cast<size_t>(value)];
        }

        float getPixelAspectRatio(ImageAspectRatio value, float pixelAspectRatio)
        {
            float out = 1.F;
            switch (value)
            {
            case UI::ImageAspectRatio::FromSource:
                out = pixelAspectRatio;
                break;
            default: break;
            }
            return out;
        }

        float getAspectRatioScale(ImageAspectRatio value, float aspectRatio)
        {
            float out = 1.F;
            switch (value)
            {
            case UI::ImageAspectRatio::_16_9:
            case UI::ImageAspectRatio::_1_85:
            case UI::ImageAspectRatio::_2_35:
            case UI::ImageAspectRatio::_2_39:
                out = aspectRatio / UI::getImageAspectRatio(value);
                break;
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

    picojson::value toJSON(UI::ImageRotate value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    picojson::value toJSON(UI::ImageAspectRatio value)
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
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const picojson::value& value, UI::ImageRotate& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const picojson::value& value, UI::ImageAspectRatio& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        Orientation,
        DJV_TEXT("ui_orientation_horizontal"),
        DJV_TEXT("ui_orientation_vertical"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        Side,
        DJV_TEXT("ui_side_left"),
        DJV_TEXT("ui_side_top"),
        DJV_TEXT("ui_side_right"),
        DJV_TEXT("ui_side_bottom"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        Expand,
        DJV_TEXT("ui_expand_none"),
        DJV_TEXT("ui_expand_horizontal"),
        DJV_TEXT("ui_expand_vertical"),
        DJV_TEXT("ui_expand_both"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        HAlign,
        DJV_TEXT("ui_h_align_center"),
        DJV_TEXT("ui_h_align_left"),
        DJV_TEXT("ui_h_align_right"),
        DJV_TEXT("ui_h_align_fill"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        VAlign,
        DJV_TEXT("ui_v_align_center"),
        DJV_TEXT("ui_v_align_top"),
        DJV_TEXT("ui_v_align_bottom"),
        DJV_TEXT("ui_v_align_fill"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        TextHAlign,
        DJV_TEXT("ui_text_v_align_center"),
        DJV_TEXT("ui_text_v_align_left"),
        DJV_TEXT("ui_text_v_align_right"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        TextVAlign,
        DJV_TEXT("ui_text_v_align_center"),
        DJV_TEXT("ui_text_v_align_top"),
        DJV_TEXT("ui_text_v_align_bottom"),
        DJV_TEXT("ui_text_v_align_baseline"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        SelectionType,
        DJV_TEXT("ui_selection_type_none"),
        DJV_TEXT("ui_selection_type_single"),
        DJV_TEXT("ui_selection_type_radio"),
        DJV_TEXT("ui_selection_type_multiple"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        SortOrder,
        DJV_TEXT("ui_sort_order_forward"),
        DJV_TEXT("ui_sort_order_reverse"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ButtonType,
        DJV_TEXT("ui_button_type_push"),
        DJV_TEXT("ui_button_type_toggle"),
        DJV_TEXT("ui_button_type_radio"),
        DJV_TEXT("ui_button_type_exclusive"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ViewType,
        DJV_TEXT("ui_view_type_tiles"),
        DJV_TEXT("ui_view_type_list"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ColorRole,
        DJV_TEXT("ui_color_role_none"),
        DJV_TEXT("ui_color_role_background"),
        DJV_TEXT("ui_color_role_background_header"),
        DJV_TEXT("ui_color_role_background_bellows"),
        DJV_TEXT("ui_color_role_background_tool_bar"),
        DJV_TEXT("ui_color_role_foreground"),
        DJV_TEXT("ui_color_role_foreground_dim"),
        DJV_TEXT("ui_color_role_border"),
        DJV_TEXT("ui_color_role_border_button"),
        DJV_TEXT("ui_color_role_trough"),
        DJV_TEXT("ui_color_role_button"),
        DJV_TEXT("ui_color_role_hovered"),
        DJV_TEXT("ui_color_role_pressed"),
        DJV_TEXT("ui_color_role_checked"),
        DJV_TEXT("ui_color_role_text_focus"),
        DJV_TEXT("ui_color_role_tooltip_background"),
        DJV_TEXT("ui_color_role_tooltip_foreground"),
        DJV_TEXT("ui_color_role_overlay"),
        DJV_TEXT("ui_color_role_overlay_light"),
        DJV_TEXT("ui_color_role_shadow"),
        DJV_TEXT("ui_color_role_handle"),
        DJV_TEXT("ui_color_role_cached"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        MetricsRole,
        DJV_TEXT("ui_metrics_role_none"),
        DJV_TEXT("ui_metrics_role_border"),
        DJV_TEXT("ui_metrics_role_margin"),
        DJV_TEXT("ui_metrics_role_margin_small"),
        DJV_TEXT("ui_metrics_role_margin_large"),
        DJV_TEXT("ui_metrics_role_margin_dialog"),
        DJV_TEXT("ui_metrics_role_spacing"),
        DJV_TEXT("ui_metrics_role_spacing_small"),
        DJV_TEXT("ui_metrics_role_spacing_large"),
        DJV_TEXT("ui_metrics_role_drag"),
        DJV_TEXT("ui_metrics_role_icon"),
        DJV_TEXT("ui_metrics_role_icon_small"),
        DJV_TEXT("ui_metrics_role_icon_mini"),
        DJV_TEXT("ui_metrics_role_font_small"),
        DJV_TEXT("ui_metrics_role_font_medium"),
        DJV_TEXT("ui_metrics_role_font_large"),
        DJV_TEXT("ui_metrics_role_font_header"),
        DJV_TEXT("ui_metrics_role_font_title"),
        DJV_TEXT("ui_metrics_role_swatch"),
        DJV_TEXT("ui_metrics_role_swatch_small"),
        DJV_TEXT("ui_metrics_role_slider"),
        DJV_TEXT("ui_metrics_role_scroll_area"),
        DJV_TEXT("ui_metrics_role_scroll_bar"),
        DJV_TEXT("ui_metrics_role_scroll_bar_small"),
        DJV_TEXT("ui_metrics_role_menu"),
        DJV_TEXT("ui_metrics_role_text_column"),
        DJV_TEXT("ui_metrics_role_text_column_large"),
        DJV_TEXT("ui_metrics_role_search_box"),
        DJV_TEXT("ui_metrics_role_dialog"),
        DJV_TEXT("ui_metrics_role_shadow"),
        DJV_TEXT("ui_metrics_role_shadow_small"),
        DJV_TEXT("ui_metrics_role_tooltip_offset"),
        DJV_TEXT("ui_metrics_role_handle"),
        DJV_TEXT("ui_metrics_role_move"),
        DJV_TEXT("ui_metrics_role_scrub"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ImageRotate,
        DJV_TEXT("ui_image_rotate_0"),
        DJV_TEXT("ui_image_rotate_90"),
        DJV_TEXT("ui_image_rotate_180"),
        DJV_TEXT("ui_image_rotate_270"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ImageAspectRatio,
        DJV_TEXT("ui_aspect_ratio_unscaled"),
        DJV_TEXT("ui_aspect_ratio_from_source"),
        DJV_TEXT("ui_aspect_ratio_16_9"),
        DJV_TEXT("ui_aspect_ratio_1_85"),
        DJV_TEXT("ui_aspect_ratio_2_35"),
        DJV_TEXT("ui_aspect_ratio_2_39"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        Popup,
        DJV_TEXT("popup_below_right"),
        DJV_TEXT("popup_below_left"),
        DJV_TEXT("popup_above_right"),
        DJV_TEXT("popup_above_left"));

} // namespace djv

