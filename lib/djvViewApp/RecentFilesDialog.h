// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/IDialog.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class Info;

        } // namespace File
    } // namespace System

    namespace ViewApp
    {
        //! This class provides the recently used files dialog.
        class RecentFilesDialog : public UI::IDialog
        {
            DJV_NON_COPYABLE(RecentFilesDialog);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            RecentFilesDialog();

        public:
            ~RecentFilesDialog() override;

            static std::shared_ptr<RecentFilesDialog> create(const std::shared_ptr<System::Context>&);

            void setCallback(const std::function<void(const std::vector<System::File::Info>&)>&);

        protected:
            void _initEvent(System::Event::Init&) override;

        private:
            std::string _getItemCountLabel(size_t) const;

            void _itemsUpdate();
            void _selectedUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

