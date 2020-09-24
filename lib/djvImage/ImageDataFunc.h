// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/RapidJSONFunc.h>

#include <memory>

namespace djv
{
    namespace Image
    {
        class Mirror;
        class Size;
    
    } // namespace Image

    std::ostream& operator << (std::ostream&, const Image::Size&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Image::Size&);

    rapidjson::Value toJSON(const Image::Size&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Image::Mirror&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Image::Size&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Image::Mirror&);

} // namespace djv

