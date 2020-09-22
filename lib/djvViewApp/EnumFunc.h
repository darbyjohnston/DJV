// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSONFunc.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace ViewApp
    {
        DJV_ENUM_HELPERS(ViewLock);
        DJV_ENUM_HELPERS(GridLabels);
        DJV_ENUM_HELPERS(HUDBackground);
        DJV_ENUM_HELPERS(ViewBackground);
        DJV_ENUM_HELPERS(ScrollWheelSpeed);
        DJV_ENUM_HELPERS(Playback);
        DJV_ENUM_HELPERS(PlaybackSpeed);
        DJV_ENUM_HELPERS(PlaybackMode);
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

