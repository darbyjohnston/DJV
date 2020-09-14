// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/Rational.h>
#include <djvCore/RapidJSONFunc.h>
#include <djvCore/Speed.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            //! \name Default Speed
            ///@{

            FPS getDefaultSpeed();
            
            void setDefaultSpeed(FPS);

            ///@}

            //! \name Utilities
            ///@{

            Math::Rational fromSpeed(FPS);
            Math::Rational fromSpeed(float);
            
            bool toSpeed(const Math::Rational&, FPS&);

            ///@}

            DJV_ENUM_HELPERS(FPS);

        } // namespace Time
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Time::FPS);

    rapidjson::Value toJSON(Core::Time::FPS, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Core::Time::FPS&);

} // namespace djv
