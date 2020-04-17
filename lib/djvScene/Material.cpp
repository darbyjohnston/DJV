// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene/Material.h>

#include <djvAV/Render3DMaterial.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        std::shared_ptr<DefaultMaterial> DefaultMaterial::create()
        {
            auto out = std::shared_ptr<DefaultMaterial>(new DefaultMaterial);
            return out;
        }

        std::shared_ptr<AV::Render3D::IMaterial> DefaultMaterial::createMaterial(const std::shared_ptr<Core::Context>& context)
        {
            auto out = AV::Render3D::DefaultMaterial::create(context);
            out->setAmbient(_ambient);
            out->setDiffuse(_diffuse);
            out->setEmission(_emission);
            out->setSpecular(_specular);
            out->setShine(_shine);
            out->setTransparency(_transparency);
            out->setReflectivity(_reflectivity);
            out->setDisableLighting(_disableLighting);
            return out;
        }

    } // namespace Scene
} // namespace djv

