//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvScene/Render.h>

#include <djvScene/Camera.h>
#include <djvScene/Material.h>
#include <djvScene/Primitive.h>
#include <djvScene/Scene.h>

#include <djvAV/Render3D.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        struct Render::Private
        {
            std::weak_ptr<Core::Context> context;
            std::shared_ptr<Scene> scene;
            std::map<std::shared_ptr<IMaterial>, std::shared_ptr<AV::Render3D::IMaterial> > materials;
            std::map<std::shared_ptr<AV::Render3D::IMaterial>, std::vector<std::shared_ptr<AV::Geom::TriangleMesh> > > materialToMesh;
            size_t triangleCount = 0;
        };

        void Render::_init(const std::shared_ptr<Core::Context>& context)
        {
            DJV_PRIVATE_PTR();
            p.context = context;
        }

        Render::Render() :
            _p(new Private)
        {}

        std::shared_ptr<Render> Render::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<Render>(new Render);
            out->_init(context);
            return out;
        }

        void Render::setScene(const std::shared_ptr<Scene>& value)
        {
            DJV_PRIVATE_PTR();
            p.scene = value;
            p.materials.clear();
            p.materialToMesh.clear();
            if (auto context = p.context.lock())
            {
                if (p.scene)
                {
                    for (const auto& i : p.scene->getPrimitives())
                    {
                        if (const auto& mesh = i->getMesh())
                        {
                            auto material = i->getRenderMaterial();
                            const auto j = p.materials.find(material);
                            if (j == p.materials.end())
                            {
                                auto renderMaterial = material->createMaterial(context);
                                p.materialToMesh[renderMaterial].push_back(mesh);
                            }
                        }
                    }
                }
            }
        }

        void Render::render(
            const std::shared_ptr<AV::Render3D::Render>& render,
            const RenderOptions& renderOptions)
        {
            DJV_PRIVATE_PTR();
            AV::Render3D::RenderOptions render3DOptions;
            render3DOptions.camera.v = renderOptions.camera->getV();
            render3DOptions.camera.p = renderOptions.camera->getP();
            render3DOptions.size = renderOptions.size;
            render->beginFrame(render3DOptions);
            p.triangleCount = 0;
            for (const auto& i : p.materialToMesh)
            {
                render->setMaterial(i.first);
                for (const auto& j : i.second)
                {
                    render->drawTriangleMesh(*j);
                    p.triangleCount += j->triangles.size();
                }
            }
            render->endFrame();
        }

        size_t Render::getTriangleCount() const
        {
            return _p->triangleCount;
        }

    } // namespace Scene
} // namespace djv
