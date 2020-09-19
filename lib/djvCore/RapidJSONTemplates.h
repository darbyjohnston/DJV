// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/RapidJSONFunc.h>

#include <map>
#include <vector>

namespace djv
{
    template<typename T>
    rapidjson::Value toJSON(const std::vector<T>&, rapidjson::Document::AllocatorType&);
    template<typename T>
    rapidjson::Value toJSON(const std::map<std::string, T>&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    template<typename T>
    void fromJSON(const rapidjson::Value&, std::vector<T>&);
    template<typename T>
    void fromJSON(const rapidjson::Value&, std::map<std::string, T>&);

} // namespace djv

#include <djvCore/RapidJSONTemplatesInline.h>
