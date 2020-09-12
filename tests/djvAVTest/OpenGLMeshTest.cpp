// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/OpenGLMeshTest.h>

#include <djvAV/OpenGLMesh.h>
#include <djvAV/PointList.h>
#include <djvAV/TriangleMesh.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::OpenGL;

namespace djv
{
    namespace AVTest
    {
        OpenGLMeshTest::OpenGLMeshTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::OpenGLMeshTest", tempPath, context)
        {}
        
        void OpenGLMeshTest::run()
        {
            _enum();
            _convert();
        }
        
        void OpenGLMeshTest::_enum()
        {
            for (const auto& i : getVBOTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << getVertexByteCount(i);
                _print(_getText(ss.str()) + " byte count: " + ss2.str());
            }
        }
        
        void OpenGLMeshTest::_convert()
        {
            {
                Geom::TriangleMesh mesh;
                Geom::TriangleMesh::triangulateBBox(BBox3f(-1.F, -1.F, -1.F, 1.F, 1.F, 1.F), mesh);
                for (const auto& i : mesh.v)
                {
                    mesh.c.push_back(glm::vec3(1.F, 1.F, 1.F));
                }
                for (const auto& i : getVBOTypeEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    _print("Mesh: " + _getText(ss.str()));
                    const std::vector<uint8_t> data = VBO::convert(mesh, i);
                    auto vbo = VBO::create(mesh.triangles.size() * 3, i);
                    vbo->copy(data);
                    vbo->copy(data, 0);
                }
            }
            
            {
                Geom::PointList pointList;
                for (size_t i = 0; i < 100; ++i)
                {
                    pointList.v.push_back(glm::vec3(i, i, i));
                    pointList.c.push_back(glm::vec3(1.F, 1.F, 1.F));
                }
                for (const auto& i : getVBOTypeEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    _print("Point list: " + _getText(ss.str()));
                    const std::vector<uint8_t> data = VBO::convert(pointList, i);
                    auto vbo = VBO::create(pointList.v.size(), i);
                    vbo->copy(data);
                    vbo->copy(data, 0);
                }
            }
        }

    } // namespace AVTest
} // namespace djv

