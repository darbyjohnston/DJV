// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/Render3DMaterialTest.h>

#include <djvAV/Render3DLight.h>
#include <djvAV/Render3DMaterial.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        Render3DMaterialTest::Render3DMaterialTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::Render3DMaterialTest", tempPath, context)
        {}
        
        void Render3DMaterialTest::run()
        {
            if (auto context = getContext().lock())
            {
                for (const auto& i : Render3D::getDefaultMaterialModeEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    _print("Default material mode: " + _getText(ss.str()));
                }
                
                {
                    auto material = Render3D::SolidColorMaterial::create(context);
                    
                    auto light = Render3D::PointLight::create();
                    Render3D::BindData bindData;
                    bindData.lights.push_back(light);
                    material->bind(bindData);
                    
                    Render3D::PrimitiveBindData primitiveBindData;
                    primitiveBindData.model = glm::mat4x4(1.F);
                    primitiveBindData.camera = glm::mat4x4(1.F);
                    primitiveBindData.color = Image::Color(1.F);
                    material->primitiveBind(primitiveBindData);
                    
                    DJV_ASSERT(material->getShader());
                }
                
                {
                    auto material = Render3D::DefaultMaterial::create(context);
                    
                    Render3D::DefaultMaterialMode mode(Render3D::DefaultMaterialMode::Unlit);
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
                    
                    auto hemisphereLight = Render3D::HemisphereLight::create();
                    auto directionalLight = Render3D::DirectionalLight::create();
                    auto pointLight = Render3D::PointLight::create();
                    auto spotLight = Render3D::SpotLight::create();
                    Render3D::BindData bindData;
                    bindData.lights.push_back(hemisphereLight);
                    bindData.lights.push_back(directionalLight);
                    bindData.lights.push_back(pointLight);
                    bindData.lights.push_back(spotLight);
                    material->bind(bindData);
                    
                    Render3D::PrimitiveBindData primitiveBindData;
                    primitiveBindData.model = glm::mat4x4(1.F);
                    primitiveBindData.camera = glm::mat4x4(1.F);
                    primitiveBindData.color = Image::Color(1.F);
                    material->primitiveBind(primitiveBindData);
                }
            }
        }

    } // namespace AVTest
} // namespace djv

