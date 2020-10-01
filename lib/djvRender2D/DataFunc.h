// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvRender2D/Data.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSONFunc.h>

namespace djv
{
    namespace Render2D
    {
        //! \name Conversion
        ///@{

        GLenum toGL(ImageFilter);

        ///@}

        DJV_ENUM_HELPERS(ImageChannelDisplay);
        DJV_ENUM_HELPERS(ImageCache);
        DJV_ENUM_HELPERS(ImageFilter);

    } // namespace Render2D

    DJV_ENUM_SERIALIZE_HELPERS(Render2D::ImageChannelDisplay);
    DJV_ENUM_SERIALIZE_HELPERS(Render2D::ImageCache);
    DJV_ENUM_SERIALIZE_HELPERS(Render2D::ImageFilter);

    rapidjson::Value toJSON(Render2D::ImageChannelDisplay, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Render2D::ImageColor&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Render2D::ImageLevels&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Render2D::ImageExposure&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(Render2D::ImageFilter, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Render2D::ImageFilterOptions&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageChannelDisplay&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageColor&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageLevels&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageExposure&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageFilter&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageFilterOptions&);

} // namespace djv
