// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene/Camera.h>

#include <djvMath/Math.h>

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

        void DefaultCamera::setClip(const Math::FloatRange& value)
        {
            _data.clip = value;
            _update();
        }

        void DefaultCamera::_update()
        {
            _v = glm::lookAt(_data.position, _data.target, _data.up);
            _p = glm::perspective(Math::deg2rad(_data.fov), _data.aspect, _data.clip.getMin(), _data.clip.getMax());
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

        void PolarCamera::setClip(const Math::FloatRange& value)
        {
            _data.clip = value;
            _update();
        }

        void PolarCamera::_update()
        {
            _v = glm::translate(glm::mat4x4(1.F), glm::vec3(0.F, 0.F, -_data.distance));
            _v = glm::rotate(_v, Math::deg2rad(_data.latitude), glm::vec3(1.F, 0.F, 0.F));
            _v = glm::rotate(_v, Math::deg2rad(_data.longitude), glm::vec3(0.F, 1.F, 0.F));
            _v = glm::translate(_v, -_data.target);
            _p = glm::perspective(Math::deg2rad(_data.fov), _data.aspect, _data.clip.getMin(), _data.clip.getMax());
        }

    } // namespace Scene
} // namespace djv

