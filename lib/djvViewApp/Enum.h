// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/PicoJSON.h>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            class Speed;

        } // namespace Time
    } // namespace Core

    namespace ViewApp
    {
        enum class ViewLock
        {
            None,
            Fill,
            Frame,
            Center,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(ViewLock);

        enum class GridLabels
        {
            None,
            X_Y,
            A_Y,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(GridLabels);

        //! This enumeration provides HUD background options.
        enum class HUDBackground
        {
            None,
            Overlay,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(HUDBackground);

        enum class ViewBackground
        {
            Solid,
            Checkers,

            Count,
            First = Solid
        };
        DJV_ENUM_HELPERS(ViewBackground);

        enum class ScrollWheelSpeed
        {
            Slow,
            Medium,
            Fast,

            Count,
            First = Slow
        };
        DJV_ENUM_HELPERS(ScrollWheelSpeed);

        enum class Playback
        {
            Stop,
            Forward,
            Reverse,

            Count,
            First = Stop
        };
        DJV_ENUM_HELPERS(Playback);

        enum class PlaybackSpeed
        {
            Default,
            Custom,
            _6,
            _8,
            _12,
            _16,
            _23_98,
            _24,
            _25,
            _29_97,
            _30,
            _48,
            _50,
            _59_94,
            _60,
            _120,
            _240,

            Count,
            First = Default
        };
        DJV_ENUM_HELPERS(PlaybackSpeed);
        Core::Time::Speed getPlaybackSpeed(PlaybackSpeed);

        enum class PlaybackMode
        {
            Once,
            Loop,
            PingPong,

            Count,
            First = Once
        };
        DJV_ENUM_HELPERS(PlaybackMode);

        enum class CmdLineMode
        {
            DJV,
            Maya,

            Count,
            First = DJV
        };
        DJV_ENUM_HELPERS(CmdLineMode);

    } // namespace ViewApp

    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::ViewLock);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::GridLabels);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::HUDBackground);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::ViewBackground);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::ScrollWheelSpeed);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::Playback);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::PlaybackSpeed);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::PlaybackMode);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::CmdLineMode);

    picojson::value toJSON(ViewApp::ViewLock);
    picojson::value toJSON(ViewApp::GridLabels);
    picojson::value toJSON(ViewApp::HUDBackground);
    picojson::value toJSON(ViewApp::ViewBackground);
    picojson::value toJSON(ViewApp::ScrollWheelSpeed);
    picojson::value toJSON(ViewApp::PlaybackSpeed);
    picojson::value toJSON(ViewApp::PlaybackMode);
    picojson::value toJSON(ViewApp::CmdLineMode);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::ViewLock&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::GridLabels&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::HUDBackground&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::ViewBackground&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::ScrollWheelSpeed&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::PlaybackSpeed&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::PlaybackMode&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::CmdLineMode&);

} // namespace djv

