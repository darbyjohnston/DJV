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

#include <djvAV/ImageData.h>

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
        class IMaterial;
        class IPrimitive;
        class Scene;

        //! This struct provides render options.
        struct RenderOptions
        {
            std::shared_ptr<ICamera> camera;
            AV::Image::Size size;
        };

        //! This class provides a renderer.
        class Render : public std::enable_shared_from_this<Render>
        {
            DJV_NON_COPYABLE(Render);
            void _init(const std::shared_ptr<Core::Context>&);
            Render();

        public:
            static std::shared_ptr<Render> create(const std::shared_ptr<Core::Context>&);

            void setScene(const std::shared_ptr<Scene>&);

            void render(
                const std::shared_ptr<AV::Render3D::Render>&,
                const RenderOptions&);

            size_t getPrimitivesCount() const;
            size_t getTriangleCount() const;

        private:
            std::shared_ptr<IMaterial> _getMaterial(const std::shared_ptr<IPrimitive>&) const;
            void _render(
                const std::shared_ptr<IPrimitive>&,
                const std::shared_ptr<AV::Render3D::Render>&,
                const std::shared_ptr<Core::Context>&);

            DJV_PRIVATE();
        };

    } // namespace Scene
} // namespace djv
