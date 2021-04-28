// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

namespace djv
{
    //! Three-dimensional scenes.
    namespace Scene3D
    {
        //! Scene orientation.
        enum class SceneOrient
        {
            YUp,
            ZUp
        };

        //! Color assignment.
        enum class ColorAssignment
        {
            Layer,
            Parent,
            Primitive,

            Count,
            First = Layer
        };

        //! Material assignment.
        enum class MaterialAssignment
        {
            Layer,
            Parent,
            Primitive,

            Count,
            First = Layer
        };

    } // namespace Scene3D
} // namespace djv

