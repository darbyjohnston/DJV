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

#include <djvScene/Camera.h>

#include <djvCore/Math.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        void DefaultCamera::setData(const DefaultCameraData& value)
        {
            _data = value;
            _update();
        }

        void DefaultCamera::setPosition(const glm::vec3& value)
        {
            _data.position = value;
            _update();
        }

        void DefaultCamera::setTarget(const glm::vec3& value)
        {
            _data.target = value;
            _update();
        }

        void DefaultCamera::setUp(const glm::vec3& value)
        {
            _data.up = value;
            _update();
        }

        void DefaultCamera::setFOV(float value)
        {
            _data.fov = value;
            _update();
        }

        void DefaultCamera::setAspect(float value)
        {
            _data.aspect = value;
            _update();
        }

        void DefaultCamera::setClip(const FloatRange& value)
        {
            _data.clip = value;
            _update();
        }

        void DefaultCamera::_update()
        {
            _v = glm::lookAt(_data.position, _data.target, _data.up);
            _p = glm::perspective(Math::deg2rad(_data.fov), _data.aspect, _data.clip.min, _data.clip.max);
        }

        void PolarCamera::setData(const PolarCameraData& value)
        {
            _data = value;
            _update();
        }

        void PolarCamera::setTarget(const glm::vec3& value)
        {
            _data.target = value;
            _update();
        }

        void PolarCamera::setDistance(float value)
        {
            _data.distance = value;
            _update();
        }

        void PolarCamera::setLatitude(float value)
        {
            _data.latitude = value;
            _update();
        }

        void PolarCamera::setLongitude(float value)
        {
            _data.longitude = value;
            _update();
        }

        void PolarCamera::setFOV(float value)
        {
            _data.fov = value;
            _update();
        }

        void PolarCamera::setAspect(float value)
        {
            _data.aspect = value;
            _update();
        }

        void PolarCamera::setClip(const FloatRange& value)
        {
            _data.clip = value;
            _update();
        }

        void PolarCamera::_update()
        {
            _v = glm::translate(glm::mat4x4(1.F), glm::vec3(0.F, 0.F, -_data.distance));
            _v = glm::rotate(_v, Core::Math::deg2rad(_data.latitude), glm::vec3(1.F, 0.F, 0.F));
            _v = glm::rotate(_v, Core::Math::deg2rad(_data.longitude), glm::vec3(0.F, 1.F, 0.F));
            _v = glm::translate(_v, -_data.target);
            _p = glm::perspective(Math::deg2rad(_data.fov), _data.aspect, _data.clip.min, _data.clip.max);
        }

    } // namespace Scene
} // namespace djv

