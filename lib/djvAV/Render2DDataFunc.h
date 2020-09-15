// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Render2DData.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSONFunc.h>

namespace djv
{
    namespace AV
    {
        namespace Render2D
        {
            GLenum toGL(ImageFilter);

            DJV_ENUM_HELPERS(ImageChannelDisplay);
            DJV_ENUM_HELPERS(ImageCache);
            DJV_ENUM_HELPERS(ImageFilter);

        } // namespace Render2D
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Render2D::ImageChannelDisplay);
    DJV_ENUM_SERIALIZE_HELPERS(AV::Render2D::ImageCache);
    DJV_ENUM_SERIALIZE_HELPERS(AV::Render2D::ImageFilter);

    rapidjson::Value toJSON(AV::Render2D::ImageFilter, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const AV::Render2D::ImageFilterOptions&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::Render2D::ImageFilter&);
    void fromJSON(const rapidjson::Value&, AV::Render2D::ImageFilterOptions&);

} // namespace djv
