// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvRender3D/Light.h>

using namespace djv::Core;

namespace djv
{
    namespace Render3D
    {
        ILight::ILight()
        {}

        ILight::~ILight()
        {}

        HemisphereLight::HemisphereLight()
        {}

        std::shared_ptr<HemisphereLight> HemisphereLight::create()
        {
            auto out = std::shared_ptr<HemisphereLight>(new HemisphereLight);
            return out;
        }

        DirectionalLight::DirectionalLight()
        {}

        std::shared_ptr<DirectionalLight> DirectionalLight::create()
        {
            auto out = std::shared_ptr<DirectionalLight>(new DirectionalLight);
            return out;
        }

        PointLight::PointLight()
        {}

        std::shared_ptr<PointLight> PointLight::create()
        {
            auto out = std::shared_ptr<PointLight>(new PointLight);
            return out;
        }

        SpotLight::SpotLight()
        {}

        std::shared_ptr<SpotLight> SpotLight::create()
        {
            auto out = std::shared_ptr<SpotLight>(new SpotLight);
            return out;
        }

        } // namespace Render3D
} // namespace djv
