// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/ActiveFilesDialog.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the active files thumbnail widget.
        class ActiveFilesThumbnailWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ActiveFilesThumbnailWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ActiveFilesThumbnailWidget();

        public:
            ~ActiveFilesThumbnailWidget() override;

            static std::shared_ptr<ActiveFilesThumbnailWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

