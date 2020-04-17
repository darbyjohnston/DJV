// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
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
        //! This class provides the annotation widget.
        class AnnotateWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(AnnotateWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            AnnotateWidget();

        public:
            ~AnnotateWidget() override;

            static std::shared_ptr<AnnotateWidget> create(const std::shared_ptr<Core::Context>&);
            
            void setColor(const AV::Image::Color&);
            void setColorCallback(const std::function<void(const AV::Image::Color)>&);

            void setLineWidth(float);
            void setLineWidthCallback(const std::function<void(float)>&);

        protected:
            void _initEvent(Core::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

