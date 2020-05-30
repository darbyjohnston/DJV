// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Color;
    
        } // namespace Image
    } // namespace AV

    namespace ViewApp
    {
        struct GridOptions;

        //! This class provides the image controls widget.
        class ImageControlsWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(ImageControlsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ImageControlsWidget();

        public:
            ~ImageControlsWidget() override;

            static std::shared_ptr<ImageControlsWidget> create(const std::shared_ptr<Core::Context>&);

            std::map<std::string, bool> getBellowsState() const;
            void setBellowsState(const std::map<std::string, bool>&);

        protected:
            void _initLayoutEvent(Core::Event::InitLayout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

