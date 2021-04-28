// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/RapidJSON.h>

#include <sstream>
#include <vector>

namespace djv
{
    //! User interface.
    namespace UI
    {
        //! Orientations.
        enum class Orientation
        {
            Horizontal,
            Vertical,

            Count,
            First = Horizontal
        };
        DJV_ENUM_HELPERS(Orientation);
        
        //! Get the opposite orientation.
        Orientation getOpposite(Orientation);

        //! Sides.
        enum class Side
        {
            Left,
            Top,
            Right,
            Bottom,

            Count,
            First = Left
        };
        DJV_ENUM_HELPERS(Side);

        //! Corners.
        enum class Corner
        {
            UpperLeft,
            UpperRight,
            LowerRight,
            LowerLeft,

            Count,
            First = UpperLeft
        };
        DJV_ENUM_HELPERS(Corner);

        //! Layout expansion directions.
        enum class Expand
        {
            None,
            Horizontal,
            Vertical,
            Both,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(Expand);

        //! Horizontal alignment.
        enum class HAlign
        {
            Center,
            Left,
            Right,
            Fill,
            
            Count,
            First = Center
        };
        DJV_ENUM_HELPERS(HAlign);

        //! Vertical alignment.
        enum class VAlign
        {
            Center,
            Top,
            Bottom,
            Fill,

            Count,
            First = Center
        };
        DJV_ENUM_HELPERS(VAlign);

        //! Horizontal text alignment.
        enum class TextHAlign
        {
            Center,
            Left,
            Right,
            
            Count,
            First = Center
        };
        DJV_ENUM_HELPERS(TextHAlign);

        //! Vertical text alignment.
        enum class TextVAlign
        {
            Center,
            Top,
            Bottom,
            Baseline,

            Count,
            First = Center
        };
        DJV_ENUM_HELPERS(TextVAlign);

        //! Selection types.
        enum class SelectionType
        {
            None,
            Single,
            Radio,
            Multiple,

            Count,
            First = Single
        };
        DJV_ENUM_HELPERS(SelectionType);

        //! Sort orders.
        enum class SortOrder
        {
            Forward,
            Reverse,

            Count,
            First = Forward
        };
        DJV_ENUM_HELPERS(SortOrder);

        //! Button types.
        enum class ButtonType
        {
            Push,
            Toggle,
            Radio,
            Exclusive,

            Count,
            First = Push
        };
        DJV_ENUM_HELPERS(ButtonType);

        //! Item view types.
        enum class ViewType
        {
            Tiles,
            List,

            Count,
            First = Tiles
        };

        //! Callback options.
        enum class Callback
        {
            Trigger,
            Suppress
        };

        //! Color roles.
        enum class ColorRole
        {
            None,
            Background,
            BackgroundHeader,
            BackgroundBellows,
            BackgroundToolBar,
            Foreground,
            ForegroundDim,
            Border,
            Trough,
            Button,
            Hovered,
            Pressed,
            Checked,
            TextFocus,
            TooltipBackground,
            TooltipForeground,
            Overlay,
            OverlayLight,
            Shadow,
            Handle,
            Cached,
            Warning,
            Error,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(ColorRole);

        //! Metrics roles.
        enum class MetricsRole
        {
            None,
            Border,
            BorderTextFocus,
            Margin,
            MarginSmall,
            MarginLarge,
            MarginInside,
            MarginDialog,
            Spacing,
            SpacingSmall,
            SpacingLarge,
            Drag,
            Icon,
            IconSmall,
            IconMini,
            FontSmall,
            FontMedium,
            FontLarge,
            FontHeader,
            FontTitle,
            Swatch,
            SwatchSmall,
            Slider,
            ScrollArea,
            ScrollAreaSmall,
            ScrollBar,
            ScrollBarSmall,
            Menu,
            TextColumn,
            TextColumnLarge,
            SearchBox,
            Dialog,
            Shadow,
            ShadowSmall,
            TooltipOffset,
            Handle,
            Move,
            Scrub,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(MetricsRole);

        //! Information about text editing.
        enum class TextEditReason
        {
            Accepted,
            LostFocus
        };

        //! Text focus navigation direction.
        enum class TextFocusDirection
        {
            Next,
            Prev
        };

        //! Image rotations.
        enum class ImageRotate
        {
            _0,
            _90,
            _180,
            _270,

            Count,
            First = _0
        };
        DJV_ENUM_HELPERS(ImageRotate);

        //! Get the image rotation value.
        float getImageRotate(ImageRotate);

        //! Image aspect ratios.
        enum class ImageAspectRatio
        {
            Unscaled,
            FromSource,
            _16_9,
            _1_85,
            _2_35,
            _2_39,

            Count,
            First = Unscaled
        };
        DJV_ENUM_HELPERS(ImageAspectRatio);
        
        //! Get the image aspect ratio.
        float getImageAspectRatio(ImageAspectRatio);

        //! Get the pixel aspect ratio.
        float getPixelAspectRatio(ImageAspectRatio, float pixelAspectRatio);
        
        //! Get the image aspect ratio scale.
        float getAspectRatioScale(ImageAspectRatio, float aspectRatio);

        //! Menu popup directions.
        enum class Popup
        {
            BelowRight,
            BelowLeft,
            AboveRight,
            AboveLeft,

            Count,
            First = BelowRight
        };
        DJV_ENUM_HELPERS(Popup);

        //! Menu button styles.
        enum class MenuButtonStyle
        {
            Flat,
            Tool,
            ComboBox
        };

        //! Mouse scroll wheel speeds.
        enum class ScrollWheelSpeed
        {
            Slow,
            Medium,
            Fast,

            Count,
            First = Slow
        };
        DJV_ENUM_HELPERS(ScrollWheelSpeed);

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(UI::Orientation);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Side);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Corner);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Expand);
    DJV_ENUM_SERIALIZE_HELPERS(UI::HAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::VAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::TextHAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::TextVAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::SelectionType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::SortOrder);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ButtonType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ViewType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ColorRole);
    DJV_ENUM_SERIALIZE_HELPERS(UI::MetricsRole);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ImageRotate);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ImageAspectRatio);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Popup);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ScrollWheelSpeed);

    rapidjson::Value toJSON(UI::ViewType, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(UI::ImageRotate, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(UI::ImageAspectRatio, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(UI::ScrollWheelSpeed, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::ViewType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::ImageRotate&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::ImageAspectRatio&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::ScrollWheelSpeed&);

} // namespace djv

