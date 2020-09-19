// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvRender3DTest/RenderTest.h>

#include <djvRender3D/Render.h>
#include <djvRender3D/Camera.h>
#include <djvRender3D/Light.h>
#include <djvRender3D/Material.h>

#include <djvGL/OffscreenBuffer.h>

#include <djvGeom/PointList.h>
#include <djvGeom/TriangleMesh.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

using namespace djv::Core;
using namespace djv::Render3D;

namespace djv
{
    namespace Render3DTest
    {
        RenderTest::RenderTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::Render3DTest::RenderTest", tempPath, context)
        {}
        
        void RenderTest::run()
        {
            _enum();
            _system();
        }
        
        void RenderTest::_enum()
        {
            for (const auto& i : getDepthBufferModeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Depth buffer mode: " + _getText(ss.str()));
            }
        }
        
        void RenderTest::_system()
        {
            if (auto context = getContext().lock())
            {
                const Image::Size size(1280, 720);
                auto offscreenBuffer = GL::OffscreenBuffer::create(
                    size,
                    Image::Type::RGBA_U8,
                    context->getSystemT<System::TextSystem>());
                offscreenBuffer->bind();
                auto render = context->getSystemT<Render>();
                
                auto camera = DefaultCamera::create();
                
                RenderOptions options;
                options.camera = camera;
                options.size.w = 1280;
                options.size.h = 720;
                options.clip = Math::FloatRange(.1F, 1000.F);                
                render->beginFrame(options);
                
                const glm::mat4x4 m(1.F);
                render->pushTransform(m);
                DJV_ASSERT(render->getCurrentTransform() == m);
                DJV_ASSERT(render->getCurrentInverseTransform() == m);
                render->pushTransform(m);
                
                render->setColor(Image::Color(1.F, 1.F, 1.F));
                auto material = SolidColorMaterial::create(context);
                render->setMaterial(material);
                
                auto hemisphereLight = HemisphereLight::create();
                render->addLight(hemisphereLight);
                auto directionalLight = DirectionalLight::create();
                render->addLight(directionalLight);
                auto pointLight = PointLight::create();
                render->addLight(pointLight);
                auto spotLight = SpotLight::create();
                render->addLight(spotLight);
                
                auto pointList = std::shared_ptr<Geom::PointList>(new Geom::PointList);
                pointList->v.push_back(glm::vec3(0.F, 100.F, 200.F));
                pointList->v.push_back(glm::vec3(300.F, 400.F, 500.F));
                pointList->v.push_back(glm::vec3(600.F, 700.F, 800.F));
                pointList->c.push_back(glm::vec3(1.F, 1.F, 1.F));
                pointList->c.push_back(glm::vec3(1.F, 1.F, 1.F));
                pointList->c.push_back(glm::vec3(1.F, 1.F, 1.F));
                render->drawPoints({ pointList, pointList });
                
                render->drawPolyLine(pointList);
                render->drawPolyLines({ pointList, pointList });
                
                auto mesh = std::shared_ptr<Geom::TriangleMesh>(new Geom::TriangleMesh);
                Geom::TriangleMesh::triangulateBBox(
                    Math::BBox3f(-100.F, -100.F, -100.F, 100.F, 100.F, 100.F),
                    *mesh);
                render->drawTriangleMesh(*mesh);
                render->drawTriangleMeshes({ *mesh, *mesh });
                render->drawTriangleMeshes({ mesh, mesh });
                
                render->popTransform();
                render->popTransform();
                
                render->endFrame();
            }
        }

    } // namespace Render3DTest
} // namespace djv

