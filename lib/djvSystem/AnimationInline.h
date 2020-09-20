// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace System
    {
        namespace Animation
        {
            inline Type Animation::getType() const
            {
                return _type;
            }

            inline bool Animation::isRepeating() const
            {
                return _repeating;
            }

            inline void Animation::setRepeating(bool value)
            {
                _repeating = value;
            }

            inline bool Animation::isActive() const
            {
                return _active;
            }

        } // namespace Animation
    } // namespace System
} // namespace djv

