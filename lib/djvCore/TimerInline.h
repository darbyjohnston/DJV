// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            inline Timer::Timer()
            {}

            inline bool Timer::isRepeating() const
            {
                return _repeating;
            }

            inline bool Timer::isActive() const
            {
                return _active;
            }

        } // namespace Time
    } // namespace Core
} // namespace djv

