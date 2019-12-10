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

#pragma once

#include <djvScene/Primitive.h>

#include <djvCore/Range.h>

#include <glm/mat4x4.hpp>

#include <memory>

namespace djv
{
    namespace Scene
    {
        //! This class provides the base functionality for cameras.
        class ICamera : public IPrimitive
        {
            DJV_NON_COPYABLE(ICamera);

        protected:
            ICamera();

        public:
            virtual ~ICamera() = 0;

            const glm::mat4x4& getV() const;
            const glm::mat4x4& getP() const;

        protected:
            glm::mat4x4 _v = glm::mat4x4(1.F);
            glm::mat4x4 _p = glm::mat4x4(1.F);
        };

        //! This class provides default camera data.
        struct DefaultCameraData
        {
            float               fov         = 45.F;
            float               aspect      = 1.F;
            Core::FloatRange    clip        = Core::FloatRange(0.1F, 10000.F);
            glm::vec3           position    = glm::vec3(10.F, 20.F, -20.F);
            glm::vec3           target      = glm::vec3(0.F, 0.F, 0.F);
            glm::vec3           up          = glm::vec3(0.F, 1.F, 0.F);

            bool operator == (const DefaultCameraData&) const;
        };

        //! This class provides a default camera.
        class DefaultCamera : public ICamera
        {
        protected:
            DefaultCamera();

        public:
            static std::shared_ptr<DefaultCamera> create();

            const DefaultCameraData& getData() const;

            void setData(const DefaultCameraData&);
            void setPosition(const glm::vec3&);
            void setTarget(const glm::vec3&);
            void setUp(const glm::vec3&);
            void setFOV(float);
            void setAspect(float);
            void setClip(const Core::FloatRange&);

            std::string getClassName() const override;

        private:
            void _update();

            DefaultCameraData _data;
        };

        //! This class provides polat camera data.
        struct PolarCameraData
        {
            float               fov         = 45.F;
            float               aspect      = 1.F;
            Core::FloatRange    clip        = Core::FloatRange(.1F, 10000.F);
            glm::vec3           target      = glm::vec3(0.F, 0.F, 0.F);
            float               distance    = 20.F;
            float               latitude    = 30.F;
            float               longitude   = 30.F;

            bool operator == (const PolarCameraData&) const;
        };

        //! This class provides a polar camera.
        class PolarCamera : public ICamera
        {
        protected:
            PolarCamera();

        public:
            static std::shared_ptr<PolarCamera> create();

            const PolarCameraData& getData() const;
            float getFOV() const;
            const Core::FloatRange& getClip() const;
            const glm::vec3& getTarget() const;
            float getDistance() const;
            float getLatitude() const;
            float getLongitude() const;

            void setData(const PolarCameraData&);
            void setFOV(float);
            void setAspect(float);
            void setClip(const Core::FloatRange&);
            void setTarget(const glm::vec3&);
            void setDistance(float);
            void setLatitude(float);
            void setLongitude(float);

            std::string getClassName() const override;

        private:
            void _update();

            PolarCameraData _data;
        };

    } // namespace Scene
} // namespace djv

#include <djvScene/CameraInline.h>
