// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            inline int Speed::getScale() const
            {
                return _scale;
            }

            inline int Speed::getDuration() const
            {
                return _duration;
            }

            inline bool Speed::isValid() const
            {
                return _scale != 0 && _duration != 0;
            }

        } // namespace Time
    } // namespace Core
} // namespace djv
