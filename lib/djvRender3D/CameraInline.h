// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Render3D
    {

        inline const glm::mat4x4& ICamera::getV() const
        {
            return _v;
        }

        inline const glm::mat4x4& ICamera::getP() const
        {
            return _p;
        }

    } // namespace Render3D
} // namespace djv
