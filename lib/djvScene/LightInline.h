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
        inline ILight::ILight()
        {}

        inline ILight::~ILight()
        {}

        inline bool ILight::isEnabled() const
        {
            return _enabled;
        }

        inline float ILight::getIntensity() const
        {
            return _intensity;
        }

        inline void ILight::setEnabled(bool value)
        {
            _enabled = value;
        }

        inline void ILight::setIntensity(float value)
        {
            _intensity = value;
        }

        inline DirectionalLight::DirectionalLight()
        {}

        inline const glm::vec3& DirectionalLight::getDirection() const
        {
            return _direction;
        }

        inline void DirectionalLight::setDirection(const glm::vec3& value)
        {
            _direction = value;
        }

        inline PointLight::PointLight()
        {}

        inline SpotLight::SpotLight()
        {}

        inline float SpotLight::getConeAngle() const
        {
            return _coneAngle;
        }

        inline const glm::vec3& SpotLight::getDirection() const
        {
            return _direction;
        }

        inline void SpotLight::setConeAngle(float value)
        {
            _coneAngle = value;
        }

        inline void SpotLight::setDirection(const glm::vec3& value)
        {
            _direction = value;
        }

    } // namespace Scene
} // namespace djv

