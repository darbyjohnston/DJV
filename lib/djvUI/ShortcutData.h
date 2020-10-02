// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <map>
#include <string>

namespace djv
{
    namespace System
    {
        class TextSystem;

    } // namespace System

    namespace UI
    {
        //! This struct provides keyboard shortcut data.
        struct ShortcutData
        {
            explicit ShortcutData(int key = 0, int modifiers = 0);

            int key       = 0;
            int modifiers = 0;

            bool isValid() const;

            bool operator == (const ShortcutData&) const;
            bool operator < (const ShortcutData&) const;
        };

        //! This struct provides primary and secondary shortcut data.
        struct ShortcutDataPair
        {
            ShortcutDataPair();
            ShortcutDataPair(const ShortcutData& primary);
            ShortcutDataPair(const ShortcutData& primary, const ShortcutData& secondary);

            ShortcutData primary;
            ShortcutData secondary;

            bool operator == (const ShortcutDataPair&) const;
            bool operator < (const ShortcutDataPair&) const;
        };

        //! This typedef provides a map of shortcut data.
        typedef std::map<std::string, ShortcutDataPair> ShortcutDataMap;

    } // namespace UI
} // namespace djv

#include <djvUI/ShortcutDataInline.h>
