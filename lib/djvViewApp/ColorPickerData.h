// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Type.h>

#include <djvCore/RapidJSON.h>

namespace djv
{
    namespace ViewApp
    {
        struct ColorPickerData
        {
            size_t      sampleSize           = 1;
            Image::Type lockType             = Image::Type::None;
            bool        applyColorOperations = true;
            bool        applyColorSpace      = true;

            bool operator == (const ColorPickerData&) const;
            bool operator != (const ColorPickerData&) const;
        };

    } // namespace ViewApp

    rapidjson::Value toJSON(const ViewApp::ColorPickerData&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::ColorPickerData&);
    
} // namespace djv

