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
            //! \name Time Utilities
            ///@{

            void sleep(const Duration&);

            ///@}

            //! \name Time Conversion
            ///@{

            void secondsToTime(
                double,
                int&    hours,
                int&    minutes,
                double& seconds);

            std::string getLabel(double seconds);
            std::string getLabel(time_t);
            
            void localtime(const time_t*, tm*);
            
            ///@}
            
        } // namespace Time
    } // namespace Core
} // namespace djv

