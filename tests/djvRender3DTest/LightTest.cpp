// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvRender3DTest/LightTest.h>

#include <djvRender3D/Light.h>

using namespace djv::Core;
using namespace djv::Render3D;

namespace djv
{
    namespace Render3DTest
    {
        LightTest::LightTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::Render3DTest::LightTest", tempPath, context)
        {}
        
        void LightTest::run()
        {
            {
                auto light = HemisphereLight::create();
                
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
                auto light = DirectionalLight::create();
                
                const glm::vec3 direction(1.F, 1.F, 1.F);
                light->setDirection(direction);
                DJV_ASSERT(direction == light->getDirection());
            }
            
            {
                auto light = PointLight::create();
                
                const glm::vec3 position(1.F, 1.F, 1.F);
                light->setPosition(position);
                DJV_ASSERT(position == light->getPosition());
            }
            
            {
                auto light = SpotLight::create();
                
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

    } // namespace Render3DTest
} // namespace djv

