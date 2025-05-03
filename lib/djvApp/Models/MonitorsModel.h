// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/RecentFilesModel.h>

namespace dtk
{
    class Settings;
}

namespace tl
{
    namespace play
    {
        //! Recent files model.
        class RecentFilesModel : public dtk::RecentFilesModel
        {
            DTK_NON_COPYABLE(RecentFilesModel);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<dtk::Settings>&);

            RecentFilesModel();

        public:
            ~RecentFilesModel();

            //! Create a new model.
            static std::shared_ptr<RecentFilesModel> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<dtk::Settings>&);

        private:
            DTK_PRIVATE();
        };
    }
}
