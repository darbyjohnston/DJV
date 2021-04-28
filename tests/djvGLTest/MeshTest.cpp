// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvGLTest/MeshTest.h>

#include <djvGL/Mesh.h>

#include <djvGeom/PointList.h>
#include <djvGeom/TriangleMesh.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::GL;

namespace djv
{
    namespace GLTest
    {
        MeshTest::MeshTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::GLTest::MeshTest", tempPath, context)
        {}
        
        void MeshTest::run()
        {
            _enum();
            _convert();
        }
        
        void MeshTest::_enum()
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
        
        void MeshTest::_convert()
        {
            {
                Geom::TriangleMesh mesh;
                Geom::triangulateBBox(Math::BBox3f(-1.F, -1.F, -1.F, 1.F, 1.F, 1.F), mesh);
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

    } // namespace GLTest
} // namespace djv

