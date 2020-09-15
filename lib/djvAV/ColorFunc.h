// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Color.h>

#include <djvCore/RapidJSONFunc.h>

namespace djv
{
    std::ostream& operator << (std::ostream&, const AV::Image::Color&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, AV::Image::Color&);

    rapidjson::Value toJSON(const AV::Image::Color&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::Image::Color&);

} // namespace djv
