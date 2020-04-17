// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the histogram widget.
        class HistogramWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(HistogramWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            HistogramWidget();

        public:
            ~HistogramWidget() override;

            static std::shared_ptr<HistogramWidget> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _initEvent(Core::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

