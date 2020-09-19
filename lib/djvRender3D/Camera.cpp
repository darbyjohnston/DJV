// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvRender3D/Camera.h>

using namespace djv::Core;

namespace djv
{
    namespace Render3D
    {
        std::shared_ptr<DefaultCamera> DefaultCamera::create()
        {
            auto out = std::shared_ptr<DefaultCamera>(new DefaultCamera);
            return out;
        }

        void DefaultCamera::setV(const glm::mat4x4& value)
        {
            _v = value;
        }

        void DefaultCamera::setP(const glm::mat4x4& value)
        {
            _p = value;
        }

        } // namespace Render3D
} // namespace djv
