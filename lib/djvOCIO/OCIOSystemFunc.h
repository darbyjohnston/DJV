// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvOCIO/OCIOSystem.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSONFunc.h>

namespace djv
{
    namespace OCIO
    {
        DJV_ENUM_HELPERS(ConfigMode);

    } // namespace OCIO

    DJV_ENUM_SERIALIZE_HELPERS(OCIO::ConfigMode);

    rapidjson::Value toJSON(OCIO::ConfigMode, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const OCIO::Config&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, OCIO::ConfigMode&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, OCIO::Config&);

} // namespace djv
