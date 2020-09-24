// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorPickerDataFunc.h>

#include <djvViewApp/ColorPickerData.h>

#include <djvImage/PixelFunc.h>

#include <djvMath/VectorFunc.h>

namespace djv
{
    rapidjson::Value toJSON(const ViewApp::ColorPickerData& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("SampleSize", toJSON(value.sampleSize, allocator), allocator);
            out.AddMember("LockType", toJSON(value.lockType, allocator), allocator);
            out.AddMember("ApplyColorOperations", toJSON(value.applyColorOperations, allocator), allocator);
            out.AddMember("ApplyColorSpace", toJSON(value.applyColorSpace, allocator), allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::ColorPickerData& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("SampleSize", i.name.GetString()))
                {
                    fromJSON(i.value, out.sampleSize);
                }
                else if (0 == strcmp("LockType", i.name.GetString()))
                {
                    fromJSON(i.value, out.lockType);
                }
                else if (0 == strcmp("ApplyColorOperations", i.name.GetString()))
                {
                    fromJSON(i.value, out.applyColorOperations);
                }
                else if (0 == strcmp("ApplyColorSpace", i.name.GetString()))
                {
                    fromJSON(i.value, out.applyColorSpace);
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

