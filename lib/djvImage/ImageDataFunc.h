// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Image.h>

#include <djvCore/RapidJSONFunc.h>

#include <memory>

namespace djv
{
    std::ostream& operator << (std::ostream&, const Image::Size&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Image::Size&);

    rapidjson::Value toJSON(const Image::Size&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Image::Size&);

} // namespace djv

