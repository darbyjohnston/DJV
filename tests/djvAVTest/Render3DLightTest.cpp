// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/Render3DLightTest.h>

#include <djvAV/Render3DLight.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        Render3DLightTest::Render3DLightTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::Render3DLightTest", tempPath, context)
        {}
        
        void Render3DLightTest::run()
        {
            {
                auto light = Render3D::HemisphereLight::create();
                
                const float intensity = .5F;
                light->setIntensity(intensity);
                DJV_ASSERT(intensity == light->getIntensity());
                
                const glm::vec3 up(0.F, 0.F, 1.F);
                light->setUp(up);
                DJV_ASSERT(up == light->getUp());
                
                const Image::Color color(1.F);
                light->setTopColor(color);
                light->setBottomColor(color);
                DJV_ASSERT(color == light->getTopColor());
                DJV_ASSERT(color == light->getBottomColor());
            }
            
            {
                auto light = Render3D::DirectionalLight::create();
                
                const glm::vec3 direction(1.F, 1.F, 1.F);
                light->setDirection(direction);
                DJV_ASSERT(direction == light->getDirection());
            }
            
            {
                auto light = Render3D::PointLight::create();
                
                const glm::vec3 position(1.F, 1.F, 1.F);
                light->setPosition(position);
                DJV_ASSERT(position == light->getPosition());
            }
            
            {
                auto light = Render3D::SpotLight::create();
                
                const float coneAngle = 45.F;
                const glm::vec3 direction(1.F, 1.F, 1.F);
                const glm::vec3 position(-1.F, -1.F, -1.F);
                light->setConeAngle(coneAngle);
                light->setDirection(direction);
                light->setPosition(position);
                DJV_ASSERT(coneAngle == light->getConeAngle());
                DJV_ASSERT(direction == light->getDirection());
                DJV_ASSERT(position == light->getPosition());
            }
        }

    } // namespace AVTest
} // namespace djv

