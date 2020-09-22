// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            inline MetricsRole Margin::operator [] (Side side) const
            {
                return _value[static_cast<size_t>(side)];
            }

            inline MetricsRole& Margin::operator [] (Side side)
            {
                return _value[static_cast<size_t>(side)];
            }

            inline bool Margin::operator == (const Margin& other) const
            {
                return
                    _value[static_cast<size_t>(Side::Left)] == other._value[static_cast<size_t>(Side::Left)] &&
                    _value[static_cast<size_t>(Side::Top)] == other._value[static_cast<size_t>(Side::Top)] &&
                    _value[static_cast<size_t>(Side::Right)] == other._value[static_cast<size_t>(Side::Right)] &&
                    _value[static_cast<size_t>(Side::Bottom)] == other._value[static_cast<size_t>(Side::Bottom)];
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
