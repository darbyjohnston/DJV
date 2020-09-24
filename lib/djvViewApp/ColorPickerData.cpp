// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorPickerData.h>

namespace djv
{
    namespace ViewApp
    {
        bool ColorPickerData::operator == (const ColorPickerData& other) const
        {
            return sampleSize == other.sampleSize &&
                lockType == other.lockType &&
                applyColorOperations == other.applyColorOperations &&
                applyColorSpace == other.applyColorSpace;
        }

        bool ColorPickerData::operator != (const ColorPickerData& other) const
        {
            return !(*this == other);
        }

    } // namespace ViewApp
} // namespace djv

