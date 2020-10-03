// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

namespace djv
{
    namespace ViewApp
    {
        //! This enumeration provides view lock modes.
        enum class ViewLock
        {
            None,
            Frame,
            Center,

            Count,
            First = None
        };

        //! This enumeration provides grid labels.
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

        //! This enumeration provides view backgrounds.
        enum class ViewBackground
        {
            Solid,
            Checkers,

            Count,
            First = Solid
        };

        //! This enumeration provides playback modes.
        enum class Playback
        {
            Stop,
            Forward,
            Reverse,

            Count,
            First = Stop
        };

        //! This enumeration provides playback speeds.
        enum class PlaybackSpeed
        {
            Default,
            Custom,

            Count,
            First = Default
        };

        //! This enumeration provides playback modes.
        enum class PlaybackMode
        {
            Once,
            Loop,
            PingPong,

            Count,
            First = Once
        };

        //! This enumeration provides command-line modes.
        enum class CmdLineMode
        {
            DJV,
            Maya,

            Count,
            First = DJV
        };

    } // namespace ViewApp
} // namespace djv
