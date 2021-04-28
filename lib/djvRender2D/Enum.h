// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/RapidJSON.h>

#include <sstream>
#include <vector>

namespace djv
{
    //! Two-dimensional rendering.
    namespace Render2D
    {
        //! Sides.
        enum class Side
        {
            None,
            Left,
            Top,
            Right,
            Bottom,

            Count,
            First = Left
        };
        DJV_ENUM_HELPERS(Side);
        
        //! Alpha channel blending.
        //!
        //! References:
        //! - https://microsoft.github.io/Win2D/html/PremultipliedAlpha.htm
        enum class AlphaBlend
        {
            None,
            Straight,
            Premultiplied,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(AlphaBlend);

    } // namespace Render2D

    DJV_ENUM_SERIALIZE_HELPERS(Render2D::Side);
    DJV_ENUM_SERIALIZE_HELPERS(Render2D::AlphaBlend);

    rapidjson::Value toJSON(Render2D::AlphaBlend, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::AlphaBlend&);
    
} // namespace djv

