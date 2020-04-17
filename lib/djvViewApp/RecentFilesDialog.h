// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/IDialog.h>

#include <djvUI/Enum.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class FileInfo;

        } // namespace FileSystem
    } // namespace Core

    namespace ViewApp
    {
        //! This class provides the recently used files dialog.
        class RecentFilesDialog : public UI::IDialog
        {
            DJV_NON_COPYABLE(RecentFilesDialog);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            RecentFilesDialog();

        public:
            ~RecentFilesDialog() override;

            static std::shared_ptr<RecentFilesDialog> create(const std::shared_ptr<Core::Context>&);

            void setCallback(const std::function<void(const Core::FileSystem::FileInfo&)>&);

        protected:
            void _initEvent(Core::Event::Init&) override;

        private:
            std::string _getItemCountLabel(size_t) const;

            void _recentFilesUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

