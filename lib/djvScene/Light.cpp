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

#include <djvScene/Light.h>

#include <djvAV/Render3D.h>
#include <djvAV/Render3DLight.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        std::shared_ptr<HemisphereLight> HemisphereLight::create()
        {
            auto out = std::shared_ptr<HemisphereLight>(new HemisphereLight);
            return out;
        }

        void HemisphereLight::render(const glm::mat4x4& xform, const std::shared_ptr<AV::Render3D::Render>& render)
        {
            if (isEnabled())
            {
            }
        }

        std::shared_ptr<DirectionalLight> DirectionalLight::create()
        {
            auto out = std::shared_ptr<DirectionalLight>(new DirectionalLight);
            return out;
        }

        void DirectionalLight::render(const glm::mat4x4& xform, const std::shared_ptr<AV::Render3D::Render>& render)
        {
            if (isEnabled())
            {
                auto renderLight = AV::Render3D::DirectionalLight::create();
                glm::vec3 renderDirection = xform * glm::vec4(_direction.x, _direction.y, _direction.z, 1.F);
                glm::vec3 renderPosition = xform * glm::vec4(0.F, 0.F, 0.F, 1.F);
                renderLight->setIntensity(getIntensity());
                renderLight->setDirection(renderDirection - renderPosition);
                render->addLight(renderLight);
            }
        }

        std::shared_ptr<PointLight> PointLight::create()
        {
            auto out = std::shared_ptr<PointLight>(new PointLight);
            return out;
        }

        void PointLight::render(const glm::mat4x4& xform, const std::shared_ptr<AV::Render3D::Render>& render)
        {
            if (isEnabled())
            {
                auto renderLight = AV::Render3D::PointLight::create();
                glm::vec3 renderPosition = xform * glm::vec4(0.F, 0.F, 0.F, 1.F);
                renderLight->setIntensity(getIntensity());
                renderLight->setPosition(renderPosition);
                render->addLight(renderLight);
            }
        }

        std::shared_ptr<SpotLight> SpotLight::create()
        {
            auto out = std::shared_ptr<SpotLight>(new SpotLight);
            return out;
        }

        void SpotLight::render(const glm::mat4x4& xform, const std::shared_ptr<AV::Render3D::Render>& render)
        {
            if (isEnabled())
            {
                auto renderLight = AV::Render3D::SpotLight::create();
                glm::vec3 renderDirection = xform * glm::vec4(_direction.x, _direction.y, _direction.z, 1.F);
                glm::vec3 renderPosition = xform * glm::vec4(0.F, 0.F, 0.F, 1.F);
                renderLight->setIntensity(getIntensity());
                renderLight->setConeAngle(_coneAngle);
                renderLight->setPosition(renderPosition);
                renderLight->setDirection(renderDirection - renderPosition);
                render->addLight(renderLight);
            }
        }

    } // namespace Scene
} // namespace djv

