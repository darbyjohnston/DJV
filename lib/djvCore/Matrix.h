// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <glm/mat4x4.hpp>

#include <sstream>

namespace djv
{
    std::ostream & operator << (std::ostream &, const glm::mat4x4 &);
    std::istream & operator >> (std::istream &, glm::mat4x4 &);

} // namespace djv

#include <djvCore/MatrixInline.h>
