// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/RapidJSONFunc.h>

namespace djv
{
    namespace ViewApp
    {
        struct ImageData;
    
    } // namespace ViewApp

    rapidjson::Value toJSON(const ViewApp::ImageData&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::ImageData&);

} // namespace djv

