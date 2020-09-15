// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Enum.h>

#include <djvCore/RapidJSONFunc.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace AV
    {
        DJV_ENUM_HELPERS(Side);
        DJV_ENUM_HELPERS(SwapInterval);
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

