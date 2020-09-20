// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ShortcutData.h>

#include <djvCore/RapidJSONFunc.h>

#include <map>
#include <memory>
#include <vector>

namespace djv
{
    rapidjson::Value toJSON(const UI::ShortcutData&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const UI::ShortcutDataPair&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::ShortcutData&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::ShortcutDataPair&);

} // namespace djv
