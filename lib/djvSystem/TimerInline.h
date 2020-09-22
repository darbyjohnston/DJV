// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace System
    {
        inline bool Timer::isRepeating() const
        {
            return _repeating;
        }

        inline void Timer::setRepeating(bool value)
        {
            _repeating = value;
        }

        inline bool Timer::isActive() const
        {
            return _active;
        }

        inline void Timer::stop()
        {
            _active = false;
        }

    } // namespace System
} // namespace djv

