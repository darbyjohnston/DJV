// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <ftk/UI/RecentFilesModel.h>

namespace ftk
{
    class Settings;
}

namespace djv
{
    namespace app
    {
        //! Recent files model.
        class RecentFilesModel : public ftk::RecentFilesModel
        {
            FTK_NON_COPYABLE(RecentFilesModel);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::Settings>&);

            RecentFilesModel();

        public:
            ~RecentFilesModel();

            //! Create a new model.
            static std::shared_ptr<RecentFilesModel> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::Settings>&);

        private:
            FTK_PRIVATE();
        };
    }
}
