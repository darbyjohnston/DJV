// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/Info.h>

namespace djv
{
    namespace Image
    {
        Layout::Layout() noexcept
        {}

        Size::Size(uint16_t w, uint16_t h) noexcept :
            w(w),
            h(h)
        {}

        Info::Info()
        {}

        Info::Info(const Size & size, Type type, const Layout & layout) :
            size(size),
            type(type),
            layout(layout)
        {}

        Info::Info(uint16_t width, uint16_t height, Type type, const Layout & layout) :
            size(width, height),
            type(type),
            layout(layout)
        {}
        
    } // namespace Image
} // namespace djv

