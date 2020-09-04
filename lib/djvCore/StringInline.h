// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <algorithm>

namespace djv
{
    namespace Core
    {
        namespace String
        {
            template<typename T>
            inline size_t intToString(T value, char * out, size_t maxLen)
            {
                const size_t max = maxLen ? maxLen - 1 : 0;
                const bool   negative = !(0 == value || value > 0);
                size_t count = 0;
                T tmp = value;
                do
                {
                    ++count;
                    tmp /= 10;
                } while (tmp && count < max);
                if (negative)
                {
                    ++count;
                    out[0] = '-';
                }
                if (count > 0)
                {
                    const int end = negative ? 1 : 0;
                    for (int i = static_cast<int>(count) - 1; i >= end; --i, value /= 10)
                    {
                        if (negative)
                        {
                            out[i] = '0' - (value % 10);
                        }
                        else
                        {
                            out[i] = '0' + (value % 10);
                        }
                    }
                }
                out[count] = 0;
                return count;
            }

        } // namespace String
    } // namespace Core
} // namespace djv
