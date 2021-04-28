// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/Rational.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSON.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace AV
    {
        //! Common speeds.
        enum class FPS
        {
            _1,
            _3,
            _6,
            _12,
            _15,
            _16,
            _18,
            _23_976,
            _24,
            _25,
            _29_97,
            _30,
            _50,
            _59_94,
            _60,
            _120,
            _240,

            Count,
            First = _1
        };
        DJV_ENUM_HELPERS(FPS);
        
        //! \name Default Speed
        ///@{

        FPS getDefaultSpeed();
        
        void setDefaultSpeed(FPS);

        ///@}

        //! \name Utility
        ///@{

        Math::IntRational fromSpeed(FPS);
        Math::IntRational fromSpeed(float);
        
        bool toSpeed(const Math::IntRational&, FPS&);

        ///@}

    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::FPS);

    rapidjson::Value toJSON(AV::FPS, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::FPS&);

} // namespace djv
