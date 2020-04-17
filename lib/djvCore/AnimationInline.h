// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace Animation
        {
            inline Animation::Animation()
            {}

            inline Type Animation::getType() const
            {
                return _type;
            }

            inline bool Animation::isRepeating() const
            {
                return _repeating;
            }

            inline bool Animation::isActive() const
            {
                return _active;
            }

        } // namespace Animation
    } // namespace Core
} // namespace djv

