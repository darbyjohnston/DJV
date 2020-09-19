// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene
    {
        inline ICamera::ICamera()
        {}

        inline ICamera::~ICamera()
        {}

        inline const glm::mat4x4& ICamera::getV() const
        {
            return _v;
        }

        inline const glm::mat4x4& ICamera::getP() const
        {
            return _p;
        }

        inline bool DefaultCameraData::operator == (const DefaultCameraData& other) const
        {
            return fov == other.fov &&
                aspect == other.aspect &&
                clip == other.clip &&
                position == other.position &&
                target == other.target &&
                up == other.up;
        }

        inline DefaultCamera::DefaultCamera()
        {
            _update();
        }

        inline std::shared_ptr<DefaultCamera> DefaultCamera::create()
        {
            auto out = std::shared_ptr<DefaultCamera>(new DefaultCamera);
            return out;
        }

        inline const DefaultCameraData& DefaultCamera::getData() const
        {
            return _data;
        }

        inline std::string DefaultCamera::getClassName() const
        {
            return "DefaultCamera";
        }

        inline bool PolarCameraData::operator == (const PolarCameraData& other) const
        {
            return fov == other.fov &&
                aspect == other.aspect &&
                clip == other.clip &&
                target == other.target &&
                distance == other.distance &&
                latitude == other.latitude &&
                longitude == other.longitude;
        }

        inline PolarCamera::PolarCamera()
        {
            _update();
        }

        inline std::shared_ptr<PolarCamera> PolarCamera::create()
        {
            auto out = std::shared_ptr<PolarCamera>(new PolarCamera);
            return out;
        }
        
        inline const PolarCameraData& PolarCamera::getData() const
        {
            return _data;
        }

        inline float PolarCamera::getFOV() const
        {
            return _data.fov;
        }

        inline const Math::FloatRange& PolarCamera::getClip() const
        {
            return _data.clip;
        }
        
        inline const glm::vec3& PolarCamera::getTarget() const
        {
            return _data.target;
        }

        inline float PolarCamera::getDistance() const
        {
            return _data.distance;
        }

        inline float PolarCamera::getLatitude() const
        {
            return _data.latitude;
        }

        inline float PolarCamera::getLongitude() const
        {
            return _data.longitude;
        }

        inline std::string PolarCamera::getClassName() const
        {
            return "PolarCamera";
        }

    } // namespace Scene
} // namespace djv
