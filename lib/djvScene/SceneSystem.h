// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/ISystem.h>

namespace djv
{
    namespace Scene
    {
        //! This class provides the scene system.
        class SceneSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(SceneSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            SceneSystem();

        public:
            ~SceneSystem() override;

            static std::shared_ptr<SceneSystem> create(const std::shared_ptr<Core::Context>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace Scene
} // namespace djv
