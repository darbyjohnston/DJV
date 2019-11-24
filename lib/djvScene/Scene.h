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

#include <djvCore/Core.h>

#include <memory>
#include <vector>

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            class Render;

        } // namespace Render3D
    } // namespace AV

    namespace Core
    {
        class Context;

    } // namespace Core

    //! This namespace provides scene functionality.
    namespace Scene
    {
        class ICamera;
        class ILight;
        class IPrimitive;
        class Layer;

        //! This class provides a scene.
        class Scene : public std::enable_shared_from_this<Scene>
        {
            DJV_NON_COPYABLE(Scene);
            Scene();

        public:
            static std::shared_ptr<Scene> create();

            std::vector<std::shared_ptr<IPrimitive> > getPrimitives() const;
            std::vector<std::shared_ptr<ICamera> > getCameras() const;
            std::vector<std::shared_ptr<ILight> > getLights() const;
            std::vector< std::shared_ptr<Layer> > getLayers() const;

            void addCamera(const std::shared_ptr<ICamera>&);
            void addPrimitive(const std::shared_ptr<IPrimitive>&);

        private:
            std::vector<std::shared_ptr<IPrimitive> > _primitives;
            std::vector<std::shared_ptr<ICamera> > _cameras;
            std::vector<std::shared_ptr<ILight> > _lights;
            std::vector< std::shared_ptr<Layer> > _layers;
        };

    } // namespace Scene
} // namespace djv

#include <djvScene/SceneInline.h>
