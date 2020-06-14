// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>

namespace djv
{
    namespace Scene
    {
        //! This enumeration provides the scene orientation.
        enum class SceneOrient
        {
            YUp,
            ZUp
        };

        //! This enumeration provides how colors are assigned.
        enum class ColorAssignment
        {
            Layer,
            Parent,
            Primitive,

            Count,
            First = Layer
        };

        //! This enumeration provides how materials are assigned.
        enum class MaterialAssignment
        {
            Layer,
            Parent,
            Primitive,

            Count,
            First = Layer
        };

    } // namespace Scene
} // namespace djv

