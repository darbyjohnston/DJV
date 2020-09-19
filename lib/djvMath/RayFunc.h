// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/Ray.h>

#include <sstream>

namespace djv
{
    template<typename T, glm::precision P = glm::defaultp>
    std::ostream& operator << (std::ostream&, const Math::tRay3<T, P>&);

    //! Throws:
    //! - std::exception
    template<typename T, glm::precision P = glm::defaultp>
    std::istream& operator >> (std::istream&, Math::tRay3<T, P>&);

} // namespace djv

#include <djvMath/RayFuncInline.h>

