// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSON.h>

#include <chrono>
#include <ctime>
#include <string>

namespace djv
{
    namespace AV
    {
        //! Time functionality.
        namespace Time
        {
            //! Time units.
            enum class Units
            {
                Timecode,
                Frames,

                Count,
                First = Timecode
            };
            DJV_ENUM_HELPERS(Units);

            //! \name Conversion
            ///@{

            int64_t scale(int64_t, const Math::IntRational&, const Math::IntRational&) noexcept;

            void secondsToTime(
                double,
                int&    hours,
                int&    minutes,
                double& seconds);

            std::string getLabel(double seconds);
            std::string getLabel(time_t);
            
            void localtime(const time_t*, tm*);
            
            ///@}

            //! \name Keycode
            ///@{

            //! Throws:
            //! - std::exception
            std::string keycodeToString(
                int id,
                int type,
                int prefix,
                int count,
                int offset);

            //! Throws:
            //! - std::exception
            void stringToKeycode(
                const std::string&,
                int& id,
                int& type,
                int& prefix,
                int& count,
                int& offset);

            ///@}

            //! \name Timecode
            ///@{

            void timecodeToTime(
                uint32_t,
                int& hour,
                int& minute,
                int& second,
                int& frame);

            uint32_t timeToTimecode(
                int hour,
                int minute,
                int second,
                int frame);

            Math::Frame::Number timecodeToFrame(uint32_t timecode, const Math::IntRational&);

            uint32_t frameToTimecode(Math::Frame::Number, const Math::IntRational&);

            std::string timecodeToString(uint32_t);

            //! Throws:
            //! - std::exception
            void stringToTimecode(const std::string&, uint32_t&);

            ///@}

            //! \name Units
            ///@{

            std::string toString(Math::Frame::Number, const Math::IntRational&, Units);

            //! Throws:
            //! - std::exception
            Math::Frame::Number fromString(const std::string&, const Math::IntRational&, Units);

            ///@}

        } // namespace Time
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Time::Units);

    rapidjson::Value toJSON(AV::Time::Units, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::Time::Units&);
    
} // namespace djv

#include <djvAV/TimeInline.h>
