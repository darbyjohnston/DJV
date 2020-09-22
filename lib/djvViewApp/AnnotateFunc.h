// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Annotate.h>

#include <djvCore/Enum.h>

namespace djv
{
    namespace ViewApp
    {
        float getAnnotateLineSize(AnnotateLineSize);

        DJV_ENUM_HELPERS(AnnotateTool);
        DJV_ENUM_HELPERS(AnnotateLineSize);

    } // namespace ViewApp

    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::AnnotateTool);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::AnnotateLineSize);

    rapidjson::Value toJSON(ViewApp::AnnotateTool, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(ViewApp::AnnotateLineSize, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::AnnotateTool&);
    void fromJSON(const rapidjson::Value&, ViewApp::AnnotateLineSize&);

} // namespace djv

