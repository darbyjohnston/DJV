// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvGL/Enum.h>

#include <djvCore/RapidJSONFunc.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace GL
    {
        DJV_ENUM_HELPERS(SwapInterval);

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(GL::SwapInterval);

    rapidjson::Value toJSON(GL::SwapInterval, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, GL::SwapInterval&);

} // namespace djv

