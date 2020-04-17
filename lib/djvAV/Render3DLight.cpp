// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Render3DLight.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            std::shared_ptr<HemisphereLight> HemisphereLight::create()
            {
                auto out = std::shared_ptr<HemisphereLight>(new HemisphereLight);
                return out;
            }

            std::shared_ptr<DirectionalLight> DirectionalLight::create()
            {
                auto out = std::shared_ptr<DirectionalLight>(new DirectionalLight);
                return out;
            }

            std::shared_ptr<PointLight> PointLight::create()
            {
                auto out = std::shared_ptr<PointLight>(new PointLight);
                return out;
            }

            std::shared_ptr<SpotLight> SpotLight::create()
            {
                auto out = std::shared_ptr<SpotLight>(new SpotLight);
                return out;
            }

        } // namespace Render3D
    } // namespace AV
} // namespace djv
