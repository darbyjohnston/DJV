// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvRender3DTest/MaterialTest.h>

#include <djvRender3D/Light.h>
#include <djvRender3D/Material.h>

using namespace djv::Core;
using namespace djv::Render3D;

namespace djv
{
    namespace Render3DTest
    {
        MaterialTest::MaterialTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::Render3DTest::MaterialTest", tempPath, context)
        {}
        
        void MaterialTest::run()
        {
            if (auto context = getContext().lock())
            {
                for (const auto& i : getDefaultMaterialModeEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    _print("Default material mode: " + _getText(ss.str()));
                }
                
                {
                    auto material = SolidColorMaterial::create(context);
                    
                    auto light = PointLight::create();
                    BindData bindData;
                    bindData.lights.push_back(light);
                    material->bind(bindData);
                    
                    PrimitiveBindData primitiveBindData;
                    primitiveBindData.model = glm::mat4x4(1.F);
                    primitiveBindData.camera = glm::mat4x4(1.F);
                    primitiveBindData.color = Image::Color(1.F);
                    material->primitiveBind(primitiveBindData);
                    
                    DJV_ASSERT(material->getShader());
                }
                
                {
                    auto material = DefaultMaterial::create(context);
                    
                    DefaultMaterialMode mode(DefaultMaterialMode::Unlit);
                    const Image::Color ambient(1.F);
                    const Image::Color diffuse(1.F);
                    const Image::Color emission(1.F);
                    const Image::Color specular(1.F);
                    const float shine = 1.F;
                    const float transparency = 1.F;
                    const float reflectivity = 1.F;
                    const bool disableLighting = true;
                    material->setMode(mode);
                    material->setAmbient(ambient);
                    material->setDiffuse(diffuse);
                    material->setEmission(emission);
                    material->setSpecular(specular);
                    material->setShine(shine);
                    material->setTransparency(transparency);
                    material->setReflectivity(reflectivity);
                    material->setDisableLighting(disableLighting);
                    
                    auto hemisphereLight = HemisphereLight::create();
                    auto directionalLight = DirectionalLight::create();
                    auto pointLight = PointLight::create();
                    auto spotLight = SpotLight::create();
                    BindData bindData;
                    bindData.lights.push_back(hemisphereLight);
                    bindData.lights.push_back(directionalLight);
                    bindData.lights.push_back(pointLight);
                    bindData.lights.push_back(spotLight);
                    material->bind(bindData);
                    
                    PrimitiveBindData primitiveBindData;
                    primitiveBindData.model = glm::mat4x4(1.F);
                    primitiveBindData.camera = glm::mat4x4(1.F);
                    primitiveBindData.color = Image::Color(1.F);
                    material->primitiveBind(primitiveBindData);
                }
            }
        }

    } // namespace Render3DTest
} // namespace djv

