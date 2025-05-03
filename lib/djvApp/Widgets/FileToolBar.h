// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Models/FilesModel.h>

#include <dtk/ui/ToolBar.h>

namespace djv
{
    namespace app
    {
        //! File tool bar.
        class FileToolBar : public dtk::ToolBar
        {
            DTK_NON_COPYABLE(FileToolBar);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::map<std::string, std::shared_ptr<dtk::Action> >&,
                const std::shared_ptr<IWidget>& parent);

            FileToolBar() = default;

        public:
            ~FileToolBar();

            static std::shared_ptr<FileToolBar> create(
                const std::shared_ptr<dtk::Context>&,
                const std::map<std::string, std::shared_ptr<dtk::Action> >&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
