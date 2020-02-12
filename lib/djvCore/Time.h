//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvCore/Rational.h>

#include <chrono>
#include <ctime>

namespace djv
{
    namespace Core
    {
        //! This namespace provides time functionality.
        namespace Time
        {
            //! This typedef provides the time units.
            typedef std::chrono::microseconds Unit;

            //! \name Time Conversion
            ///@{

            // This is the same timebase as FFmpeg (libavutil.h/avutil.h).
            const int64_t timebase = 1000000;

            Math::Rational getTimebaseRational();

            int64_t scale(int64_t, const Math::Rational&, const Math::Rational&);

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

            void timecodeToTime(
                uint32_t,
                int & hour,
                int & minute,
                int & second,
                int & frame);
            uint32_t timeToTimecode(
                int hour,
                int minute,
                int second,
                int frame);

            int64_t timecodeToFrame(uint32_t timecode, const Math::Rational&);
            uint32_t frameToTimecode(int64_t frame, const Math::Rational&);

            std::string timecodeToString(uint32_t);

            //! Throws:
            //! - std::exception
            void stringToTimecode(const std::string &, uint32_t &);

            ///@}

        } // namespace Time
    } // namespace Core
} // namespace djv

#include <djvCore/TimeInline.h>
