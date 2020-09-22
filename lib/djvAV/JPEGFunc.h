// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/JPEG.h>

#include <djvCore/RapidJSONFunc.h>

namespace djv
{
    rapidjson::Value toJSON(const AV::IO::JPEG::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::IO::JPEG::Options&);

} // namespace djv
