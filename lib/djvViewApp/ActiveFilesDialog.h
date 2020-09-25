// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/IDialog.h>

namespace djv
{
    namespace ViewApp
    {
        class Media;

        //! This class provides the active files dialog.
        class ActiveFilesDialog : public UI::IDialog
        {
            DJV_NON_COPYABLE(ActiveFilesDialog);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ActiveFilesDialog();

        public:
            ~ActiveFilesDialog() override;

            static std::shared_ptr<ActiveFilesDialog> create(const std::shared_ptr<System::Context>&);

            void setCallback(const std::function<void(const std::shared_ptr<Media>&)>&);

        protected:
            void _initEvent(System::Event::Init&) override;

        private:
            std::string _getItemCountLabel(size_t) const;

            void _itemsUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

