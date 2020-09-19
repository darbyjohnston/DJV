// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace Memory
        {
            template <class T>
            inline void hashCombine(std::size_t& seed, const T& v)
            {
                std::hash<T> hasher;
                seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

        } // namespace Memory
    } // namespace Core
} // namespace djv
