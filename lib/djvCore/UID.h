// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <stdint.h>

namespace djv
{
    namespace Core
    {
        typedef uint64_t UID;

        UID createUID();

    } // namespace Core
} // namespace djv
