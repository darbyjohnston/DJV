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

        inline const Core::FloatRange& PolarCamera::getClip() const
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
