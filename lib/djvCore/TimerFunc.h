// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Timer.h>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            size_t getValue(TimerValue);
            
            Duration getTime(TimerValue);

            DJV_ENUM_HELPERS(TimerValue);

        } // namespace Time
    } // namespace Core
    
    DJV_ENUM_SERIALIZE_HELPERS(Core::Time::TimerValue);
    
} // namespace djv

