// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/Range.h>

#include <sstream>

namespace djv
{
    template<typename T>
    std::ostream& operator << (std::ostream&, const Math::Range<T>&);

    //! Throws:
    //! - std::exception
    template<typename T>
    std::istream& operator >> (std::istream&, Math::Range<T>&);

} // namespace djv

#include <djvMath/RangeFuncInline.h>

