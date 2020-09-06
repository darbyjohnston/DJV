// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <rapidjson/document.h>

#include <string>

namespace djv
{
    namespace Core
    {
        namespace RapidJSON
        {
            void errorLineNumber(const char*, size_t size, size_t index, size_t& line, size_t& character);

        } // namespace RapidJSON
    } // namespace Core

    rapidjson::Value toJSON(bool, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(int, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(int64_t, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(float, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(double, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(size_t, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const std::string&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, bool&);
    void fromJSON(const rapidjson::Value&, int&);
    void fromJSON(const rapidjson::Value&, int64_t&);
    void fromJSON(const rapidjson::Value&, float&);
    void fromJSON(const rapidjson::Value&, double&);
    void fromJSON(const rapidjson::Value&, size_t&);
    void fromJSON(const rapidjson::Value&, std::string&);

} // namespace djv
