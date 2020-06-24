// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/ISystem.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a file browser system.
        class FileBrowserSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(FileBrowserSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            FileBrowserSystem();

        public:
            ~FileBrowserSystem() override;

            static std::shared_ptr<FileBrowserSystem> create(const std::shared_ptr<Core::Context>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
