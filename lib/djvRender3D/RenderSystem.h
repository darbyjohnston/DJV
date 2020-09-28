// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>

namespace djv
{
    namespace Render3D
    {
        //! This class provides a 3D render system.
        class RenderSystem : public System::ISystem
        {
            DJV_NON_COPYABLE(RenderSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            RenderSystem();

        public:
            ~RenderSystem() override;

            static std::shared_ptr<RenderSystem> create(const std::shared_ptr<System::Context>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace Render3D
} // namespace djv
