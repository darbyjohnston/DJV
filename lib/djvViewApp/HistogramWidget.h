// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! Histogram widget.
        class HistogramWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(HistogramWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            HistogramWidget();

        public:
            ~HistogramWidget() override;

            static std::shared_ptr<HistogramWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _initEvent(System::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

