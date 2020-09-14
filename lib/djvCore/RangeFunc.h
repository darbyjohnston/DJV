// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Range.h>

#include <sstream>

namespace djv
{
    template<typename T>
    std::ostream& operator << (std::ostream&, const Core::Math::Range<T>&);

    //! Throws:
    //! - std::exception
    template<typename T>
    std::istream& operator >> (std::istream&, Core::Math::Range<T>&);

} // namespace djv

#include <djvCore/RangeFuncInline.h>

