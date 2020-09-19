// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Color.h>

#include <djvCore/RapidJSONFunc.h>

namespace djv
{
    std::ostream& operator << (std::ostream&, const Image::Color&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Image::Color&);

    rapidjson::Value toJSON(const Image::Color&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Image::Color&);

} // namespace djv
