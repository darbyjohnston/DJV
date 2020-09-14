// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Ray.h>

#include <sstream>

namespace djv
{
    template<typename T, glm::precision P = glm::defaultp>
    std::ostream& operator << (std::ostream&, const Core::Ray::tRay3<T, P>&);

    //! Throws:
    //! - std::exception
    template<typename T, glm::precision P = glm::defaultp>
    std::istream& operator >> (std::istream&, Core::Ray::tRay3<T, P>&);

} // namespace djv

#include <djvCore/RayFuncInline.h>

