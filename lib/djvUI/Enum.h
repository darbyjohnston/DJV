// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/UI.h>

namespace djv
{
    namespace UI
    {
        //! This enumeration provides orientations.
        enum class Orientation
        {
            Horizontal,
            Vertical,

            Count,
            First = Horizontal
        };

        //! This enumeration provides sides.
        enum class Side
        {
            Left,
            Top,
            Right,
            Bottom,

            Count,
            First = Left
        };

        //! This enumeration provides corners.
        enum class Corner
        {
            UpperLeft,
            UpperRight,
            LowerRight,
            LowerLeft,

            Count,
            First = UpperLeft
        };

        //! This enumeration provides layout expansion directions.
        enum class Expand
        {
            None,
            Horizontal,
            Vertical,
            Both,

            Count,
            First = None
        };

        //! This enumeration provides horizontal alignment.
        enum class HAlign
        {
            Center,
            Left,
            Right,
            Fill,
            
            Count,
            First = Center
        };

        //! This enumeration provides vertical alignment.
        enum class VAlign
        {
            Center,
            Top,
            Bottom,
            Fill,

            Count,
            First = Center
        };

        //! This enumeration provides horizontal text alignment.
        enum class TextHAlign
        {
            Center,
            Left,
            Right,
            
            Count,
            First = Center
        };

        //! This enumeration provides vertical text alignment.
        enum class TextVAlign
        {
            Center,
            Top,
            Bottom,
            Baseline,

            Count,
            First = Center
        };

        //! This enumeration provides selection types.
        enum class SelectionType
        {
            None,
            Single,
            Radio,
            Multiple,

            Count,
            First = Single
        };

        //! This enumeration provides sort orders.
        enum class SortOrder
        {
            Forward,
            Reverse,

            Count,
            First = Forward
        };

        //! This enumeration provides button types.
        enum class ButtonType
        {
            Push,
            Toggle,
            Radio,
            Exclusive,

            Count,
            First = Push
        };

        //! This enumeration provides item view types.
        enum class ViewType
        {
            Tiles,
            List,

            Count,
            First = Tiles
        };

        //! This enumeration provides callback options.
        enum class Callback
        {
            Trigger,
            Suppress
        };

        //! This enumeration provides the color roles.
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

        //! This enumeration provides the metrics roles.
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

        //! This enumeration provides information about text editing.
        enum class TextEditReason
        {
            Accepted,
            LostFocus
        };

        //! This enumeration provides the text focus navigation direction.
        enum class TextFocusDirection
        {
            Next,
            Prev
        };

        //! This enumeration provides image rotations.
        enum class ImageRotate
        {
            _0,
            _90,
            _180,
            _270,

            Count,
            First = _0
        };

        //! This enumeration provides image aspect ratios.
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

        //! This enumeration provides menu popup directions.
        enum class Popup
        {
            BelowRight,
            BelowLeft,
            AboveRight,
            AboveLeft,

            Count,
            First = BelowRight
        };

        //! This enum provides menu button styles.
        enum class MenuButtonStyle
        {
            Flat,
            Tool,
            ComboBox
        };

    } // namespace UI
} // namespace djv

