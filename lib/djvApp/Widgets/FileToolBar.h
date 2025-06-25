// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Models/FilesModel.h>

#include <feather-tk/ui/ToolBar.h>

namespace djv
{
    namespace app
    {
        //! File tool bar.
        class FileToolBar : public feather_tk::ToolBar
        {
            FEATHER_TK_NON_COPYABLE(FileToolBar);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::map<std::string, std::shared_ptr<feather_tk::Action> >&,
                const std::shared_ptr<IWidget>& parent);

            FileToolBar() = default;

        public:
            ~FileToolBar();

            static std::shared_ptr<FileToolBar> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::map<std::string, std::shared_ptr<feather_tk::Action> >&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
