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

#include <djvAV/AV.h>

#include <glm/mat4x4.hpp>

#include <memory>

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            //! This class provides the base functionality for cameras.
            class ICamera : public std::enable_shared_from_this<ICamera>
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

            //! This struct provides a default camera.
            class DefaultCamera : public ICamera
            {
            protected:
                DefaultCamera();

            public:
                static std::shared_ptr<DefaultCamera> create();

                void setV(const glm::mat4x4&);
                void setP(const glm::mat4x4&);
            };

        } // namespace Render3D
    } // namespace AV
} // namespace djv

#include <djvAV/Render3DCameraInline.h>
