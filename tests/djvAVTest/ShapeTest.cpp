// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/ShapeTest.h>

#include <djvAV/Shape.h>
#include <djvAV/TriangleMesh.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::Geom;

namespace djv
{
    namespace AVTest
    {
        ShapeTest::ShapeTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::ShapeTest", tempPath, context)
        {}
        
        void ShapeTest::run()
        {
            {
                Square square;
                square.setRadius(1.F);
                DJV_ASSERT(1.F == square.getRadius());
                TriangleMesh mesh;
                square.triangulate(mesh);
            }
            
            {
                Circle circle;
                circle.setRadius(1.F);
                circle.setResolution(100);
                DJV_ASSERT(1.F == circle.getRadius());
                DJV_ASSERT(100 == circle.getResolution());
                TriangleMesh mesh;
                circle.triangulate(mesh);
            }
            
            {
                Cube cube;
                cube.setRadius(1.F);
                DJV_ASSERT(1.F == cube.getRadius());
                TriangleMesh mesh;
                cube.triangulate(mesh);
            }
            
            {
                Sphere sphere;
                sphere.setRadius(1.F);
                const Sphere::Resolution resolution(100, 100);
                sphere.setResolution(resolution);
                const Sphere::Span uSpan(.1F, .9F);
                sphere.setUSpan(uSpan);
                const Sphere::Span vSpan(.1F, .9F);
                sphere.setVSpan(vSpan);
                sphere.setTextureSpan(false);
                DJV_ASSERT(1.F == sphere.getRadius());
                DJV_ASSERT(resolution == sphere.getResolution());
                DJV_ASSERT(uSpan == sphere.getUSpan());
                DJV_ASSERT(vSpan == sphere.getVSpan());
                DJV_ASSERT(!sphere.hasTextureSpan());
                TriangleMesh mesh;
                sphere.triangulate(mesh);
            }
            
            {
                Cylinder cylinder;
                cylinder.setRadius(1.F);
                cylinder.setLength(2.F);
                cylinder.setCapped(true);
                cylinder.setResolution(100);
                const Cylinder::Span span(.1F, .9F);
                cylinder.setSpan(span);
                cylinder.setTextureSpan(false);
                DJV_ASSERT(1.F == cylinder.getRadius());
                DJV_ASSERT(2.F == cylinder.getLength());
                DJV_ASSERT(cylinder.isCapped());
                DJV_ASSERT(100 == cylinder.getResolution());
                DJV_ASSERT(span == cylinder.getSpan());
                DJV_ASSERT(!cylinder.hasTextureSpan());
                TriangleMesh mesh;
                cylinder.triangulate(mesh);
            }
        }

    } // namespace AVTest
} // namespace djv

