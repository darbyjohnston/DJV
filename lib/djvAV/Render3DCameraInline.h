// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            inline ICamera::ICamera()
            {}

            inline ICamera::~ICamera()
            {}

            inline const glm::mat4x4& ICamera::getV() const
            {
                return _v;
            }

            inline const glm::mat4x4& ICamera::getP() const
            {
                return _p;
            }

            inline DefaultCamera::DefaultCamera()
            {}

        } // namespace Render3D
    } // namespace AV
} // namespace djv
