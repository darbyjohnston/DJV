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
    //! Viewer application.
    namespace ViewApp
    {
        //! View lock modes.
        enum class ViewLock
        {
            None,
            Frame,
            Center,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(ViewLock);

        //! Grid labels.
        enum class GridLabels
        {
            None,
            X_Y,
            A_Y,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(GridLabels);

        //! HUD background options.
        enum class HUDBackground
        {
            None,
            Overlay,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(HUDBackground);

        //! View backgrounds.
        enum class ViewBackground
        {
            Solid,
            Checkers,

            Count,
            First = Solid
        };
        DJV_ENUM_HELPERS(ViewBackground);

        //! Playback modes.
        enum class Playback
        {
            Stop,
            Forward,
            Reverse,

            Count,
            First = Stop
        };
        DJV_ENUM_HELPERS(Playback);

        //! Playback speeds.
        enum class PlaybackSpeed
        {
            Default,
            Custom,

            Count,
            First = Default
        };
        DJV_ENUM_HELPERS(PlaybackSpeed);

        //! Playback modes.
        enum class PlaybackMode
        {
            Once,
            Loop,
            PingPong,

            Count,
            First = Once
        };
        DJV_ENUM_HELPERS(PlaybackMode);

        //! Command-line modes.
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
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::Playback);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::PlaybackSpeed);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::PlaybackMode);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::CmdLineMode);

    rapidjson::Value toJSON(ViewApp::ViewLock, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(ViewApp::GridLabels, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(ViewApp::HUDBackground, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(ViewApp::ViewBackground, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(ViewApp::PlaybackSpeed, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(ViewApp::PlaybackMode, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(ViewApp::CmdLineMode, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::ViewLock&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::GridLabels&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::HUDBackground&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::ViewBackground&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::PlaybackSpeed&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::PlaybackMode&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::CmdLineMode&);

} // namespace djv
