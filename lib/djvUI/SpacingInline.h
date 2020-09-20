// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            inline MetricsRole Spacing::operator [] (Orientation value) const
            {
                return _value[static_cast<size_t>(value)];
            }

            inline MetricsRole& Spacing::operator [] (Orientation value)
            {
                return _value[static_cast<size_t>(value)];
            }

            inline bool Spacing::operator == (const Spacing& other) const
            {
                return
                    _value[static_cast<size_t>(Orientation::Horizontal)] == other._value[static_cast<size_t>(Orientation::Horizontal)] &&
                    _value[static_cast<size_t>(Orientation::Vertical)] == other._value[static_cast<size_t>(Orientation::Vertical)];
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

