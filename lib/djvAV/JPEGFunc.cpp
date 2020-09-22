// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAV/JPEGFunc.h>

using namespace djv::Core;

namespace djv
{
    rapidjson::Value toJSON(const AV::IO::JPEG::Options& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("Quality", rapidjson::Value(value.quality), allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, AV::IO::JPEG::Options& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Quality", i.name.GetString()) && i.value.IsInt())
                {
                    out.quality = i.value.GetInt();
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }
} // namespace djv

