// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/ShortcutData.h>

namespace djv
{
    namespace UI
    {
        ShortcutData::ShortcutData(int key, int modifiers) :
            key(key),
            modifiers(modifiers)
        {}

        ShortcutDataPair::ShortcutDataPair()
        {}

        ShortcutDataPair::ShortcutDataPair(const ShortcutData& primary) :
            primary(primary)
        {}

        ShortcutDataPair::ShortcutDataPair(const ShortcutData& primary, const ShortcutData& secondary) :
            primary(primary),
            secondary(secondary)
        {}

    } // namespace UI
} // namespace djv
