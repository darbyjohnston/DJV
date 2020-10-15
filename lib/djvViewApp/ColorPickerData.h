// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Type.h>

namespace djv
{
    namespace ViewApp
    {
        struct ColorPickerData
        {
            size_t      sampleSize           = 1;
            Image::Type lockType             = Image::Type::None;
            bool        applyColorOperations = true;
            bool        applyColorSpace      = true;

            bool operator == (const ColorPickerData&) const;
            bool operator != (const ColorPickerData&) const;
        };

    } // namespace ViewApp
} // namespace djv

