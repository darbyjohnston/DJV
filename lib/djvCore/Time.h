//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvCore/Speed.h>

#include <ctime>

namespace djv
{
    namespace Core
    {
        //! This namespace provides time functionality.
        namespace Time
        {
            class Speed;

            //! This typedef provides a timestamp.
            typedef int64_t Timestamp;

            //! This typedef provides a duration.
            typedef int64_t Duration;

            //! \name Time Conversion
            ///@{

            double timestampToSeconds(Timestamp);
            double durationToSeconds(Duration);
            Timestamp secondsToTimestamp(double);
            Duration secondsToDuration(double);

            inline void secondsToTime(
                double,
                int &    hours,
                int &    minutes,
                double & seconds);

            std::string getLabel(double seconds);
            std::string getLabel(time_t);
            
            void localtime(const time_t *, tm *);
            
            ///@}

            //! \name Keycode
            ///@{

            std::string keycodeToString(
                int id,
                int type,
                int prefix,
                int count,
                int offset);

            //! Throws:
            //! - std::exception
            void stringToKeycode(
                const std::string &,
                int & id,
                int & type,
                int & prefix,
                int & count,
                int & offset);

            ///@}

            //! \name Timecode
            ///@{

            inline void timecodeToTime(
                uint32_t,
                int & hour,
                int & minute,
                int & second,
                int & frame);
            inline uint32_t timeToTimecode(
                int hour,
                int minute,
                int second,
                int frame);

            inline int64_t timecodeToFrame(uint32_t timecode, const Speed &);
            inline uint32_t frameToTimecode(int64_t frame, const Speed &);

            std::string timecodeToString(uint32_t);
            void stringToTimecode(const std::string &, uint32_t &, bool* ok = nullptr);

            ///@}

        } // namespace Time
    } // namespace Core
} // namespace djv

#include <djvCore/TimeInline.h>
