// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/RapidJSON.h>

namespace djv
{
    namespace Core
    {
        namespace Math
        {
            class Rational;

        } // namespace Math
    } // namespace Core

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

            Count,
            First = Default
        };
        DJV_ENUM_HELPERS(PlaybackSpeed);

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

    rapidjson::Value toJSON(ViewApp::ViewLock, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(ViewApp::GridLabels, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(ViewApp::HUDBackground, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(ViewApp::ViewBackground, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(ViewApp::ScrollWheelSpeed, rapidjson::Document::AllocatorType&);
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
    void fromJSON(const rapidjson::Value&, ViewApp::ScrollWheelSpeed&);

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

