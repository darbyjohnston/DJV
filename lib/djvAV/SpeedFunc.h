// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Speed.h>

#include <djvMath/Rational.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSONFunc.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace AV
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

    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::FPS);

    rapidjson::Value toJSON(AV::FPS, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::FPS&);

} // namespace djv
