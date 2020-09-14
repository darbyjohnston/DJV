// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/OpenGLMeshCacheTest.h>

#include <djvAV/OpenGLMesh.h>
#include <djvAV/OpenGLMeshCache.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::OpenGL;

namespace djv
{
    namespace AVTest
    {
        OpenGLMeshCacheTest::OpenGLMeshCacheTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::OpenGLMeshCacheTest", tempPath, context)
        {}
        
        void OpenGLMeshCacheTest::run()
        {
            Geom::TriangleMesh mesh;
            Geom::TriangleMesh::triangulateBBox(BBox3f(-1.F, -1.F, -1.F, 1.F, 1.F, 1.F), mesh);
            for (const auto& i : mesh.v)
            {
                mesh.c.push_back(glm::vec3(1.F, 1.F, 1.F));
            }
            const UID invalid = createUID();
            for (const auto& i : getVBOTypeEnums())
            {
                const auto data = OpenGL::VBO::convert(mesh, i);
                MeshCache cache(100, i);
                DJV_ASSERT(cache.getVBOSize() == 100);
                DJV_ASSERT(cache.getVBOType() == i);
                DJV_ASSERT(cache.getVBO());
                DJV_ASSERT(cache.getVAO());

                SizeTRange range;
                for (size_t j = 0; j < 100; ++j)
                {
                    UID uid = cache.addItem(data, range);
                    SizeTRange range2;
                    DJV_ASSERT(cache.getItem(uid, range2));
                    DJV_ASSERT(range == range2);
                }
                
                DJV_ASSERT(!cache.getItem(invalid, range));
                
                {
                    std::stringstream ss;
                    ss << i;
                    std::stringstream ss2;
                    ss2 << cache.getPercentageUsed();
                    _print(_getText(ss.str()) + " percentage used: " + ss2.str());
                }
            }
        }

    } // namespace AVTest
} // namespace djv

