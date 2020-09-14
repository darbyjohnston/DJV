// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AV.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSONFunc.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace AV
    {
        //! This enumeration provides sides.
        enum class Side
        {
            None,
            Left,
            Top,
            Right,
            Bottom,

            Count,
            First = Left
        };
        DJV_ENUM_HELPERS(Side);

        //! This enumeration provides GLFW swap intervals.
        enum class SwapInterval
        {
            Default,
            _0,
            _1,

            Count,
            First = Default
        };
        DJV_ENUM_HELPERS(SwapInterval);

        //! This enumeration provides how the alpha channel is used for blending.
        //!
        //! References:
        //! - https://microsoft.github.io/Win2D/html/PremultipliedAlpha.htm
        enum class AlphaBlend
        {
            None,
            Straight,
            Premultiplied,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(AlphaBlend);

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(AV::Side);
    DJV_ENUM_SERIALIZE_HELPERS(AV::SwapInterval);
    DJV_ENUM_SERIALIZE_HELPERS(AV::AlphaBlend);

    rapidjson::Value toJSON(AV::SwapInterval, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(AV::AlphaBlend, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::SwapInterval&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::AlphaBlend&);

} // namespace djv

