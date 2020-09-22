// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the color space widget.
        class ColorSpaceWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(ColorSpaceWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ColorSpaceWidget();

        public:
            ~ColorSpaceWidget() override;

            static std::shared_ptr<ColorSpaceWidget> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, bool> getBellowsState() const;

            void setBellowsState(const std::map<std::string, bool>&);

        protected:
            void _initLayoutEvent(System::Event::InitLayout&) override;

            void _initEvent(System::Event::Init &) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

