// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/Enum.h>

#include <djvCore/Speed.h>

#include <algorithm>

using namespace djv::Core;

namespace djv
{
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        ViewLock,
        DJV_TEXT("view_lock_none"),
        DJV_TEXT("view_lock_frame"),
        DJV_TEXT("view_lock_center"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        GridLabels,
        DJV_TEXT("view_grid_labels_none"),
        DJV_TEXT("view_grid_labels_x_y"),
        DJV_TEXT("view_grid_labels_a_y"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        HUDBackground,
        DJV_TEXT("hud_background_none"),
        DJV_TEXT("hud_background_overlay"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        ViewBackground,
        DJV_TEXT("view_background_solid"),
        DJV_TEXT("view_background_checkers"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        ScrollWheelSpeed,
        DJV_TEXT("settings_scroll_wheel_slow"),
        DJV_TEXT("settings_scroll_wheel_medium"),
        DJV_TEXT("settings_scroll_wheel_fast"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        Playback,
        DJV_TEXT("playback_stop"),
        DJV_TEXT("playback_forward"),
        DJV_TEXT("playback_reverse"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        PlaybackMode,
        DJV_TEXT("playback_once"),
        DJV_TEXT("playback_loop"),
        DJV_TEXT("playback_pingpong"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        CmdLineMode,
        DJV_TEXT("cmd_line_mode_djv"),
        DJV_TEXT("cmd_line_mode_maya"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        PlaybackSpeed,
        DJV_TEXT("playback_speed_default"),
        DJV_TEXT("playback_speed_custom"));

    rapidjson::Value toJSON(ViewApp::ViewLock value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(ViewApp::GridLabels value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(ViewApp::HUDBackground value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(ViewApp::ViewBackground value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(ViewApp::ScrollWheelSpeed value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(ViewApp::PlaybackSpeed value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(ViewApp::PlaybackMode value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::ViewLock& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::GridLabels& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::HUDBackground& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::ViewBackground& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::ScrollWheelSpeed& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::PlaybackSpeed& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::PlaybackMode& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

