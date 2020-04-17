// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/ViewApp.h>

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides a background widget for displaying a logo image.
        class BackgroundImageWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(BackgroundImageWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            BackgroundImageWidget();

        public:
            ~BackgroundImageWidget() override;

            static std::shared_ptr<BackgroundImageWidget> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _paintEvent(Core::Event::Paint &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

