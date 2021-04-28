// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <chrono>
#include <ctime>
#include <string>

namespace djv
{
    namespace Core
    {
        //! This namespace provides time functionality.
        namespace Time
        {
            //! This typedef provides the default time duration.
            typedef std::chrono::microseconds Duration;

            //! This typedef provides the default time point.
            typedef std::chrono::steady_clock::time_point TimePoint;
            
            //! \name Utility
            ///@{

            //! Get the local time.
            void localtime(const time_t*, tm*);

            //! Sleep the current thread for the given duration.
            void sleep(const Duration&);

            ///@}

            //! \name Conversion
            ///@{

            void secondsToTime(
                double,
                int&    hours,
                int&    minutes,
                double& seconds);

            std::string getLabel(double seconds);
            std::string getLabel(time_t);
            
            ///@}

        } // namespace Time
    } // namespace Core
} // namespace djv

#include <djvCore/TimeInline.h>
