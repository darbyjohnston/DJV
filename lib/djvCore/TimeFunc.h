// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Time.h>

#include <ctime>
#include <string>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
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

#include <djvCore/TimeFuncInline.h>
