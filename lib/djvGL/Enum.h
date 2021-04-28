// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvGL/GL.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSON.h>

#include <sstream>
#include <vector>

namespace djv
{
    //! OpenGL functionality.
    namespace GL
    {
        //! GLFW swap intervals.
        enum class SwapInterval
        {
            Default,
            _0,
            _1,

            Count,
            First = Default
        };
        DJV_ENUM_HELPERS(SwapInterval);

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(GL::SwapInterval);

    rapidjson::Value toJSON(GL::SwapInterval, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, GL::SwapInterval&);
    
} // namespace djv

