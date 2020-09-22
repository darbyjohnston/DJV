// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/View.h>

#include <djvCore/RapidJSONFunc.h>

namespace djv
{
    rapidjson::Value toJSON(const ViewApp::GridOptions&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const ViewApp::HUDOptions&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const ViewApp::ViewBackgroundOptions&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::GridOptions&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::HUDOptions&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::ViewBackgroundOptions&);

} // namespace djv

