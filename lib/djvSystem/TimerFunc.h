// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/Timer.h>

#include <djvCore/Enum.h>
#include <djvCore/Time.h>

namespace djv
{
    namespace System
    {
        //! \name Information
        ///@{

        size_t getTimerValue(TimerValue);
        
        Core::Time::Duration getTimerDuration(TimerValue);

        ///@}

        DJV_ENUM_HELPERS(TimerValue);

    } // namespace System
    
    DJV_ENUM_SERIALIZE_HELPERS(System::TimerValue);
    
} // namespace djv

