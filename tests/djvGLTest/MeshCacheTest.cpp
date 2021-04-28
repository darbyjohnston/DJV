// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvGLTest/MeshCacheTest.h>

#include <djvGL/MeshCache.h>

#include <djvGeom/TriangleMesh.h>

#include <djvCore/UID.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::GL;

namespace djv
{
    namespace GLTest
    {
        MeshCacheTest::MeshCacheTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::GLTest::MeshCacheTest", tempPath, context)
        {}
        
        void MeshCacheTest::run()
        {
            Geom::TriangleMesh mesh;
            Geom::TriangleMesh::triangulateBBox(Math::BBox3f(-1.F, -1.F, -1.F, 1.F, 1.F, 1.F), mesh);
            for (const auto& i : mesh.v)
            {
                mesh.c.push_back(glm::vec3(1.F, 1.F, 1.F));
            }
            const UID invalid = createUID();
            for (const auto& i : getVBOTypeEnums())
            {
                const auto data = VBO::convert(mesh, i);
                MeshCache cache(100, i);
                DJV_ASSERT(cache.getVBOSize() == 100);
                DJV_ASSERT(cache.getVBOType() == i);
                DJV_ASSERT(cache.getVBO());
                DJV_ASSERT(cache.getVAO());

                Math::SizeTRange range;
                for (size_t j = 0; j < 100; ++j)
                {
                    UID uid = cache.add(data, range);
                    Math::SizeTRange range2;
                    DJV_ASSERT(cache.get(uid, range2));
                    DJV_ASSERT(range == range2);
                }
                
                DJV_ASSERT(!cache.get(invalid, range));
                
                {
                    std::stringstream ss;
                    ss << i;
                    std::stringstream ss2;
                    ss2 << cache.getPercentageUsed();
                    _print(_getText(ss.str()) + " percentage used: " + ss2.str());
                }
            }
        }

    } // namespace GLTest
} // namespace djv

