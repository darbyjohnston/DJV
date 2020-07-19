// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

namespace djv
{
    namespace Core
    {
        //! This enumeration provides observer callback options.
        enum class ObserverCallbackAction
        {
            Trigger,
            Suppress
        };

    } // namespace Core
} // namespace djv
