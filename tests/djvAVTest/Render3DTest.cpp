// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/Render3DTest.h>

#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/PointList.h>
#include <djvAV/Render3D.h>
#include <djvAV/Render3DCamera.h>
#include <djvAV/Render3DLight.h>
#include <djvAV/Render3DMaterial.h>
#include <djvAV/Render3D.h>
#include <djvAV/TriangleMesh.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::Render3D;

namespace djv
{
    namespace AVTest
    {
        Render3DTest::Render3DTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::Render3DTest", tempPath, context)
        {}
        
        void Render3DTest::run()
        {
            _enum();
            _system();
        }
        
        void Render3DTest::_enum()
        {
            for (const auto& i : getDepthBufferModeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Depth buffer mode: " + _getText(ss.str()));
            }
        }
        
        void Render3DTest::_system()
        {
            if (auto context = getContext().lock())
            {
                const Image::Size size(1280, 720);
                auto offscreenBuffer = AV::OpenGL::OffscreenBuffer::create(
                    size,
                    AV::Image::Type::RGBA_U8,
                    context->getSystemT<TextSystem>());
                offscreenBuffer->bind();
                auto render = context->getSystemT<Render3D::Render>();
                
                auto camera = DefaultCamera::create();
                
                RenderOptions options;
                options.camera = camera;
                options.size.w = 1280;
                options.size.h = 720;
                options.clip = FloatRange(.1F, 1000.F);                
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
                    BBox3f(-100.F, -100.F, -100.F, 100.F, 100.F, 100.F),
                    *mesh);
                render->drawTriangleMesh(*mesh);
                render->drawTriangleMeshes({ *mesh, *mesh });
                render->drawTriangleMeshes({ mesh, mesh });
                
                render->popTransform();
                render->popTransform();
                
                render->endFrame();
            }
        }

    } // namespace AVTest
} // namespace djv

