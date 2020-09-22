// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Style.h>

#include <djvCore/RapidJSONFunc.h>

namespace djv
{
    rapidjson::Value toJSON(const UI::Style::Palette&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const UI::Style::Metrics&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::Style::Palette&);
    void fromJSON(const rapidjson::Value&, UI::Style::Metrics&);

} // namespace djv
