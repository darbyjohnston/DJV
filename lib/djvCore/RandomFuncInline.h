// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace Random
        {
            template<typename T>
            inline const T& getRandom(const std::vector<T>& value)
            {
                return value[getRandom(0, static_cast<int>(value.size()) - 1)];
            }

        } // namespace Random
    } // namespace Core
} // namespace djv

