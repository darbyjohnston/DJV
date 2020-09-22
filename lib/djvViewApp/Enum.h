// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

namespace djv
{
    namespace ViewApp
    {
        enum class ViewLock
        {
            None,
            Frame,
            Center,

            Count,
            First = None
        };

        enum class GridLabels
        {
            None,
            X_Y,
            A_Y,

            Count,
            First = None
        };

        //! This enumeration provides HUD background options.
        enum class HUDBackground
        {
            None,
            Overlay,

            Count,
            First = None
        };

        enum class ViewBackground
        {
            Solid,
            Checkers,

            Count,
            First = Solid
        };

        enum class ScrollWheelSpeed
        {
            Slow,
            Medium,
            Fast,

            Count,
            First = Slow
        };

        enum class Playback
        {
            Stop,
            Forward,
            Reverse,

            Count,
            First = Stop
        };

        enum class PlaybackSpeed
        {
            Default,
            Custom,

            Count,
            First = Default
        };

        enum class PlaybackMode
        {
            Once,
            Loop,
            PingPong,

            Count,
            First = Once
        };

        enum class CmdLineMode
        {
            DJV,
            Maya,

            Count,
            First = DJV
        };

    } // namespace ViewApp
} // namespace djv
