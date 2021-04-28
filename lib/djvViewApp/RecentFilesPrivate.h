// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/RecentFilesDialog.h>

namespace djv
{
    namespace ViewApp
    {
        //! Recent files thumbnail widget.
        class RecentFilesThumbnailWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(RecentFilesThumbnailWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            RecentFilesThumbnailWidget();

        public:
            ~RecentFilesThumbnailWidget() override;

            static std::shared_ptr<RecentFilesThumbnailWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

