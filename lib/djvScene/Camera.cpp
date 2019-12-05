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
        void BasicCamera::setPosition(const glm::vec3& value)
        {
            _position = value;
            _update();
        }

        void BasicCamera::setTarget(const glm::vec3& value)
        {
            _target = value;
            _update();
        }

        void BasicCamera::setUp(const glm::vec3& value)
        {
            _up = value;
            _update();
        }

        void BasicCamera::setFOV(float value)
        {
            _fov = value;
            _update();
        }

        void BasicCamera::setAspect(float value)
        {
            _aspect = value;
            _update();
        }

        void BasicCamera::setClip(const FloatRange& value)
        {
            _clip = value;
            _update();
        }

        void BasicCamera::_update()
        {
            _v = glm::lookAt(_position, _target, _up);
            _p = glm::perspective(_fov, _aspect, _clip.min, _clip.max);
        }

        void PolarCamera::setTarget(const glm::vec3& value)
        {
            _target = value;
            _update();
        }

        void PolarCamera::setDistance(float value)
        {
            _distance = value;
            _update();
        }

        void PolarCamera::setLatitude(float value)
        {
            _latitude = value;
            _update();
        }

        void PolarCamera::setLongitude(float value)
        {
            _longitude = value;
            _update();
        }

        void PolarCamera::setFOV(float value)
        {
            _fov = value;
            _update();
        }

        void PolarCamera::setAspect(float value)
        {
            _aspect = value;
            _update();
        }

        void PolarCamera::setClip(const FloatRange& value)
        {
            _clip = value;
            _update();
        }

        void PolarCamera::_update()
        {
            _v = glm::translate(glm::mat4x4(1.F), glm::vec3(0.F, 0.F, -_distance));
            _v = glm::rotate(_v, Core::Math::deg2rad(_latitude), glm::vec3(1.F, 0.F, 0.F));
            _v = glm::rotate(_v, Core::Math::deg2rad(_longitude), glm::vec3(0.F, 1.F, 0.F));
            _v = glm::translate(_v, -_target);
            _p = glm::perspective(_fov, _aspect, _clip.min, _clip.max);
        }

    } // namespace Scene
} // namespace djv

