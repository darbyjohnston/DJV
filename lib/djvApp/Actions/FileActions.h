// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Actions/IActions.h>

namespace djv
{
    namespace app
    {
        //! File actions.
        //!
        //! \todo Add actions for opening the next/previous file in the directory.
        class FileActions : public IActions
        {
            FTK_NON_COPYABLE(FileActions);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&);

            FileActions();

        public:
            ~FileActions();

            static std::shared_ptr<FileActions> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&);

        private:
            FTK_PRIVATE();
        };
    }
}
