// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvRender2D/Enum.h>

#include <djvCore/RapidJSONFunc.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace Render2D
    {
        DJV_ENUM_HELPERS(Side);
        DJV_ENUM_HELPERS(AlphaBlend);

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(Render2D::Side);
    DJV_ENUM_SERIALIZE_HELPERS(Render2D::AlphaBlend);

    rapidjson::Value toJSON(Render2D::AlphaBlend, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::AlphaBlend&);

} // namespace djv

