// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

namespace djv
{
    namespace Core
    {
        //! This namespace provides functionality for the observer pattern.
        namespace Observer
        {
            //! This enumeration provides observer callback options.
            enum class CallbackAction
            {
                Trigger,
                Suppress
            };

        } // namespace Observer
    } // namespace Core
} // namespace djv
