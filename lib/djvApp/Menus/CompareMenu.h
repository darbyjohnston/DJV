// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Models/FilesModel.h>

#include <ftk/UI/Menu.h>

namespace djv
{
    namespace app
    {
        class App;
        class CompareActions;

        //! Compare menu.
        class CompareMenu : public ftk::Menu
        {
            FTK_NON_COPYABLE(CompareMenu);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<CompareActions>&,
                const std::shared_ptr<IWidget>& parent);

            CompareMenu();

        public:
            ~CompareMenu();

            static std::shared_ptr<CompareMenu> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<CompareActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void close() override;

        private:
            void _filesUpdate(const std::vector<std::shared_ptr<FilesModelItem> >&);
            void _bUpdate(const std::vector<int>&);

            FTK_PRIVATE();
        };
    }
}
