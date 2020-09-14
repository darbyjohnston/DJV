// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/FrameNumber.h>
#include <djvCore/Rational.h>
#include <djvCore/RapidJSONFunc.h>
#include <djvCore/Time.h>

#include <ctime>
#include <string>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            //! \name Time Utilities
            ///@{

            void sleep(const Duration&);

            ///@}

            //! \name Time Conversion
            ///@{

            Math::Rational getTimebaseRational();

            int64_t scale(int64_t, const Math::Rational&, const Math::Rational&) noexcept;

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

            Frame::Number timecodeToFrame(uint32_t timecode, const Math::Rational&);

            uint32_t frameToTimecode(Frame::Number, const Math::Rational&);

            std::string timecodeToString(uint32_t);

            //! Throws:
            //! - std::exception
            void stringToTimecode(const std::string&, uint32_t&);

            ///@}

            //! \name Time Units
            ///@{

            std::string toString(Frame::Number, const Math::Rational&, Units);

            //! Throws:
            //! - std::exception
            Frame::Number fromString(const std::string&, const Math::Rational&, Units);

            ///@}

            DJV_ENUM_HELPERS(Units);
            
        } // namespace Time
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Time::Units);

    rapidjson::Value toJSON(Core::Time::Units, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Core::Time::Units&);

} // namespace djv

